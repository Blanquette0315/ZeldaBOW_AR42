#pragma once

class CGameObject;

class CCameraMgr :
    public CSingleton<CCameraMgr>
{
private:
    map<wstring ,CGameObject*> m_mapCameraObj;

public:
    void RegisterCamera( CGameObject* _pCamObj );
    void DeregisterCamera(CGameObject* _pCamObj);

    CGameObject* GetCameraObj( wstring _strObjName );

    void Clear() { m_mapCameraObj.clear(); }

private:
    CCameraMgr();
    ~CCameraMgr();
    
    friend class CSingleton;
};

