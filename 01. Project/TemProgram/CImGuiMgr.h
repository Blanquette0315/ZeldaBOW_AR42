#pragma once

#include "imgui.h"

class UI;

class CImGuiMgr
	: public CSingleton<CImGuiMgr>
{
private:
	map<string, UI*>	m_mapUI;

	HANDLE				m_NotifyHandle;

public:
	void RegisterUI(UI* _UI);
	UI* FindUI(const string& _name);

public:
	void init(HWND _hWnd);
	void progress();

private:
	void clear();
	void CreateUI();
	void ObserveContent();

public:
	CImGuiMgr();
	~CImGuiMgr();
};

