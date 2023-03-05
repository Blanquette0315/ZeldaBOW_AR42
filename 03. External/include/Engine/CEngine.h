#pragma once

class CImage;

class CEngine
	: public CSingleton<CEngine>
{
private:
	HWND			m_hMainWnd;	// 메인 윈도우 핸들
	Vec2			m_vResolution; // 해상도


public:
	void progress();
	int init(HWND _hWnd, UINT _iWidth, UINT _iHeight);

private:
	// 랜더
	void render();

	// 매 프레임 실행
	void tick();

public:
	HWND GetMainHwnd() { return m_hMainWnd; }

public:
	CEngine();
	~CEngine();
};
