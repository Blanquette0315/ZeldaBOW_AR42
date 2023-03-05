#include "pch.h"
#include "ComponentUI.h"

ComponentUI::ComponentUI(const string& _UIName, COMPONENT_TYPE _eType)
	: UI(_UIName)
	, m_TargetObj(nullptr)
	, m_eType(_eType)
{

}

ComponentUI::~ComponentUI()
{

}

void ComponentUI::render_update()
{
    // UI�� Ÿ��Ʋ(��ư)�� ��������� �帧.
    // ID�� 0������ �����ǰ�, ��ư�� �������� ���� ������ �����Ѵ�.
    // ���� �ش� ��ư�� ���ڿ��� �־� ��ư�� �������� ��, ������ �÷��� �ٽ� �����ϰ� ID0���� �����ش�.
    ImGui::PushID(0);
    // HSV�� ä���� ä���� ������ ���� �����ϰ� �ִ�.
    // HSV�� ���������� �ʴ´ٸ�, Color�� ����� ���� �ִ�.
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));
    ImGui::Button(ToString(m_eType));
    ImGui::PopStyleColor(3);
    ImGui::PopID();
}