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

#include "CExportMgr.h"
#include "CKeyMgr.h"

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
	, m_bCurDataCooking(false)
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

	if (m_arrVertexPos != nullptr)
	{
		delete[] m_arrVertexPos;
	}
}

void CLandScape::begin()
{
	// LandStreamOut();
}

void CLandScape::finaltick()
{
	if (KEY_TAP(KEY::NUM_5))
	{
		LoadWeightData();
	}

	if (LANDSCAPE_MOD::NONE == m_eMod)
	{
		return;
	}

	Raycasting();

	BrushAreaShow(false);

	if (KEY_PRESSED(KEY::LBTN))
	{
		m_bCurDataCooking = false;

		if (LANDSCAPE_MOD::HEIGHT_MAP == m_eMod)
		{
			// 픽킹 정보를 system memory 로 가져옴
			//tRaycastOut out = {};
			//m_pCrossBuffer->GetData(&out);

			if (KEY_PRESSED(KEY::LCTRL))
			{
				m_pCSHeightMap->IsDecrease(true);
			}
			else
			{
				m_pCSHeightMap->IsDecrease(false);
			}

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

void CLandScape::render(UINT _iSubset)
{
	render();
}

void CLandScape::SetFaceCount(UINT _X, UINT _Z)
{
	m_iXFaceCount = _X;
	m_iZFaceCount = _Z;

	// 면 개수에 맞는 메시 생성
	CreateMesh();
}

void CLandScape::SetLandMod(LANDSCAPE_MOD _eMod)
{
	m_eMod = _eMod; 
	if (_eMod == LANDSCAPE_MOD::NONE)
	{
		BrushAreaShow(true); 
	}
}

void CLandScape::Cooking()
{
	if (m_arrVertexPos != nullptr)
	{
		delete[] m_arrVertexPos;
		m_arrVertexPos = nullptr;
	}

	LandStreamOut();

	Ptr<CMesh> pMesh = GetMesh();
	ComPtr<ID3D11Buffer> pVB = m_pSOBuffer;

	// vertex pos
	D3D11_BUFFER_DESC vbDesc;
	pVB->GetDesc(&vbDesc);
	UINT inumTessVtx = vbDesc.ByteWidth / sizeof(TessVtx);

	CreateCpBuffer(vbDesc.ByteWidth);

	CONTEXT->CopyResource(m_pCopyBuffer.Get(), pVB.Get());

	TessVtx* arrTessVtx = new TessVtx[inumTessVtx];

	CONTEXT->Map(m_pCopyBuffer.Get(), 0, D3D11_MAP_READ, 0, &m_MappedResource);
	memcpy(arrTessVtx, m_MappedResource.pData, sizeof(TessVtx) * inumTessVtx);
	CONTEXT->Unmap(m_pCopyBuffer.Get(), 0);

	m_inumVertices = ((inumTessVtx * 3));
	m_arrVertexPos = new Vec3[m_inumVertices];

	int idx = 0;
	for (size_t i = 0; i < inumTessVtx; ++i)
	{
		m_arrVertexPos[idx] = arrTessVtx[i].v1;
		idx++;
		m_arrVertexPos[idx] = arrTessVtx[i].v2;
		idx++;
		m_arrVertexPos[idx] = arrTessVtx[i].v3;
		idx++;
	}

	delete[] arrTessVtx;
	m_bCurDataCooking = true;
}

void CLandScape::CreateActor()
{
	if (m_bCurDataCooking == false)
	{
		Cooking();
	}

	RigidBody()->SetTerrainCollider(m_inumVertices, m_arrVertexPos, Transform()->GetRelativeScale());
}

void CLandScape::SaveCookingData()
{
	// CookingData를 파일로 세이브
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	wstring RelativePath = L"texture\\landscape\\TestCookingData.cookdata";
	strFilePath += RelativePath;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	fwrite(&m_inumVertices, sizeof(UINT), 1, pFile);

	for (int i = 0; i < m_inumVertices; ++i)
	{
		fwrite(&m_arrVertexPos[i], sizeof(Vec3), 1, pFile);
	}

	fclose(pFile);
}

void CLandScape::LoadCookingData()
{
	// CookingData를 파일로부터 로딩
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	wstring RelativePath = L"texture\\landscape\\TestCookingData.cookdata";
	strFilePath += RelativePath;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	fread(&m_inumVertices, sizeof(UINT), 1, pFile);

	for (int i = 0; i < m_inumVertices; ++i)
	{
		fread(&m_arrVertexPos[i], sizeof(Vec3), 1, pFile);
	}

	fclose(pFile);
}

void CLandScape::SaveHeightImage()
{
	// 높이 맵을 파일로 세이브
	HRESULT hr = S_OK;
	ScratchImage& pImage = m_pHeightMap->GetScratchImage();
	hr = DirectX::CaptureTexture(DEVICE, CONTEXT, m_pHeightMap->GetTex2D().Get(), pImage);
	wstring strFileName = CPathMgr::GetInst()->GetContentPath();
	strFileName += L"texture\\landscape\\TempHight.dds";
	hr = DirectX::SaveToDDSFile(pImage.GetImages(), pImage.GetImageCount(), pImage.GetMetadata(), DDS_FLAGS_NONE, strFileName.c_str());
}

void CLandScape::SaveWeightData()
{
	// 가중치 맵을 파일로 세이브
	// 가중치 구조화 버퍼 데이터 가져오기
	int iWeightCount = m_iWeightWidth * m_iWeightHeight;
	tWeight_4* vecWeight_4 = new  tWeight_4[iWeightCount];
	m_pWeightMapBuffer->GetData(vecWeight_4, sizeof(tWeight_4) * iWeightCount);
	
	// 경로
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	wstring RelativePath = L"texture\\landscape\\TestWieghtMapData.buf";
	strFilePath += RelativePath;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	fwrite(&iWeightCount, sizeof(int), 1, pFile);
	for (int i = 0; i < iWeightCount; ++i)
	{
		fwrite(&vecWeight_4[i], sizeof(tWeight_4), 1, pFile);
	}

	fclose(pFile);

	delete[] vecWeight_4;
}

void CLandScape::LoadWeightData()
{
	// 가중치 맵 데이터 로딩
	int iWeightCount = 0;
	 
	// 경로
	wstring strFilePath = CPathMgr::GetInst()->GetContentPath();
	wstring RelativePath = L"texture\\landscape\\TestWieghtMapData.buf";
	strFilePath += RelativePath;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	fread(&iWeightCount, sizeof(int), 1, pFile);

	tWeight_4* vecWeight_4 = new  tWeight_4[iWeightCount];

	for (int i = 0; i < iWeightCount; ++i)
	{
		fread(&vecWeight_4[i], sizeof(tWeight_4), 1, pFile);
	}

	fclose(pFile);

	// 버퍼에 값 셋팅
	m_pWeightMapBuffer->SetData(vecWeight_4, iWeightCount);

	delete[] vecWeight_4;
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

void CLandScape::CreateSOBuffer()
{
	if (m_pSOBuffer.Get())
		m_pSOBuffer = nullptr;

	m_iMaxVtxCount = m_iXFaceCount * m_iZFaceCount * 768;

	D3D11_BUFFER_DESC Desc = {};
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.CPUAccessFlags = 0;
	Desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	Desc.ByteWidth = sizeof(TessVtx) * m_iMaxVtxCount;
	Desc.MiscFlags = 0;

	HRESULT hr = DEVICE->CreateBuffer(&Desc, nullptr, m_pSOBuffer.GetAddressOf());
}

void CLandScape::LandStreamOut()
{
	CreateSOBuffer();

	// StreamOut Setting
	CONTEXT->SOSetTargets(1, m_pSOBuffer.GetAddressOf(), 0);

	if (nullptr == GetMesh() || nullptr == GetCurMaterial())
		return;

	Transform()->UpdateData();

	// 재질 정보 업데이트
	{
		// 지형 면 개수 전달
		m_pMaxTessMtrl->SetScalarParam(INT_0, &m_iXFaceCount);
		m_pMaxTessMtrl->SetScalarParam(INT_1, &m_iZFaceCount);

		Vec2 vResolution = Vec2(m_pHeightMap->GetWidth(), m_pHeightMap->GetHeight());
		m_pMaxTessMtrl->SetScalarParam(SCALAR_PARAM::VEC2_0, &vResolution);
		m_pMaxTessMtrl->SetTexParam(TEX_PARAM::TEX_0, GetCurMaterial()->GetTexParam(TEX_0));

		// 업데이트
		m_pMaxTessMtrl->UpdateData();
	}

	GetMesh()->render();

	// Clear
	CMaterial::Clear();
}