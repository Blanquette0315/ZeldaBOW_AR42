#pragma once

#include "imgui.h"

class UI
{
private:
	string			m_strName;
	vector<UI*>		m_vecChildUI;
	UI*				m_ParentUI;

	ImVec2			m_vSize;

	bool			m_bSeperator;
	bool			m_bOpen;
	bool			m_bModal;	// 해당 UI창이 Open되면 다른 UI들과 상호작용이 안되게 해주는 옵션

public:
	const string& GetName() { return m_strName; }
	UI* GetParent() { return m_ParentUI; }

	void SetSize(ImVec2 _size) { m_vSize = _size; }
	ImVec2 GetSize() { return m_vSize; }

	void AddChild(UI* _ChildUI);

	virtual void Open() { m_bOpen = true; }
	virtual void Close() { m_bOpen = false; }
	void SetModal(bool _Modal) { m_bModal = _Modal; }
	void ShowSeperator(bool _bShow) { m_bSeperator = _bShow; }

	static void ResetFocus();

public:
	virtual void update();
	virtual void render();
	virtual void render_update() = 0;

public:
	UI(const string& _strName);
	virtual ~UI();
};

typedef void (UI::* FUNC_0)(void);
typedef void (UI::* FUNC_1)(DWORD_PTR);
typedef void (UI::* FUNC_2)(DWORD_PTR, DWORD_PTR);

