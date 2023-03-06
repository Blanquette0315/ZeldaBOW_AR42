#include "pch.h"
#include "CGameObject.h"

#include "CLevelMgr.h"
#include "CLevel.h"
#include "CLayer.h"

#include "CComponent.h"
#include "CMeshRender.h"
#include "CRenderComponent.h"
#include "CPrefab.h"

#include <Script/CScriptMgr.h>
#include "CScript.h"

CGameObject::CGameObject()
	: m_pParent(nullptr)
	, m_arrCom{}
	, m_pRenderComponent(nullptr)
	, m_iLayerIdx(-1)
	, m_bDead(false)
	, m_bRender(true)
	, m_bFrustumCul(true)
{
}

CGameObject::CGameObject(const CGameObject& _origin)
	: CEntity(_origin)
	, m_pParent(nullptr)
	, m_arrCom{}
	, m_pRenderComponent(nullptr)
	, m_iLayerIdx(-1)
	, m_bDead(false)
	, m_bRender(true)
	, m_bFrustumCul(true)
{
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr == _origin.m_arrCom[i])
			continue;

		AddComponent(_origin.m_arrCom[i]->Clone());
	}

	for (size_t i = 0; i < _origin.m_vecScripts.size(); ++i)
	{
		AddComponent(_origin.m_vecScripts[i]->Clone());
	}

	for (size_t i = 0; i < _origin.m_vecChild.size(); ++i)
	{
		AddChild(_origin.m_vecChild[i]->Clone());
	}
}

CGameObject::~CGameObject()
{
	Safe_Del_Array(m_arrCom);
	Safe_Del_Vec(m_vecScripts);
	Safe_Del_Vec(m_vecChild);
}


void CGameObject::begin()
{
	// Component
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->begin();
	}

	// Script
	for (size_t i = 0; i < m_vecScripts.size(); ++i)
	{
		m_vecScripts[i]->begin();
	}

	// Child Object
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->begin();
	}
}

void CGameObject::tick()
{
	// Component
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->tick();
	}

	// Script
	for (size_t i = 0; i < m_vecScripts.size(); ++i)
	{
		m_vecScripts[i]->tick();
	}

	// Child Object
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->tick();
	}
}

void CGameObject::finaltick()
{
	// Component
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->finaltick();
	}

	// Child Object
	vector<CGameObject*>::iterator iter = m_vecChild.begin();
	for (; iter != m_vecChild.end();)
	{
		(*iter)->finaltick();

		if ((*iter)->IsDead())
		{
			iter = m_vecChild.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	// Register Layer
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	pCurLevel->GetLayer(m_iLayerIdx)->RegisterObject(this);
}

void CGameObject::render()
{
	if (nullptr == m_pRenderComponent)
		return;

	if (m_bRender)
		m_pRenderComponent->render();
}

void CGameObject::ChangePrefab()
{
	// Prefab�� ����� �� Ű������ ����� ���ڿ��� ����� ���� ������ �����.
	wstring strKey = GetName();
	strKey += L"_Prefab_";

	// Prefab���� ����� Ŭ���� �����.
	CGameObject* temClone = Clone();

	// �ߺ��� ���� �ʵ��� �ѹ����� ���ش�.
	UINT Count = 0;

	while (true)
	{
		wstring Number = std::to_wstring(Count);

		wstring strTempName = strKey;
		strTempName += Number;

		if (nullptr != CResMgr::GetInst()->FindRes<CPrefab>(strTempName).Get())
		{
			++Count;
		}
		else
		{
			// �ߺ����� �ʴ� Ű���� ã������, �ش� Ű������ Prefab�� ������ش�.
			CResMgr::GetInst()->AddRes(strTempName, new CPrefab(temClone));
			break;
		}
	}
	
}

void CGameObject::AddComponent(CComponent* _pComponent)
{
	COMPONENT_TYPE eComType = _pComponent->GetType();

	// ��ũ��Ʈ�� �ƴ� ���
	if (COMPONENT_TYPE::SCRIPT != eComType)
	{
		assert(!m_arrCom[(UINT)eComType]);

		// �Էµ� Component�� RenderComponent���
		CRenderComponent* pRenderCom = dynamic_cast<CRenderComponent*>(_pComponent);
		if (nullptr != pRenderCom)
		{
			assert(!m_pRenderComponent); // render ��� Component�� �Ѱ��� ���� �� �ִ�.
			m_pRenderComponent = pRenderCom;
		}

		// GameObj�� Component�� ���θ� ����Ŵ.
		_pComponent->m_pOwnerObject = this;
		m_arrCom[(UINT)eComType] = _pComponent;
	}

	// ��ũ��Ʈ�� ���
	else
	{
		_pComponent->m_pOwnerObject = this;
		m_vecScripts.push_back((CScript*)_pComponent);
	}
}

void CGameObject::ReleaseComponent(CComponent* _pComponent)
{
	COMPONENT_TYPE eComType = _pComponent->GetType();

	// ��ũ��Ʈ�� �ƴѰ��
	if (COMPONENT_TYPE::SCRIPT != eComType)
	{
		if (m_arrCom[(UINT)eComType] != nullptr)
		{
			CComponent* pCompo = m_arrCom[(UINT)eComType];
			CRenderComponent* pRenderCompo = dynamic_cast<CRenderComponent*>(pCompo);
			if (nullptr != pRenderCompo)
			{
				m_pRenderComponent = nullptr;
			}

			delete pCompo;

			m_arrCom[(UINT)eComType] = nullptr;
		}
	}

	// ��ũ��Ʈ�� ���
	else
	{
		CScript* pTargetScript = (CScript*)_pComponent;
		wstring TargetName = CScriptMgr::GetScriptName(pTargetScript);

		vector<CScript*>::const_iterator iter = m_vecScripts.begin();
		vector<CScript*>::const_iterator Targetiter = m_vecScripts.end();
		for (; iter != m_vecScripts.end(); ++iter)
		{
			wstring ScriptName = {};
			ScriptName = CScriptMgr::GetScriptName(*iter);

			if (TargetName == ScriptName)
			{
				Targetiter = iter;
			}
		}

		if (Targetiter != m_vecScripts.end())
		{
			CScript* Target = *Targetiter;
			m_vecScripts.erase(Targetiter);
			delete Target;
		}
	}
}

void CGameObject::AddChild(CGameObject* _pChild)
{
	// �ڽ����� ������ ������Ʈ�� �ֻ��� �θ� ������Ʈ��.
	if (nullptr == _pChild->GetParent())
	{
		// ���Ҽ� ���̾��� ��� ���̾� ������ ���־�� �Ѵ�.
		if (-1 == _pChild->m_iLayerIdx)
		{
			_pChild->m_iLayerIdx = m_iLayerIdx;
		}
		// ���̾ �־��ٸ� ���̾ �����ϴ� �θ� vector���� ���־�� �Ѵ�.
		else
		{
			CLayer* pLayer = CLevelMgr::GetInst()->GetCurLevel()->GetLayer(_pChild->m_iLayerIdx);
			pLayer->DeregisterObject(_pChild);
		}
	}

	// �ڽ����� ������ ������Ʈ�� �ٸ� ������Ʈ�� �ڽ� ������Ʈ���ٸ�
	else
	{
		// �ڽ��� ���� �θ���踦 �����־�� �Ѵ�.
		_pChild->DisconnectFromParent();
	}

	_pChild->m_pParent = this;
	m_vecChild.push_back(_pChild);
}


CGameObject* CGameObject::GetChildObject(const wstring& _strName)
{
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		if (_strName == m_vecChild[i]->GetName())
		{
			return m_vecChild[i];
		}
	}
}

void CGameObject::DisconnectFromParent()
{
	vector<CGameObject*>::iterator iter = m_pParent->m_vecChild.begin();

	for (; iter != m_pParent->m_vecChild.end(); ++iter)
	{
		if (*iter == this)
		{
			m_pParent->m_vecChild.erase(iter);
			m_pParent = nullptr;

			return;
		}
	}

	// ���� �θ��� ChildVector�� ������ ���ٴ� ���� ���� �̻��ϴٴ� �̾߱��̹Ƿ� assert�� �ɾ��ش�.
	assert(nullptr);
}

CScript* CGameObject::GetScript(int _ScriptType)
{
	// ���� ������Ʈ�� Script�� �ϳ��� �����ϰ� ���� �ʴ´ٸ�, nullptr ��ȯ
	if (m_vecScripts.empty())
		return nullptr;

	for (int i = 0; i < m_vecScripts.size(); ++i)
	{
		if (_ScriptType == m_vecScripts[i]->GetScriptType())
		{
			return m_vecScripts[i];
		}
	}

	// ���� ã�� Ÿ���� Script�� ���ϰ� ���� �ʾҴٸ�, nullptr ��ȯ
	return nullptr;
}

void CGameObject::SetAllLayer(int _LayerIdx)
{
	m_iLayerIdx = _LayerIdx;

	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->SetAllLayer(m_iLayerIdx);
	}
}

#include "CEventMgr.h"
void CGameObject::Destroy()
{
	tEvent eve = {};
	eve.eType = EVENT_TYPE::DELETE_OBJECT;
	eve.wParam = (DWORD_PTR)this;

	CEventMgr::GetInst()->AddEvent(eve);
}