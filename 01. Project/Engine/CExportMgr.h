#pragma once

class CGameObject;
class dtNavMesh;



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

    UINT                        m_iAccumVtxCount; // accumulate vertex count for divide (vtx1,idx1) (vtx2, idx2)... 



private:
    void CreateBuffer(UINT _byteWidth);

public:
    float GetScale() { return m_fScale; }

public:
    void SaveAsObj();
    void SaveForNavMesh(CGameObject* _pObj, std::ofstream& _file);
    wstring GetProperName();

    void ClearVec() { m_vecExportObj.clear(); }
    void RegisterObj(CGameObject* _pObj) { m_vecExportObj.push_back(_pObj); }
    void AssignVector(const vector<CGameObject*>& _vecCopy) { m_vecExportObj.assign(_vecCopy.begin(), _vecCopy.end()); }
public:
    CExportMgr();
    ~CExportMgr();
};

