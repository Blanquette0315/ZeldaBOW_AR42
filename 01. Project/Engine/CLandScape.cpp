#include "pch.h"
#include "CLandScape.h"

#include "CDevice.h"
#include "CStructuredBuffer.h"

#include "CTransform.h"
#include "CRigidBody.h"
#include "CCamera.h"

#include "CRenderMgr.h"
#include "CKeyMgr.h"
#include "CResMgr.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_iXFaceCount(1)
	, m_iZFaceCount(1)
	, m_vBrushScale(Vec2(0.3f, 0.3f))
	, m_pCrossBuffer(nullptr)
	, m_pWeightMapBuffer(nullptr)
	, m_iWeightWidth(0)
	, m_iWeightHeight(0)
	, m_iWeightIdx(0)
	, m_eMod(LANDSCAPE_MOD::NONE)
{
	m_MappedResource = {};

	SetFaceCount(1, 1);

	CreateMaterial();

	CreateTexture();

	m_pCrossBuffer = new CStructuredBuffer;
	m_pCrossBuffer->Create(sizeof(tRaycastOut), 1, SB_TYPE::UAV_INC, nullptr, true);

	// 타일 텍스쳐(Color, Normal 혼합, 총 6장)	
	m_pTileArrTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\tile\\TILE_ARRR.dds", L"texture\\tile\\TILE_ARRR.dds");
	m_pTileArrTex->GenerateMip(8);

	//m_pTileArrTex = CResMgr::GetInst()->LoadTexture(L"TileArrayTex", L"texture\\tile\\TILE_ARRR.dds", 8);
}

CLandScape::~CLandScape()
{
	if (nullptr != m_pCrossBuffer)
		delete m_pCrossBuffer;

	if (nullptr != m_pWeightMapBuffer)
		delete m_pWeightMapBuffer;
}

void CLandScape::finaltick()
{
	if (KEY_TAP(KEY::NUM_0))
	{
		m_eMod = LANDSCAPE_MOD::NONE;

		BrushAreaShow(true);
	}
	else if (KEY_TAP(KEY::NUM_1))
		m_eMod = LANDSCAPE_MOD::HEIGHT_MAP;
	else if (KEY_TAP(KEY::NUM_2))
		m_eMod = LANDSCAPE_MOD::SPLAT;

	if (LANDSCAPE_MOD::NONE == m_eMod)
	{
		return;
	}

	Raycasting();

	BrushAreaShow(false);

	if (KEY_PRESSED(KEY::LBTN))
	{
		if (LANDSCAPE_MOD::HEIGHT_MAP == m_eMod)
		{
			// 픽킹 정보를 system memory 로 가져옴
			//tRaycastOut out = {};
			//m_pCrossBuffer->GetData(&out);

			// 교점 위치정보를 토대로 높이를 수정 함
			m_pCSHeightMap->SetInputBuffer(m_pCrossBuffer); // 픽킹 정보를 HeightMapShader 에 세팅
			m_pCSHeightMap->SetBrushTex(m_pBrushTex);		// 사용할 브러쉬 텍스쳐 세팅
			m_pCSHeightMap->SetBrushIndex(0);				// 브러쉬 인덱스 설정
			m_pCSHeightMap->SetBrushScale(m_vBrushScale);   // 브러쉬 크기
			m_pCSHeightMap->SetHeightMap(m_pHeightMap);
			m_pCSHeightMap->Execute();
		}
		else if (LANDSCAPE_MOD::SPLAT == m_eMod)
		{
			// 교점 위치정보를 가중치를 수정함	
			m_pCSWeightMap->SetInputBuffer(m_pCrossBuffer); // 레이 캐스트 위치
			m_pCSWeightMap->SetBrushArrTex(m_pBrushTex);
			m_pCSWeightMap->SetBrushIndex(0);
			m_pCSWeightMap->SetBrushScale(m_vBrushScale); // 브러쉬 크기
			m_pCSWeightMap->SetWeightMap(m_pWeightMapBuffer, m_iWeightWidth, m_iWeightHeight); // 가중치맵, 가로 세로 개수			
			m_pCSWeightMap->SetWeightIdx(m_iWeightIdx);
			m_pCSWeightMap->Execute();
		}
	}

	if (KEY_TAP(KEY::RIGHT))
	{
		if (LANDSCAPE_MOD::SPLAT == m_eMod)
		{
			m_iWeightIdx++;
			if (3 <= m_iWeightIdx)
				m_iWeightIdx = 0;
		}
	}


	if (KEY_TAP(KEY::NUM_5))
	{
		// 높이 맵을 파일로 세이브
		HRESULT hr = S_OK;
		ScratchImage& pImage = m_pHeightMap->GetScratchImage();
		hr = DirectX::CaptureTexture(DEVICE, CONTEXT, m_pHeightMap->GetTex2D().Get(), pImage);
		wstring strFileName = CPathMgr::GetInst()->GetContentPath();
		strFileName += L"texture\\landscape\\TempHight.dds";
		hr = DirectX::SaveToDDSFile(pImage.GetImages(), pImage.GetImageCount(), pImage.GetMetadata(), DDS_FLAGS_NONE, strFileName.c_str());
	}
}

void CLandScape::render()
{
	if (nullptr == GetMesh() || nullptr == GetCurMaterial())
		return;

	Transform()->UpdateData();

	// 재질 정보 업데이트
	{
		// 가중치 버퍼 전달
		m_pWeightMapBuffer->UpdateData(17, PIPELINE_STAGE::PS);

		// 가중치 버퍼 해상도 전달
		Vec2 vWeightMapResolution = Vec2((float)m_iWeightWidth, (float)m_iWeightHeight);
		GetCurMaterial()->SetScalarParam(SCALAR_PARAM::VEC2_1, &vWeightMapResolution);

		// 지형 면 개수 전달
		GetCurMaterial()->SetScalarParam(INT_0, &m_iXFaceCount);
		GetCurMaterial()->SetScalarParam(INT_1, &m_iZFaceCount);

		// 타일 배열 개수 전달
		float m_fTileCount = float(m_pTileArrTex->GetArraySize() / 2); // 색상, 노말 합쳐져있어서 나누기 2 해줌
		GetCurMaterial()->SetScalarParam(SCALAR_PARAM::FLOAT_1, &m_fTileCount);

		// 타일 텍스쳐 전달
		GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_ARR_0, m_pTileArrTex);
		GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_4, m_pRayMap);

		// 업데이트
		GetCurMaterial()->UpdateData();
	}

	GetMesh()->render();

	// Clear
	CMaterial::Clear();
	m_pWeightMapBuffer->Clear();
}

void CLandScape::SetFaceCount(UINT _X, UINT _Z)
{
	m_iXFaceCount = _X;
	m_iZFaceCount = _Z;

	// 면 개수에 맞는 메시 생성
	CreateMesh();
}

void CLandScape::CreateActor()
{
	Ptr<CMesh> pMesh = GetMesh();
	ComPtr<ID3D11Buffer> pVB = pMesh->GetVB();
	ComPtr<ID3D11Buffer> pIB = pMesh->GetIB();

	// vertex pos
	D3D11_BUFFER_DESC vbDesc;
	pVB->GetDesc(&vbDesc);
	UINT numVertices = vbDesc.ByteWidth / sizeof(Vtx);

	CreateCpBuffer(vbDesc.ByteWidth);

	CONTEXT->CopyResource(m_pCopyBuffer.Get(), pVB.Get());

	Vtx* arrVertices = new Vtx[numVertices];

	CONTEXT->Map(m_pCopyBuffer.Get(), 0, D3D11_MAP_READ, 0, &m_MappedResource);
	memcpy(arrVertices, m_MappedResource.pData, sizeof(Vtx) * numVertices);
	CONTEXT->Unmap(m_pCopyBuffer.Get(), 0);

	m_arrVertexPos = new Vector3[numVertices];

	for (int i = 0; i < numVertices; ++i)
	{
		m_arrVertexPos[i].x = arrVertices[i].vPos.x;
		m_arrVertexPos[i].y = arrVertices[i].vPos.y;
		m_arrVertexPos[i].z = arrVertices[i].vPos.z;
	}

	// vertex index
	D3D11_BUFFER_DESC ibDesc;
	pIB->GetDesc(&ibDesc);
	UINT numIndices = ibDesc.ByteWidth / sizeof(DWORD);

	CreateCpBuffer(ibDesc.ByteWidth);

	CONTEXT->CopyResource(m_pCopyBuffer.Get(), pIB.Get());

	m_arrIndice = new UINT[numIndices];

	CONTEXT->Map(m_pCopyBuffer.Get(), 0, D3D11_MAP_READ, 0, &m_MappedResource);
	memcpy(m_arrIndice, m_MappedResource.pData, sizeof(UINT) * numIndices);
	CONTEXT->Unmap(m_pCopyBuffer.Get(), 0);

	RigidBody()->SetTriangleCollider(numIndices, numVertices
		, m_arrIndice
		, m_arrVertexPos);

	delete[] arrVertices;
	delete[] m_arrVertexPos;
	delete[] m_arrIndice;
}


void CLandScape::Raycasting()
{
	// 시점 카메라를 가져옴
	CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == pMainCam)
		return;

	// 월드 기준 광선을 지형의 로컬로 보냄
	const Matrix& matWorldInv = Transform()->GetWorldInvMat();
	const tRay& ray = pMainCam->GetRay();

	tRay CamRay = {};
	CamRay.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	CamRay.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	CamRay.vDir.Normalize();

	// 지형과 카메라 Ray 의 교점을 구함
	tRaycastOut out = { Vec2(0.f, 0.f), 0x7fffffff, 0 };
	m_pCrossBuffer->SetData(&out, 1);

	m_pCSRaycast->SetHeightMap(m_pHeightMap);
	m_pCSRaycast->SetFaceCount(m_iXFaceCount, m_iZFaceCount);
	m_pCSRaycast->SetCameraRay(CamRay);
	m_pCSRaycast->SetOuputBuffer(m_pCrossBuffer);

	m_pCSRaycast->Execute();
}

void CLandScape::BrushAreaShow(bool _Show)
{
	m_pCSRayMap->SetInputBuffer(m_pCrossBuffer);
	m_pCSRayMap->SetBrushTex(m_pBrushTex);
	m_pCSRayMap->SetBrushIndex(0);
	m_pCSRayMap->SetBrushScale(m_vBrushScale);
	m_pCSRayMap->SetRayMap(m_pRayMap);
	m_pCSRayMap->SetBrushShow(_Show);
	m_pCSRayMap->Execute();
}

void CLandScape::CreateCpBuffer(UINT _byteWidth)
{
	if (m_pCopyBuffer.Get())
		m_pCopyBuffer = nullptr;

	m_CopyBufferDesc.Usage = D3D11_USAGE_STAGING;
	m_CopyBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	m_CopyBufferDesc.BindFlags = 0;
	m_CopyBufferDesc.ByteWidth = _byteWidth;
	m_CopyBufferDesc.MiscFlags = 0;
	HRESULT hr = DEVICE->CreateBuffer(&m_CopyBufferDesc, nullptr, m_pCopyBuffer.GetAddressOf());
}