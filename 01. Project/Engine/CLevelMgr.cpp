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
	// �ӽ� Level �����ϱ�
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
	// ����� Level�� ���� Collsion �浹 ���� ������ �������ش�.
	_NextLevel->RefreshCollsionSet();

	if (nullptr != pPrevLevel)
		delete pPrevLevel;
}

void CLevelMgr::ChangeLevelState(LEVEL_STATE _State)
{
	// �̹� �÷��� �����ε�, �÷��̻��°� �� ������ �ȵǱ� ������ ����ó���� ���־���.
	assert(!(m_pCurLevel->GetState() == _State));

	// �÷��� ���°� �Ǹ�, ������ ���� ��� �͵��� begin()�� ȣ������ ���̴�.
	if (LEVEL_STATE::STOP == m_pCurLevel->GetState() && LEVEL_STATE::PLAY == _State)
		m_pCurLevel->begin();

	m_pCurLevel->SetState(_State);
}
