#include "pch.h"
#include "CNavMgr.h"

#include "CExportMgr.h"

#define CHECK_SUCCESS if (dtCheck != DT_SUCCESS)\
{_vDir = Vec3::Zero;\
return false;}

CNavMgr::CNavMgr()
	: m_pNavMesh(nullptr)
	, m_pNavQuery(nullptr)
	, m_QueryFilter()
	, m_fExtents(10.f, 10.f, 10.f)
{
}

CNavMgr::~CNavMgr()
{
	delete m_pNavMesh;
	delete m_pNavQuery;
}

void CNavMgr::init(const wstring& _relpath)
{
	if (m_pNavMesh)
	{
		delete m_pNavMesh;
		m_pNavMesh = nullptr;
		delete m_pNavQuery;
		m_pNavQuery = nullptr;
	}

	static const int MAX_NODES = 2048;
	wstring strFullPath = CPathMgr::GetInst()->GetContentPath() + _relpath;
	m_pNavMesh = LoadNavMesh(strFullPath);
	m_pNavQuery = dtAllocNavMeshQuery();
	m_pNavQuery->init(m_pNavMesh, MAX_NODES);
}

bool CNavMgr::FindNextMoveDir(Vec3 _vStartPos, Vec3 _vEndPos, Vec3& _vDir)
{
	const UINT MAX_POLY_PATH = 200;

	static dtPolyRef startPolytRef;
	static dtPolyRef endPolyRef;
	static Vec3 vStartPolyPos, vEndPolyPos;
	static dtStatus dtCheck;
	static dtPolyRef arrPath[MAX_POLY_PATH] = {};
	static int iPathCount;

	static float vStraightPath[MAX_POLY_PATH * 3];
	static dtPolyRef arrStraightPath[MAX_POLY_PATH] = {};
	static unsigned char straightPathFlags[MAX_POLY_PATH] = {};
	static int iStraightPathCount;

	float fScale  = CExportMgr::GetInst()->GetScale();
	_vStartPos /= fScale;
	_vEndPos /= fScale;

	// find start near poly
	dtCheck = m_pNavQuery->findNearestPoly(_vStartPos, m_fExtents, &m_QueryFilter, &startPolytRef, vStartPolyPos);
	CHECK_SUCCESS;

	// find end near poly
	dtCheck = m_pNavQuery->findNearestPoly(_vEndPos, m_fExtents, &m_QueryFilter, &endPolyRef, vEndPolyPos);
	CHECK_SUCCESS;

	// find full path
	dtCheck = m_pNavQuery->findPath(startPolytRef, endPolyRef, _vStartPos, _vEndPos, &m_QueryFilter, arrPath, &iPathCount, MAX_POLY_PATH);
	CHECK_SUCCESS;

	// find straight path
	dtCheck = m_pNavQuery->findStraightPath(_vStartPos, _vEndPos, arrPath, iPathCount, vStraightPath, straightPathFlags, arrStraightPath, &iStraightPathCount, MAX_POLY_PATH);
	CHECK_SUCCESS;

	_vDir = Vec3(vStraightPath[3] - vStraightPath[0], vStraightPath[4] - vStraightPath[1], vStraightPath[5] - vStraightPath[2]).Normalize();
	
	return true;
}

void CNavMgr::CheckFailed(dtStatus _status)
{
	if (_status != DT_SUCCESS)
		assert(nullptr);
}

dtNavMesh* CNavMgr::LoadNavMesh(const wstring& _path)
{
	string path = ConvertWstrToStr(_path);
	FILE* fp = nullptr;
	fopen_s(&fp, path.c_str(), "rb");
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