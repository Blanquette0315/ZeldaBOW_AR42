#pragma once

class CImage;

class CEngine
	: public CSingleton<CEngine>
{
private:
	HWND			m_hMainWnd;	// ���� ������ �ڵ�
	Vec2			m_vResolution; // �ػ�

public:
	void progress();
	int init(HWND _hWnd, UINT _iWidth, UINT _iHeight);

private:
	// ����
	void render();

	// �� ������ ����
	void tick();

public:
	HWND GetMainHwnd() { return m_hMainWnd; }

public:
	CEngine();
	~CEngine();
};
