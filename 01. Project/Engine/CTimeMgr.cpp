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
	// �ʴ� ������ 1000
	// GetTickCount();

	// �ʴ� ������ õ��
	QueryPerformanceFrequency(&m_llFrequence);

	// ���� ī����
	QueryPerformanceCounter(&m_llPrevCount);
}

void CTimeMgr::tick()
{
	QueryPerformanceCounter(&m_llCurCount);

	m_fDeltaTime = (float)(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / (float)m_llFrequence.QuadPart;

	m_llPrevCount.QuadPart = m_llCurCount.QuadPart;

	// 120 �������� �ʰ��� ��� ���� ���
	/*if ((1.f / 120.f) < m_fDeltaTime)
	{
		m_fDeltaTime = (1.f / 240.f);
	}*/

	g_global.fDT = m_fDeltaTime;
	g_global.fAccTime += m_fDeltaTime;

	m_fEffDeltaTime = m_fDeltaTime;

	// ���� �������� �ٽ� ȸ���ϴ� ȿ���� ���� DT
	// 0.55�� ���� 0.1���� DT�� �پ����ٰ� 0.55�� ���� �ٽ� 1�� DT�� �����Ѵ�.
	if (m_bPlaySlow)
	{
		// DT ����
		m_fEffAccTime += m_fEffDeltaTime;

		//m_targetSlow = 0.1f; // ���� ��ǥ�� 0.1�� �� ��
		//if (m_fEffAccTime < 0.55f)
		//{
		//	// �ð� �帧 ���� ����
		//	m_fSlowRatio = m_fEffAccTime / 0.55f;
		//	// �̹� DT ���� ���� = ��ǥ * ����
		//	m_fEffDeltaTime = m_fDeltaTime * m_targetSlow * m_fSlowRatio;
		//}
		//else if (m_fEffAccTime >= 0.55f && m_fEffAccTime < 1.1f)
		//{
		//	// �ð� �帧 ���� ����
		//	m_fSlowRatio = 1.f - (m_fEffAccTime / 0.55f);
		//	// �̹� DT ���� ���� = ��ǥ * (1-����)
		//	m_fEffDeltaTime = m_fDeltaTime * m_targetSlow * m_fSlowRatio;
		//}

		m_targetSlow = 0.05f;
		// ���� ����
		if (m_fEffAccTime <= 0.1375f)
		{
			// ���� ���� ���ϱ�
			m_fSlowRatio = m_targetSlow * (m_fEffAccTime / 0.1375f);
		}
		// ���� ����
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

	// 1 �ʿ� �ѹ�
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

		// �����ð�, ī��Ʈ �ʱ�ȭ
		m_fAccTime = 0.f;
		iCount = 0;
	}

	CFontMgr::GetInst()->DrawFont(szFloat, 10.f, 30.f, 20, FONT_RGBA(255, 20, 20, 127));
}
