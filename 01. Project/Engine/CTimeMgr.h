#pragma once

class CTimeMgr
	: public CSingleton<CTimeMgr>
{
private:
	LARGE_INTEGER	m_llFrequence;
	LARGE_INTEGER	m_llCurCount;
	LARGE_INTEGER   m_llPrevCount;

	float			m_fAccTime;
	float			m_fDeltaTime;

	// 느려지는 효과를 위한 DT
	float			m_fEffAccTime;
	float			m_fEffDeltaTime;
	
	float			m_targetSlow;
	float			m_fSlowRatio;

	bool			m_bPlaySlow;

public:
	void init();
	void tick();
	void render();

public:
	float GetDeltaTime() { return m_fDeltaTime; }
	float GetEffectDeltaTime() { return m_fEffDeltaTime; }

	void PlaySlow() { m_bPlaySlow = true; }
	void ReleaseSlow() { m_fSlowRatio = 0.f; m_fEffAccTime = 0.f; m_bPlaySlow = false; }

public:
	CTimeMgr();
	~CTimeMgr();

};

