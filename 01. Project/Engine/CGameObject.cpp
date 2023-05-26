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
	, m_iLayerIdx(0)
	, m_bDead(false)
	, m_bRender(true)
	, m_bFrustumCul(true)
	, m_OwnerPrefab(nullptr)
	, m_bTimeSlow(true)
{
}

CGameObject::CGameObject(const CGameObject& _origin)
	: CEntity(_origin)
	, m_pParent(nullptr)
	, m_arrCom{}
	, m_pRenderComponent(nullptr)
	, m_iLayerIdx(_origin.m_iLayerIdx)
	, m_bDead(false)
	, m_bRender(true)
	, m_bFrustumCul(_origin.m_bFrustumCul)
	, m_OwnerPrefab(nullptr)
	, m_bTimeSlow(true)
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
		m_vecScripts[i]->init();
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
	if (!m_bTimeSlow)
	{
		m_fTimeRatioSave = g_timeslow.fRatio;
		g_timeslow.fRatio = 1.f;
	}
		
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

	if (!m_bTimeSlow)
		g_timeslow.fRatio = m_fTimeRatioSave;
}

void CGameObject::finaltick()
{
	if (!m_bTimeSlow)
	{
		m_fTimeRatioSave = g_timeslow.fRatio;
		g_timeslow.fRatio = 1.f;
	}

	if (m_pRenderComponent)
	{
		for (int i = 0; i < m_pRenderComponent->GetMtrlCount(); ++i)
		{
			m_pRenderComponent->GetCurMaterial(i)->SetLayerIdx(m_iLayerIdx);
		}
		
	}
		

	// Component
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->finaltick();
	}

	for (size_t i = 0; i < m_vecScripts.size(); ++i)
	{
		m_vecScripts[i]->finaltick();
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

	//// Register Layer
	//CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	//pCurLevel->GetLayer(m_iLayerIdx)->RegisterObject(this);

	if (!m_bTimeSlow)
		g_timeslow.fRatio = m_fTimeRatioSave;
}

void CGameObject::finaltick_module()
{
	// Component
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->finaltick_module();
	}

	// Child Object
	vector<CGameObject*>::iterator iter = m_vecChild.begin();
	for (; iter != m_vecChild.end();)
	{
		(*iter)->finaltick_module();

		if ((*iter)->IsDead())
		{
			iter = m_vecChild.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CGameObject::render()
{
	if (!m_bTimeSlow)
	{
		m_fTimeRatioSave = g_timeslow.fRatio;
		g_timeslow.fRatio = 1.f;
	}

	if (nullptr == m_pRenderComponent)
		return;

	if (m_bRender)
	{
		m_pRenderComponent->render();
	}

	if (!m_bTimeSlow)
	{
		g_timeslow.fRatio = m_fTimeRatioSave;
	}
}

void CGameObject::ChangePrefab()
{
	// Prefab을 등록할 때 키값으로 사용할 문자열을 만들기 위한 원본을 만든다.
	wstring strKey = GetName();
	strKey += L"_Prefab_";

	// Prefab으로 등록할 클론을 만든다.

	// 중복이 되지 않도록 넘버링을 해준다.
	UINT Count = 0;
	
	wstring strTempName;
	while (true)
	{
		wstring Number = std::to_wstring(Count);

		strTempName = strKey + Number;

		if (nullptr != CResMgr::GetInst()->FindRes<CPrefab>(strTempName).Get())
		{
			++Count;
		}
		else
		{

			break;
		}
	}
	// 중복되지 않는 키값을 찾았으면, 해당 키값으로 Prefab을 등록해준다.
	Ptr<CPrefab> pNewPref = new CPrefab(this);
	wstring strRelativePath;
	strRelativePath = L"prefab\\";
	strRelativePath += GetName();
	strRelativePath += L".pref";
	pNewPref->Save(strRelativePath);

	// in ui -> reload
	delete pNewPref->GetProtoObj();
	pNewPref->m_pProtoObj = nullptr;
	pNewPref = nullptr;
	//CResMgr::GetInst()->AddRes(strTempName, pNewPref.Get());
}

void CGameObject::AddComponent(CComponent* _pComponent)
{
	COMPONENT_TYPE eComType = _pComponent->GetType();

	// 스크립트가 아닌 경우
	if (COMPONENT_TYPE::SCRIPT != eComType)
	{
		assert(!m_arrCom[(UINT)eComType]);

		// 입력된 Component가 RenderComponent라면
		CRenderComponent* pRenderCom = dynamic_cast<CRenderComponent*>(_pComponent);
		if (nullptr != pRenderCom)
		{
			assert(!m_pRenderComponent); // render 기능 Component는 한개만 가질 수 있다.
			m_pRenderComponent = pRenderCom;
		}

		// GameObj와 Component가 서로를 가리킴.
		_pComponent->m_pOwnerObject = this;
		m_arrCom[(UINT)eComType] = _pComponent;
	}

	// 스크립트인 경우
	else
	{
		_pComponent->m_pOwnerObject = this;
		m_vecScripts.push_back((CScript*)_pComponent);
	}
}

void CGameObject::ReleaseComponent(CComponent* _pComponent)
{
	COMPONENT_TYPE eComType = _pComponent->GetType();

	// 스크립트가 아닌경우
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

	// 스크립트일 경우
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
	// 자식으로 들어오는 오브젝트가 최상위 부모 오브젝트다.
	if (nullptr == _pChild->GetParent())
	{
		// 무소속 레이어인 경우 레이어 지정을 해주어야 한다.
		if (-1 == _pChild->m_iLayerIdx)
		{
			_pChild->m_iLayerIdx = m_iLayerIdx;
		}
		// 레이어가 있었다면 레이어가 관리하던 부모 vector에서 빼주어야 한다.
		else
		{
			CLayer* pLayer = CLevelMgr::GetInst()->GetCurLevel()->GetLayer(_pChild->m_iLayerIdx);
			pLayer->DeregisterObject(_pChild);
		}
	}

	// 자식으로 들어오는 오브젝트가 다른 오브젝트의 자식 오브젝트였다면
	else
	{
		// 자식의 원래 부모관계를 끊어주어야 한다.
		_pChild->DisconnectFromParent();
	}

	_pChild->m_pParent = this;
	m_vecChild.push_back(_pChild);
}


CGameObject* CGameObject::GetChildObjByName(const wstring& _strName)
{
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		if (m_vecChild[i]->GetName() == _strName)
			return m_vecChild[i];
	}
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

	return nullptr;
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

	// 만약 부모의 ChildVector에 본인이 없다는 것은 뭔가 이상하다는 이야기이므로 assert를 걸어준다.
	assert(nullptr);
}

CScript* CGameObject::GetScript(int _ScriptType)
{
	// 만약 오브젝트가 Script를 하나도 보유하고 있지 않는다면, nullptr 반환
	if (m_vecScripts.empty())
		return nullptr;

	for (int i = 0; i < m_vecScripts.size(); ++i)
	{
		if (_ScriptType == m_vecScripts[i]->GetScriptType())
		{
			return m_vecScripts[i];
		}
	}

	// 만약 찾는 타입의 Script를 지니고 있지 않았다면, nullptr 반환
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

void CGameObject::SaveToYAML(YAML::Emitter& _emitter)
{
	CEntity::SaveToYAML(_emitter);

	_emitter << YAML::Key << "FrustumCulling";
	_emitter << YAML::Value << m_bFrustumCul;
	_emitter << YAML::Key << "Rendering";
	_emitter << YAML::Value << m_bRender;
	_emitter << YAML::Key << "LayerIdx";
	_emitter << YAML::Value << m_iLayerIdx;
}

void CGameObject::LoadFromYAML(YAML::Node& _node)
{
	CEntity::LoadFromYAML(_node);
	SAFE_LOAD_FROM_YAML(bool, m_bFrustumCul, _node["FrustumCulling"]);
	SAFE_LOAD_FROM_YAML(bool, m_bRender, _node["Rendering"]);
	SAFE_LOAD_FROM_YAML(int, m_iLayerIdx, _node["LayerIdx"]);
}
