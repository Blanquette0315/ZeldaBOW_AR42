#include "pch.h"
#include "CRayCastScript.h"


CRayCastScript::CRayCastScript()
	: CScript(RAYCASTSCRIPT)
{
}

CRayCastScript::~CRayCastScript()
{
	Safe_Del_Map<wstring, PhysRayCast*>(m_mapRay);
}

void CRayCastScript::begin()
{
}

void CRayCastScript::tick()
{
}

void CRayCastScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
}

void CRayCastScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
}


PhysRayCast* CRayCastScript::CreateRay(const wstring& _strKey)
{
	PhysRayCast* pRay = FindRay(_strKey);

	if (pRay == nullptr)
	{
		pRay = new PhysRayCast;
		m_mapRay.insert(make_pair(_strKey, pRay));
	}

	return pRay;
}

PhysRayCast* CRayCastScript::FindRay(const wstring& _strKey)
{
	map<wstring, PhysRayCast*>::iterator iter = m_mapRay.find(_strKey);

	if (iter == m_mapRay.end())
	{
		return nullptr;
	}
	
	return iter->second;
}


void CRayCastScript::RayCastAll()
{
	map<wstring, PhysRayCast*>::iterator iter = m_mapRay.begin();

	for (; iter != m_mapRay.end(); ++iter)
	{
		PhysX_RayCast(iter->second);
	}
}

bool CRayCastScript::RayCast(const wstring& _strKey)
{
	PhysRayCast* pRay = FindRay(_strKey);

	if (nullptr == pRay)
	{
		assert(pRay);
		return false;
	}

	return PhysX_RayCast(pRay);
}


bool CRayCastScript::SetRayStartPoint(const wstring& _strKey, Vec3 _vStartPos)
{
	PhysRayCast* pRay = FindRay(_strKey);
	if (nullptr == pRay)
		return false;
	else
	{
		pRay->SetStartOrigin(_vStartPos.x / 100.f, _vStartPos.y / 100.f, _vStartPos.z / 100.f);
		return true;
	}
}

bool CRayCastScript::SetRayDir(const wstring& _strKey, Vec3 _vRayDir)
{
	PhysRayCast* pRay = FindRay(_strKey);
	if (nullptr == pRay)
		return false;
	else
	{
		pRay->SetDirection(_vRayDir.x, _vRayDir.y, _vRayDir.z);
		return true;
	}
}

bool CRayCastScript::SetMaxDistance(const wstring& _strKey, float _fDistance)
{
	PhysRayCast* pRay = FindRay(_strKey);
	if (nullptr == pRay)
		return false;
	else
	{
		pRay->SetMaxDistance(_fDistance);
		return true;
	}
}

bool CRayCastScript::SetRayFilter(const wstring& _strKey, FILTER_GROUP _Filter)
{
	PhysRayCast* pRay = FindRay(_strKey);
	if (nullptr == pRay)
		return false;
	else
	{
		pRay->SetQueryFilterData0(_Filter);
		return true;
	}
}