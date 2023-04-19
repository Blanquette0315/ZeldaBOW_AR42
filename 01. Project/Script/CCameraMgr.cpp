#include "pch.h"
#include "CCameraMgr.h"

#include <Engine/CGameObject.h>

CCameraMgr::CCameraMgr()
	: m_arrCam{}
{
}

CCameraMgr::~CCameraMgr()
{
}

void CCameraMgr::RegisterCamera(CGameObject* _pCamObj, CAMERA_SELECTION _eCameraType)
{
	if (!_pCamObj->Camera())
		assert(nullptr); // obj don't have camera 

	m_mapCameraObj.insert({ _pCamObj->GetName(), _pCamObj });
	if (_eCameraType != CAMERA_SELECTION::END)
	{
		m_arrCam[(UINT)_eCameraType] = _pCamObj;
	}
}

void CCameraMgr::DeregisterCamera(CGameObject* _pCamObj, CAMERA_SELECTION _eCameraType)
{
	m_mapCameraObj.erase(_pCamObj->GetName());

	if (_eCameraType != CAMERA_SELECTION::END)
	{
		m_arrCam[(UINT)_eCameraType] = nullptr;
	}
}

CGameObject* CCameraMgr::GetCameraObj(wstring _strObjName)
{
	return m_mapCameraObj.find(_strObjName)->second;
}

CGameObject* CCameraMgr::GetCameraObj(CAMERA_SELECTION _eCameraType)
{
	return m_arrCam[(UINT)_eCameraType];
}
