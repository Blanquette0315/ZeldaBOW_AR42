#include "pch.h"
#include "TestUI.h"

#include "ListUI_EX.h"
#include "ComboBoxUI.h"

TestUI::TestUI()
	: UI("TestUI")
{
	m_Combo = new ComboBoxUI;
	m_Combo->init_res<CMaterial>(2);

	m_ListEX = new ListUI_EX;
	m_ListEX->SetMultiSelect(true);
	m_ListEX->InitRes<CMaterial>(2);
}

TestUI::~TestUI()
{
	delete m_Combo;
	delete m_ListEX;
}

void TestUI::render_update()
{
	// Res 테스트
	m_Combo->render_update();
	m_ListEX->render_update();
}

void TestUI::Open()
{
	UI::Open();
}

void TestUI::Close()
{
	UI::Close();
}

