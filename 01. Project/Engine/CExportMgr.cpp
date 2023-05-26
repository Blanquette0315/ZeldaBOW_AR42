#include "pch.h"
#include "CExportMgr.h"


#include "CDevice.h"
#include "CGameObject.h"
#include "CRenderComponent.h"
#include "CTransform.h"

#include "DetourNavMeshQuery.h"
#include "DetourCrowd.h"

UINT CExportMgr::m_iExportID = 0;

CExportMgr::CExportMgr()
	: m_fScale(10.f)
	, m_iAccumVtxCount(0)
{
}

CExportMgr::~CExportMgr()
{
}

void CExportMgr::SaveAsObj()
{
	m_iAccumVtxCount = 0;
	std::ofstream ofs(GetProperName(), std::ios::out);
	for (size_t i = 0; i < m_vecExportObj.size(); ++i)
	{
		SaveForNavMesh(m_vecExportObj[i], ofs);
	}
	ofs.close();
	ClearVec();
}

void CExportMgr::SaveForNavMesh(CGameObject* _pObj, std::ofstream& _file)
{
	/*wstring strObjName = _pObj->GetName();
	wstring strPath = GetProperName(strObjName);*/

	if (_pObj->GetRenderComponent() == nullptr)
		return;

	
	Ptr<CMesh> pMesh = _pObj->GetRenderComponent()->GetMesh();
	ComPtr<ID3D11Buffer> pVB = pMesh->GetVB();
	

	// vertex pos
	D3D11_BUFFER_DESC desc;
	pVB->GetDesc(&desc);
	UINT numVertices = desc.ByteWidth / sizeof(Vtx);

	CreateBuffer(desc.ByteWidth);

	CONTEXT->CopyResource(m_pCopyBuffer.Get(), pVB.Get());

	Vtx* arrVertices = new Vtx[numVertices];

	CONTEXT->Map(m_pCopyBuffer.Get(), 0, D3D11_MAP_READ, 0, &m_MappedResource);
	memcpy(arrVertices, m_MappedResource.pData, sizeof(Vtx) * numVertices);
	CONTEXT->Unmap(m_pCopyBuffer.Get(), 0);

	for (UINT i = 0; i < numVertices; i++)
	{
		Vec3 vPos = XMVector3TransformCoord(arrVertices[i].vPos, _pObj->Transform()->GetWorldMat());
		vPos /= m_fScale;
		_file << "v " << vPos.x << " " << vPos.y << " " << vPos.z << std::endl;
	}
	delete[] arrVertices;


	// vertex index

	// D3D11_BUFFER_DESC desc;
	for (UINT j = 0; j < pMesh->GetvecIndexInfo().size(); ++j)
	{
		ComPtr<ID3D11Buffer> pIB = pMesh->GetIB(j);
		pIB->GetDesc(&desc);
		UINT numIndices = desc.ByteWidth / sizeof(DWORD);

		CreateBuffer(desc.ByteWidth);

		CONTEXT->CopyResource(m_pCopyBuffer.Get(), pIB.Get());

		DWORD* arrIndice = new DWORD[numIndices];
		CONTEXT->Map(m_pCopyBuffer.Get(), 0, D3D11_MAP_READ, 0, &m_MappedResource);
		memcpy(arrIndice, m_MappedResource.pData, sizeof(DWORD) * numIndices);
		CONTEXT->Unmap(m_pCopyBuffer.Get(), 0);

		for (UINT i = 0; i < numIndices; i += 3)
		{
			_file << "f" <<
				" " << arrIndice[i] + 1 + m_iAccumVtxCount <<
				" " << arrIndice[i + 1] + 1 + m_iAccumVtxCount <<
				" " << arrIndice[i + 2] + 1 + m_iAccumVtxCount << "\n";
		}

		delete[] arrIndice;
	}

	m_iAccumVtxCount += numVertices;
}

wstring CExportMgr::GetProperName()
{
	wstring strPath = CPathMgr::GetInst()->GetContentPath();
	strPath += L"navmesh\\";
	strPath += L"nav_";
	strPath += std::to_wstring(m_iExportID);
	strPath += wstring(L".obj");
	++m_iExportID;

	return strPath;
}

void CExportMgr::CreateBuffer(UINT _byteWidth)
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
