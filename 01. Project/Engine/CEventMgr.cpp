#include "pch.h"
#include "CEventMgr.h"

#include "CTimeMgr.h"
#include "CLevelMgr.h"
#include "CRenderMgr.h"

#include "CCamera.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"

#include "GlobalComponent.h"

CEventMgr::CEventMgr()
	: m_bLevelStateChange(false)
{
}

CEventMgr::~CEventMgr()
{
}

void CEventMgr::tick()
{
	m_bLevelChanged = false;

	// 레벨 상태 전환을 1틱 유예한다.
	if (m_bLevelStateChange)
	{
		CLevelMgr::GetInst()->ChangeLevelState(m_eLevelState);
		m_bLevelStateChange = false;
	}

	// 메모리 정리 (dead 처리된 오브젝트들을 m_vecgarbage로 넣어두었던 것을 지운다.)
	for (size_t i = 0; i < m_vecGarbage.size(); ++i)
	{
		delete m_vecGarbage[i];
		m_bLevelChanged = true;
	}
	m_vecGarbage.clear();


	// 이벤트 처리
	for (size_t i = 0; i < m_vecEvent.size(); ++i)
	{
		switch (m_vecEvent[i].eType)
		{
		case EVENT_TYPE::CREATE_OBJECT:
		{
			// wParam : GameObject Adress
			// lParam : Layer Index
			CGameObject* pNewObj = (CGameObject*)m_vecEvent[i].wParam;
			int iLayerIdx = (int)m_vecEvent[i].lParam;

			CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
			pLevel->AddGameObject(pNewObj, iLayerIdx);
			m_bLevelChanged = true;
		}
			break;

		case EVENT_TYPE::DELETE_OBJECT:
		{
			// wParam의 Dead 처리로 바꿔주고, 해당 오브젝트를 m_vecGarbage에 넣어준다.
			// wParam : GameObject Adress
			CGameObject* pObj = (CGameObject*)m_vecEvent[i].wParam;

			if (!pObj->IsDead())
			{
				// 삭제처리할 최상위 부모만 가비지에 넣는다.
				// m_vecGarbage.push_back(pObj);

				// 삭제할 오브젝트 포함, 모든 자식 오브젝트를 Dead 체크 한다.
				static list<CGameObject*> queue;
				queue.push_back(pObj);
				while (!queue.empty())
				{
					CGameObject* pObj = queue.front();
					queue.pop_front();

					const vector<CGameObject*>& vecChild = pObj->GetChildObject();
					for (size_t i = 0; i < vecChild.size(); ++i)
					{
						queue.push_back(vecChild[i]);
					}

					// 자식의 소멸자가 호출이 안되서 자식까지 Garbage에 넣는다.
					m_vecGarbage.push_back(pObj);

					pObj->m_bDead = true;
				}
			}
		}
			break;

		case EVENT_TYPE::ADD_CHILD:
		{
			// wParam : Child Adress, lParam : Parent Adress
			CGameObject* pParent = (CGameObject*)m_vecEvent[i].lParam;
			CGameObject* pChild = (CGameObject*)m_vecEvent[i].wParam;

			pParent->AddChild(pChild);
			m_bLevelChanged = true;
		}
			break;

		case EVENT_TYPE::ADD_RES:
		{
			// wParam : RES_TYPE, lParam : Resource Adress	
			CRes* pRes = (CRes*)m_vecEvent[i].lParam;
			wstring strKey = pRes->GetKey();
			CResMgr::GetInst()->AddRes(strKey, (RES_TYPE)m_vecEvent[i].wParam, (CRes*)m_vecEvent[i].lParam);
			pRes->Release();
		}
			break;

		case EVENT_TYPE::DELETE_RES:
		{
			// wParam : RES_TYPE, lParam : Resource Adress
			CRes* pRes = (CRes*)m_vecEvent[i].lParam;
			if (!CResMgr::GetInst()->DeleteRes((RES_TYPE)m_vecEvent[i].wParam, pRes->GetKey()))
			{
				MessageBox(nullptr, L"리소스 삭제 실패", L"에러", MB_OK);
			}
			pRes->Release();
			m_bLevelChanged = true;
		}
			break;

		case EVENT_TYPE::CHANGE_LEVEL_STATE:
		{
			// wParam : Level State
			m_bLevelChanged = true;
			m_bLevelStateChange = true;
			m_eLevelState = (LEVEL_STATE)m_vecEvent[i].wParam;
			//CLevelMgr::GetInst()->ChangeLevelState((LEVEL_STATE)m_vecEvent[i].wParam);

			break;
		}

		case EVENT_TYPE::CHANGE_LEVEL:
		{
			// wParam : Level Address
			CLevelMgr::GetInst()->ChangeLevel((CLevel*)m_vecEvent[i].wParam);
			m_bLevelChanged = true;
		}
			break;

		//case EVENT_TYPE::DELETE_LEVEL:
		//{
		//	// wParam : Level Adress
		//	CLevel* _pTarget = (CLevel*)m_vecEvent[i].wParam;
		//	CLevelMgr::GetInst()->DeRegisterLevel(_pTarget->GetRelativePath());
		//	m_bLevelChanged = true;
		//}
		//	break;

		case EVENT_TYPE::CHANGE_PREFAB:
		{
			// wParam : GameObject Adress
			CGameObject* pNewObj = (CGameObject*)m_vecEvent[i].wParam;
			pNewObj->ChangePrefab();

			m_bLevelChanged = true;
		}
		break;

		case EVENT_TYPE::SLOWTIME:
		{
			// CTimeMgr::GetInst()->PlaySlow();
			g_timeslow.bActive = m_vecEvent[i].wParam;
			g_timeslow.fRatio = m_vecEvent[i].lParam;
		}
		break;

		case EVENT_TYPE::MAKE_PARENT:
		{
			CGameObject* pChildObj = (CGameObject*)m_vecEvent[i].wParam;
			CGameObject* pParentObj = pChildObj->GetParent();
			if (pParentObj)
			{
				pChildObj->DisconnectFromParent();
				CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
				pLevel->AddGameObject(pChildObj, pChildObj->GetLayerIdx());
				m_bLevelChanged = true;
			}
		}
		break;

		case EVENT_TYPE::ADD_COMPONENT:
		{
			CGameObject* pObj = (CGameObject*)m_vecEvent[i].wParam;
			COMPONENT_TYPE eType = COMPONENT_TYPE(m_vecEvent[i].lParam);

			switch (eType)
			{
			case COMPONENT_TYPE::TRANSFORM:
				pObj->AddComponent(new CTransform);
				break;
			case COMPONENT_TYPE::CAMERA:
				pObj->AddComponent(new CCamera);
				break;
			case COMPONENT_TYPE::COLLIDER:
				pObj->AddComponent(new CCollider);
				break;
			case COMPONENT_TYPE::ANIMATOR3D:
				pObj->AddComponent(new CAnimator3D);
				break;
			case COMPONENT_TYPE::LIGHT3D:
				pObj->AddComponent(new CLight3D);
				break;
			case COMPONENT_TYPE::RIGIDBODY:
				pObj->AddComponent(new CRigidBody);
				break;
			case COMPONENT_TYPE::MESHRENDER:
				pObj->AddComponent(new CMeshRender);
				break;
			case COMPONENT_TYPE::PARTICLESYSTEM:
				pObj->AddComponent(new CParticleSystem);
				break;
			case COMPONENT_TYPE::SKYBOX:
				pObj->AddComponent(new CSkyBox);
				break;
			case COMPONENT_TYPE::DECAL:
				pObj->AddComponent(new CDecal);
				break;
			case COMPONENT_TYPE::LANDSCAPE:
				pObj->AddComponent(new CLandScape);
				break;
			}
			m_bLevelChanged = true;
		}
		break;

		default:
			break;
		}
	}

	m_vecEvent.clear();
}