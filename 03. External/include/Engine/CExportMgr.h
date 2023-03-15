#pragma once

class CGameObject;
class dtNavMesh;

#include "DetourNavMesh.h"

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

class CExportMgr :
    public CSingleton<CExportMgr>
{
private:
    static UINT                 m_iExportID;

    vector<CGameObject*>        m_vecExportObj;

    bool                        m_bExportForNavMesh; // Export to make navmesh

    float                       m_fScale; // World Scale / m_fScale -> for reduce calculation cost

    ComPtr<ID3D11Buffer>        m_pCopyBuffer; // for CopyResource
    D3D11_BUFFER_DESC           m_CopyBufferDesc;
    D3D11_MAPPED_SUBRESOURCE    m_MappedResource;



private:
    void CreateBuffer(UINT _byteWidth);

public:
    void SaveAsObj();
    void SaveForNavMesh(CGameObject* _pObj);
    wstring GetProperName(const wstring& _name);

    void ClearVec() { m_vecExportObj.clear(); }
    void RegisterObj(CGameObject* _pObj) { m_vecExportObj.push_back(_pObj); }
    void AssignVector(const vector<CGameObject*>& _vecCopy) { m_vecExportObj.assign(_vecCopy.begin(), _vecCopy.end()); }

    dtNavMesh* LoadNavMesh(wstring _path);
public:
    CExportMgr();
    ~CExportMgr();
};

