#include "pch.h"
#include "CExportMgr.h"


#include "CDevice.h"
#include "CGameObject.h"
#include "CRenderComponent.h"
#include "CTransform.h"

#include "DetourNavMeshQuery.h"
#include "DetourCrowd.h"

UINT CExportMgr::m_iExportID = 0;

dtNavMesh* CExportMgr::LoadNavMesh(wstring _path)
{
	string path = ConvertWstrToStr(_path);
	FILE* fp = nullptr;
	fopen_s(&fp ,path.c_str(), "rb");
	if (!fp) return 0;

	// Read header.
	NavMeshSetHeader header;
	size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (readLen != 1)
	{
		fclose(fp);
		return 0;
	}
	//if (header.magic != NAVMESHSET_MAGIC)
	//{
	//	fclose(fp);
	//	return 0;
	//}
	//if (header.version != NAVMESHSET_VERSION)
	//{
	//	fclose(fp);
	//	return 0;
	//}

	dtNavMesh* mesh = dtAllocNavMesh();
	if (!mesh)
	{
		fclose(fp);
		return 0;
	}
	dtStatus status = mesh->init(&header.params);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return 0;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (readLen != 1)
		{
			fclose(fp);
			return 0;
		}

		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		readLen = fread(data, tileHeader.dataSize, 1, fp);
		if (readLen != 1)
		{
			dtFree(data);
			fclose(fp);
			return 0;
		}

		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
	}

	fclose(fp);

	return mesh;
}

CExportMgr::CExportMgr()
	: m_fScale(10.f)
{
}

CExportMgr::~CExportMgr()
{
}

void CExportMgr::SaveAsObj()
{
	m_iExportID = 0;
	for (size_t i = 0; i < m_vecExportObj.size(); ++i)
	{
		SaveForNavMesh(m_vecExportObj[i]);
	}
	ClearVec();
}

void CExportMgr::SaveForNavMesh(CGameObject* _pObj)
{
	wstring strObjName = _pObj->GetName();
	wstring strPath = GetProperName(strObjName);

	if (_pObj->GetRenderComponent() == nullptr)
		return;

	std::ofstream ofs(strPath, std::ios::out);
	Ptr<CMesh> pMesh = _pObj->GetRenderComponent()->GetMesh();
	ComPtr<ID3D11Buffer> pVB = pMesh->GetVB();
	ComPtr<ID3D11Buffer> pIB = pMesh->GetIB();

	// vertex pos
	{
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
			Vec3 vPos = XMVector3TransformCoord(arrVertices[i].vPos / m_fScale, _pObj->Transform()->GetWorldMat());
			ofs << "v " << vPos.x << " " << vPos.y << " " << vPos.z << std::endl;
		}
		delete[] arrVertices;
	}

	// vertex index
	{
		D3D11_BUFFER_DESC desc;
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
			ofs << "f" << " " << arrIndice[i] + 1 << " " << arrIndice[i + 1] + 1 << " " << arrIndice[i + 2] + 1 << "\n";
		}
		delete[] arrIndice;
	}
	ofs.close();
}

wstring CExportMgr::GetProperName(const wstring& _name)
{
	wstring strPath = CPathMgr::GetInst()->GetContentPath();
	strPath += L"mesh\\";
	strPath += L"nav_";
	strPath += _name;
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
