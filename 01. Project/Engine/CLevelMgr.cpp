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
	Safe_Del_Map<wstring, CLevel*>(m_mapLevel);
}

void CLevelMgr::init()
{
	// �ӽ� Level �����ϱ�
	m_pCurLevel = new CLevel;
	m_pCurLevel->SetName(L"Level 0");
	RegisterLevel(m_pCurLevel->GetName(), m_pCurLevel);
}

void CLevelMgr::progress()
{
	// ���� tick�� �ȵ��� ������ ���� ���¿��� Layer���� Clear������ ������ ��� ���̰� �Ǵ� ������ �߻��Ѵ�.
	m_pCurLevel->ClearLayer();

	if (LEVEL_STATE::PLAY == m_pCurLevel->GetState())
	{
		m_pCurLevel->tick();
	}

	// ���� ������ ���� finaltick�� �����־�� �ϴ� ������ �� ������Ʈ���� fianltick���� ��ĵ��� �������־��
	// �������� ���ֱ� ������ finaltick�� �����־�� �Ѵ�.
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
	// ���� ������ �ӽ� �����̾��� ��� �����
	//if (L"Level 0" == m_pCurLevel->GetName())
	//{
	//	m_mapLevel.erase(L"Level 0");
	//	delete m_pCurLevel;
	//}

	if (nullptr != m_pCurLevel)
	{
		m_pCurLevel->SetState(LEVEL_STATE::STOP);
	}

	m_pCurLevel = _NextLevel;
	// ����� Level�� ���� Collsion �浹 ���� ������ �������ش�.
	_NextLevel->RefreshCollsionSet();
}

void CLevelMgr::RegisterLevel(const wstring& _strKey, CLevel* _level)
{
	CLevel* pTemLevel = FindLevel(_strKey);
	assert(!pTemLevel);

	m_mapLevel.insert(make_pair(_strKey, _level));
}

void CLevelMgr::DeRegisterLevel(const wstring& _strKey)
{
	map<wstring, CLevel*>::const_iterator iter = m_mapLevel.find(_strKey);
	CLevel* TargetLevel = iter->second;
	m_mapLevel.erase(_strKey);

	delete TargetLevel;
}

CLevel* CLevelMgr::FindLevel(const wstring& _strKey)
{
	map<wstring, CLevel*>::const_iterator iter = m_mapLevel.find(_strKey);
	if (iter == m_mapLevel.end())
		return nullptr;
	else
		return iter->second;
}


void CLevelMgr::ChangeLevelState(LEVEL_STATE _State)
{
	// �̹� �÷��� �����ε�, �÷��̻��°� �� ������ �ȵǱ� ������ ����ó���� ���־���.
	assert(!(m_pCurLevel->GetState() == _State));

	m_pCurLevel->SetState(_State);

	// �÷��� ���°� �Ǹ�, ������ ���� ��� �͵��� begin()�� ȣ������ ���̴�.
	if (LEVEL_STATE::PLAY == _State)
		m_pCurLevel->begin();
}
