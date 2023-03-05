#include "pch.h"
#include "CTimeMgr.h"
#include "CEngine.h"
#include "CFontMgr.h"

CTimeMgr::CTimeMgr()
	: m_llFrequence{}
	, m_llCurCount{}
	, m_llPrevCount{}
	, m_fAccTime(0.f)
	, m_fDeltaTime(0.f)
	, m_fEffAccTime(0.f)
	, m_fEffDeltaTime(0.f)
	, m_targetSlow(0.f)
	, m_fSlowRatio(1.f)
	, m_bPlaySlow(false)
{

}

CTimeMgr::~CTimeMgr()
{

}

void CTimeMgr::init()
{
	// 초당 진동수 1000
	// GetTickCount();

	// 초당 진동수 천만
	QueryPerformanceFrequency(&m_llFrequence);

	// 현재 카운팅
	QueryPerformanceCounter(&m_llPrevCount);
}

void CTimeMgr::tick()
{
	QueryPerformanceCounter(&m_llCurCount);

	m_fDeltaTime = (float)(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / (float)m_llFrequence.QuadPart;

	m_llPrevCount.QuadPart = m_llCurCount.QuadPart;

	// 60 프레임을 초과할 경우 강제 방어
	/*if ((1.f / 60.f) < m_fDeltaTime)
	{
		m_fDeltaTime = (1.f / 60.f);
	}*/

	g_global.fDT = m_fDeltaTime;
	g_global.fAccTime += m_fDeltaTime;

	m_fEffDeltaTime = m_fDeltaTime;

	// 점점 느려졌다 다시 회복하는 효과를 위한 DT
	// 0.55초 동안 0.1까지 DT가 줄어들었다가 0.55초 동안 다시 1로 DT가 증가한다.
	if (m_bPlaySlow)
	{
		// DT 누적
		m_fEffAccTime += m_fEffDeltaTime;

		//m_targetSlow = 0.1f; // 도달 목표가 0.1배 일 때
		//if (m_fEffAccTime < 0.55f)
		//{
		//	// 시간 흐름 진행 비율
		//	m_fSlowRatio = m_fEffAccTime / 0.55f;
		//	// 이번 DT 최종 배율 = 목표 * 비율
		//	m_fEffDeltaTime = m_fDeltaTime * m_targetSlow * m_fSlowRatio;
		//}
		//else if (m_fEffAccTime >= 0.55f && m_fEffAccTime < 1.1f)
		//{
		//	// 시간 흐름 진행 비율
		//	m_fSlowRatio = 1.f - (m_fEffAccTime / 0.55f);
		//	// 이번 DT 최종 배율 = 목표 * (1-비율)
		//	m_fEffDeltaTime = m_fDeltaTime * m_targetSlow * m_fSlowRatio;
		//}

		m_targetSlow = 0.05f;
		// 비율 감소
		if (m_fEffAccTime <= 0.1375f)
		{
			// 감소 비율 구하기
			m_fSlowRatio = m_targetSlow * (m_fEffAccTime / 0.1375f);
		}
		// 비율 증가
		else
		{
			m_fSlowRatio = m_targetSlow + m_targetSlow * (m_fEffAccTime / 0.275f);
			//m_fSlowRatio = 1.f;
		}

		m_fEffDeltaTime = m_fEffDeltaTime * m_fSlowRatio;
	}
	
}

void CTimeMgr::render()
{
	static int iCount = 0;
	++iCount;

	// 1 초에 한번
	m_fAccTime += m_fDeltaTime;

	static wchar_t szFloat[50] = {};

	if (1.f < m_fAccTime)
	{
		HWND hWnd = CEngine::GetInst()->GetMainHwnd();

		//wchar_t szFloat[50] = {};
		float FPS = 1.f / m_fDeltaTime;
		swprintf_s(szFloat, 50, L"DeltaTime : %d", iCount);
		int iLen = (int)wcsnlen_s(szFloat, 50);
		//TextOut(_dc, 10, 10, szFloat, iLen);

		SetWindowText(hWnd, szFloat);

		// 누적시간, 카운트 초기화
		m_fAccTime = 0.f;
		iCount = 0;
	}

	CFontMgr::GetInst()->DrawFont(szFloat, 10.f, 30.f, 20, FONT_RGBA(255, 20, 20, 127));
}
