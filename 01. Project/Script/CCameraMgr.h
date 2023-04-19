#pragma once

class CGameObject;

class CCameraMgr :
    public CSingleton<CCameraMgr>
{
private:
    map<wstring ,CGameObject*> m_mapCameraObj;
    CGameObject* m_arrCam[(UINT)CAMERA_SELECTION::END];

public:
    void RegisterCamera( CGameObject* _pCamObj , CAMERA_SELECTION _eCameraType = CAMERA_SELECTION::END);
    void DeregisterCamera(CGameObject* _pCamObj, CAMERA_SELECTION _eCameraType);

    CGameObject* GetCameraObj( wstring _strObjName );
    CGameObject* GetCameraObj(CAMERA_SELECTION _eCameraType);

    void Clear() { m_mapCameraObj.clear(); }

private:
    CCameraMgr();
    ~CCameraMgr();
    
    friend class CSingleton;
};

