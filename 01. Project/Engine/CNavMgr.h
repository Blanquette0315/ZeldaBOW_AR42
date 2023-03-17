#pragma once
#include "singleton.h"

#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"

struct NavMeshSetHeader
{
    int magic;
    int version;
    int numTiles;
    dtNavMeshParams params;
};

struct NavMeshTileHeader
{
    dtTileRef tileRef;
    int dataSize;
};

class CNavMgr :
    public CSingleton<CNavMgr>
{
private:
    dtNavMesh* m_pNavMesh;
    dtNavMeshQuery* m_pNavQuery;
    dtQueryFilter m_QueryFilter;

    Vec3 m_fExtents;


public:
    void init(const wstring& _relpath);
    dtNavMesh* LoadNavMesh(const wstring& _path);

public:
    dtNavMesh* GetNavMesh() { return m_pNavMesh; }
    dtNavMeshQuery* GetNavMeshQuery() { m_pNavQuery; }

    Vec3 GetExtents() { return m_fExtents; }
    void SetExtents(Vec3 _vExtents) { m_fExtents = _vExtents; }

    void SetQueryFilter() {};
    void CheckFailed(dtStatus _status);

public:
    bool FindNextMoveDir(Vec3 _vStartPos, Vec3 _vEndPos, Vec3& _vDir);

public:
    CNavMgr();
    ~CNavMgr();
};

