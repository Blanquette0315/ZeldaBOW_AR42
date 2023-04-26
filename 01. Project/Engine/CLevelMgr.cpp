#include "pch.h"
#include "CLevelMgr.h"

#include "CEventMgr.h"
#include "CResMgr.h"
#include "CCollisionMgr.h"

#include "CLevel.h"
#include "CGameObject.h"

CLevelMgr::CLevelMgr()
	: m_pCurLevel(nullptr)
{
}

CLevelMgr::~CLevelMgr()
{
	delete m_pCurLevel;
}

void CLevelMgr::init()
{
	// 임시 Level 제작하기
	m_pCurLevel = new CLevel;
	m_pCurLevel->SetName(L"Level 0");
}

void CLevelMgr::refresh()
{
	m_pCurLevel->ClearLayer();
	m_pCurLevel->RegisterObjects();
}

void CLevelMgr::progress()
{
	if (LEVEL_STATE::PLAY == m_pCurLevel->GetState())
	{
		m_pCurLevel->tick();
	}

	m_pCurLevel->finaltick();
}

LEVEL_STATE CLevelMgr::GetLevelState()
{
	return m_pCurLevel->GetState();
}

CGameObject* CLevelMgr::FindObjectByName(const wstring& _name)
{
	return m_pCurLevel->FindObjectByName(_name);
}

void CLevelMgr::FindObjectByName(const wstring& _name, vector<CGameObject*>& _out)
{
	m_pCurLevel->FindObjectByName(_name, _out);
}

void CLevelMgr::ChangeLevel(CLevel* _NextLevel)
{
	CLevel* pPrevLevel = nullptr;

	if (nullptr != m_pCurLevel)
	{
		m_pCurLevel->SetState(LEVEL_STATE::STOP);
		pPrevLevel = m_pCurLevel;
	}
	
	m_pCurLevel = _NextLevel;
	// 변경될 Level이 지닌 Collsion 충돌 설정 값으로 변경해준다.
	_NextLevel->RefreshCollsionSet();

	if (nullptr != pPrevLevel)
		delete pPrevLevel;
}

void CLevelMgr::ChangeLevelState(LEVEL_STATE _State)
{
	// 이미 플레이 상태인데, 플레이상태가 또 들어오면 안되기 때문에 예외처리를 해주었다.
	assert(!(m_pCurLevel->GetState() == _State));

	// 플레이 상태가 되면, 레벨이 지닌 모든 것들을 begin()을 호출해줄 것이다.
	if (LEVEL_STATE::STOP == m_pCurLevel->GetState() && LEVEL_STATE::PLAY == _State)
		m_pCurLevel->begin();

	m_pCurLevel->SetState(_State);
}
