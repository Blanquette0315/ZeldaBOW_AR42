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

	// Ÿ�� �ؽ���(Color, Normal ȥ��, �� 6��)	
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
			// ��ŷ ������ system memory �� ������
			//tRaycastOut out = {};
			//m_pCrossBuffer->GetData(&out);

			// ���� ��ġ������ ���� ���̸� ���� ��
			m_pCSHeightMap->SetInputBuffer(m_pCrossBuffer); // ��ŷ ������ HeightMapShader �� ����
			m_pCSHeightMap->SetBrushTex(m_pBrushTex);		// ����� �귯�� �ؽ��� ����
			m_pCSHeightMap->SetBrushIndex(0);				// �귯�� �ε��� ����
			m_pCSHeightMap->SetBrushScale(m_vBrushScale);   // �귯�� ũ��
			m_pCSHeightMap->SetHeightMap(m_pHeightMap);
			m_pCSHeightMap->Execute();
		}
		else if (LANDSCAPE_MOD::SPLAT == m_eMod)
		{
			// ���� ��ġ������ ����ġ�� ������	
			m_pCSWeightMap->SetInputBuffer(m_pCrossBuffer); // ���� ĳ��Ʈ ��ġ
			m_pCSWeightMap->SetBrushArrTex(m_pBrushTex);
			m_pCSWeightMap->SetBrushIndex(0);
			m_pCSWeightMap->SetBrushScale(m_vBrushScale); // �귯�� ũ��
			m_pCSWeightMap->SetWeightMap(m_pWeightMapBuffer, m_iWeightWidth, m_iWeightHeight); // ����ġ��, ���� ���� ����			
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
		// ���� ���� ���Ϸ� ���̺�
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

	// ���� ���� ������Ʈ
	{
		// ����ġ ���� ����
		m_pWeightMapBuffer->UpdateData(17, PIPELINE_STAGE::PS);

		// ����ġ ���� �ػ� ����
		Vec2 vWeightMapResolution = Vec2((float)m_iWeightWidth, (float)m_iWeightHeight);
		GetCurMaterial()->SetScalarParam(SCALAR_PARAM::VEC2_1, &vWeightMapResolution);

		// ���� �� ���� ����
		GetCurMaterial()->SetScalarParam(INT_0, &m_iXFaceCount);
		GetCurMaterial()->SetScalarParam(INT_1, &m_iZFaceCount);

		// Ÿ�� �迭 ���� ����
		float m_fTileCount = float(m_pTileArrTex->GetArraySize() / 2); // ����, �븻 �������־ ������ 2 ����
		GetCurMaterial()->SetScalarParam(SCALAR_PARAM::FLOAT_1, &m_fTileCount);

		// Ÿ�� �ؽ��� ����
		GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_ARR_0, m_pTileArrTex);
		GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_4, m_pRayMap);

		// ������Ʈ
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

	// �� ������ �´� �޽� ����
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
	// ���� ī�޶� ������
	CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == pMainCam)
		return;

	// ���� ���� ������ ������ ���÷� ����
	const Matrix& matWorldInv = Transform()->GetWorldInvMat();
	const tRay& ray = pMainCam->GetRay();

	tRay CamRay = {};
	CamRay.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	CamRay.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	CamRay.vDir.Normalize();

	// ������ ī�޶� Ray �� ������ ����
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