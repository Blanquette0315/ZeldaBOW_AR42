#include "pch.h"
#include "CEngine.h"

#include "CDevice.h"

#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CLevelMgr.h"
#include "CPathMgr.h"
#include "CResMgr.h"
#include "CRenderMgr.h"
#include "CEventMgr.h"
#include "CCollisionMgr.h"
#include "CFontMgr.h"
#include "CSound.h"
#include "CPhysMgr.h"
#include "CSoundMgr.h"

#include <PhysEngine/Export/PhysCollider.h>

CEngine::CEngine()
	: m_hMainWnd(nullptr)
	//, m_vResolution{} Vector2의 경우 기본 생성자가 구현이 되어 있기 때문에 굳이 초기화를 해주지 않아도 기본 생성자로 만들어진다.
{

}

CEngine::~CEngine()
{
}

void CEngine::progress()
{
	tick();
	render();
}

int CEngine::init(HWND _hWnd, UINT _iWidth, UINT _iHeight)
{
	m_hMainWnd = _hWnd;
	m_vResolution.x = (float)_iWidth;
	m_vResolution.y = (float)_iHeight;

	// 비트맵 해상도를 설정하기 위해 실제 윈도우 크기 계산
	RECT rt = { 0,0,(LONG)_iWidth, (LONG)_iHeight };
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);

	// 윈도우 크기 변경 및 Show 설정
	SetWindowPos(m_hMainWnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);
	ShowWindow(m_hMainWnd, true);

	// Mgr 초기화
	if (FAILED(CDevice::GetInst()->init(m_hMainWnd, m_vResolution)))
	{
		return E_FAIL;
	}

	CPathMgr::GetInst()->init();
	CKeyMgr::GetInst()->init();
	CTimeMgr::GetInst()->init();

	CResMgr::GetInst()->init();
	CRenderMgr::GetInst()->init();
	CLevelMgr::GetInst()->init();
	CFontMgr::GetInst()->init();

	CPhysMgr::GetInst()->init();

	return S_OK;
}

void CEngine::tick()
{
	// Mgr 업데이트
	CResMgr::GetInst()->tick();
	CSound::g_pFMOD->update();
	CKeyMgr::GetInst()->tick();
	CRenderMgr::GetInst()->tick();
	CLevelMgr::GetInst()->refresh();
	CPhysMgr::GetInst()->tick();
	CCollisionMgr::GetInst()->tick();

	CLevelMgr::GetInst()->progress();

	CSoundMgr::GetInst()->tick();
}

void CEngine::render()
{
	CRenderMgr::GetInst()->render();
}
