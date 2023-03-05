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
    // UI의 타이틀(버튼)이 만들어지는 흐름.
    // ID가 0번으로 지정되고, 버튼이 눌렸을때 등의 색상을 지정한다.
    // 이후 해당 버튼에 문자열을 넣어 버튼을 생성해준 뒤, 지정한 컬러를 다시 제거하고 ID0번도 지워준다.
    ImGui::PushID(0);
    // HSV는 채도로 채도를 설정해 색을 설정하고 있다.
    // HSV가 직관적이지 않는다면, Color를 사용할 수도 있다.
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f, 0.8f, 0.8f));
    ImGui::Button(ToString(m_eType));
    ImGui::PopStyleColor(3);
    ImGui::PopID();
}