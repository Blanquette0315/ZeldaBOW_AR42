#include "pch.h"
#include "CCameraMgr.h"

#include <Engine/CGameObject.h>

CCameraMgr::CCameraMgr()
{
}

CCameraMgr::~CCameraMgr()
{
}

void CCameraMgr::RegisterCamera(CGameObject* _pCamObj)
{
	if (!_pCamObj->Camera())
		assert(nullptr); // obj don't have camera 

	m_mapCameraObj.insert({ _pCamObj->GetName(), _pCamObj });
}

void CCameraMgr::DeregisterCamera(CGameObject* _pCamObj)
{
	m_mapCameraObj.erase(_pCamObj->GetName());
}

CGameObject* CCameraMgr::GetCameraObj(wstring _strObjName)
{
	return m_mapCameraObj.find(_strObjName)->second;
}
