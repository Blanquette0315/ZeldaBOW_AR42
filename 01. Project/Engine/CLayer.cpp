#include "pch.h"
#include "CLayer.h"

#include "CGameObject.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
	Safe_Del_Vec(m_vecParent);
}


void CLayer::begin()
{
	for (size_t i = 0; i < m_vecParent.size(); ++i)
	{
		m_vecParent[i]->begin();
	}
}

void CLayer::tick()
{
	for (size_t i = 0; i < m_vecParent.size(); ++i)
	{
		m_vecParent[i]->tick();
	}
}

void CLayer::finaltick()
{
	vector<CGameObject*>::iterator iter = m_vecParent.begin();

	for (; iter != m_vecParent.end();)
	{
		(*iter)->finaltick();

		if ((*iter)->IsDead())
		{
			iter = m_vecParent.erase(iter);
			int i = 0;
		}
		else
		{
			++iter;
		}
	}
}

void CLayer::AddGameObject(CGameObject* _pObject)
{
	m_vecParent.push_back(_pObject);

	static list<CGameObject*> queue;
	queue.push_back(_pObject);

	while (!queue.empty())
	{
		CGameObject* pObj = queue.front();
		queue.pop_front();

		const vector<CGameObject*>& vecChild = pObj->GetChildObject();
		for (size_t i = 0; i < vecChild.size(); ++i)
		{
			queue.push_back(vecChild[i]);
		}

		pObj->m_iLayerIdx = m_iLayerIdx;
	}
}


void CLayer::DeregisterObject(CGameObject* _pObj)
{
	vector<CGameObject*>::iterator iter = m_vecParent.begin();

	for (; iter != m_vecParent.end(); ++iter)
	{
		if (*iter == _pObj)
		{
			m_vecParent.erase(iter);
			return;
		}

		// 최상위 부모오브젝트가, 해당 레이어 소속인데 ParentVector에 없다면
		if(iter == m_vecParent.end())
			assert(nullptr);
	}
}