#include "pch.h"
#include "CLevel.h"

#include "CCollisionMgr.h"

#include "CRenderMgr.h"

#include "CLayer.h"
#include "CGameObject.h"

CLevel::CLevel()
	: m_State(LEVEL_STATE::STOP)
{
	m_arrLayer[0].SetName(L"Default");

	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i].m_iLayerIdx = i;
	}
}

CLevel::~CLevel()
{
}

void CLevel::begin()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i].begin();
	}
}

void CLevel::tick()
{
	//for (UINT i = 0; i < MAX_LAYER; ++i)
	//{
	//	m_arrLayer[i].clear();
	//}

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i].tick();
	}
}

void CLevel::finaltick()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i].finaltick();
	}
}

void CLevel::ClearLayer()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i].clear();
	}
}

void CLevel::RefreshCollsionSet()
{
	CCollisionMgr::GetInst()->clear();
	for (size_t i = 0; i < m_vecLayerCollsionSet.size(); ++i)
	{
		CCollisionMgr::GetInst()->CollisionLayerCheck(m_vecLayerCollsionSet[i].iLeft, m_vecLayerCollsionSet[i].iRight);
	}
}

CLayer* CLevel::GetLayer(const wstring& _strLayerName)
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		if (_strLayerName == m_arrLayer[i].GetName())
		{
			return &m_arrLayer[i];
		}
	}
	return nullptr;
}

void CLevel::AddGameObject(CGameObject* _pObjet, UINT _iLayerIdx)
{
	assert(0 <= _iLayerIdx && _iLayerIdx < MAX_LAYER);

	m_arrLayer[_iLayerIdx].AddGameObject(_pObjet);
	
	if (m_State != LEVEL_STATE::STOP)
	{
		_pObjet->begin();
	}
}

void CLevel::AddGameObejct(CGameObject* _pObject, const wstring& _strLayerName)
{
	CLayer* pLayer = GetLayer(_strLayerName);

	assert(pLayer);

	pLayer->AddGameObject(_pObject);
}

void CLevel::ChangeObjectLayer(CGameObject* _pObject, int _Idx)
{
	m_arrLayer[(UINT)_pObject->GetLayerIdx()].DeregisterObject(_pObject);
	AddGameObject(_pObject, _Idx);
}

CGameObject* CLevel::FindObjectByName(const wstring& _name)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i].GetObjects();

		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (_name == vecObjects[j]->GetName())
			{
				return vecObjects[j];
			}
		}
	}

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i].GetParentObject();

		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (_name == vecObjects[j]->GetName())
			{
				return vecObjects[j];
			}
		}
	}

	return nullptr;
}

void CLevel::FindObjectByName(const wstring& _name, vector<CGameObject*>& _out)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i].GetObjects();

		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (_name == vecObjects[j]->GetName())
			{
				_out.push_back(vecObjects[j]);
			}
		}
	}
}

const vector<CGameObject*> CLevel::GetGameObjects()
{
	vector<CGameObject*> temVec = {};
	for (size_t i = 0; i < MAX_LAYER; ++i)
	{
		for (size_t j = 0; j < m_arrLayer[i].GetObjects().size(); ++j)
		{
			if (IsValid(m_arrLayer[i].GetObjects()[j]))
			{
				temVec.push_back(m_arrLayer[i].GetObjects()[j]);
			}
		}
	}

	return temVec;
}

void CLevel::RegisterObjects()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecParent = m_arrLayer[i].GetParentObject();
		for (size_t j = 0; j < vecParent.size(); ++j)
		{
			m_arrLayer[vecParent[j]->GetLayerIdx()].RegisterObject(vecParent[j]);
			if (vecParent[j]->Camera())
			{
				CRenderMgr::GetInst()->RegisterCamera(vecParent[j]->Camera());
			}
			const vector<CGameObject*>& vecChild = vecParent[j]->GetChildObject();
			for (size_t k = 0; k < vecChild.size(); ++k)
			{
				m_arrLayer[vecChild[k]->GetLayerIdx()].RegisterObject(vecChild[k]);
				if (vecChild[k]->Camera())
				{
					CRenderMgr::GetInst()->RegisterCamera(vecChild[k]->Camera());
				}
			}
		}
	}
}

void CLevel::SaveToYAML(YAML::Emitter& _emitter)
{
	CEntity::SaveToYAML(_emitter);
	_emitter << YAML::Key << "RelativePath";
	_emitter << YAML::Value << WStringToString(m_strRelativePath);
	_emitter << YAML::Key << "LayerCollisionSet";
	_emitter << YAML::Value << m_vecLayerCollsionSet;
}

void CLevel::LoadFromYAML(YAML::Node& _node)
{
	CEntity::LoadFromYAML(_node);
	m_strRelativePath = StringToWString(_node["RelativePath"].as<string>());
	m_vecLayerCollsionSet = _node["LayerCollisionSet"].as<vector<tLayerCollision>>();

	// 레이어 설정해주기
	for (size_t i = 0; i < m_vecLayerCollsionSet.size(); ++i)
	{
		CCollisionMgr::GetInst()->CollisionLayerCheck(m_vecLayerCollsionSet[i].iLeft, m_vecLayerCollsionSet[i].iRight);
	}
}

