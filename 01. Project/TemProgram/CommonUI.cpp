#include "pch.h"
#include "CommonUI.h"

UINT CommonUI::iCommonUIID = 0;
vector<bool> CommonUI::vecPickerChange = {};

void CommonUI::CreateColorPicker(Vec4& _vecColor)
{
	static bool bPickerChange = false;

	if (iCommonUIID >= vecPickerChange.size())
	{
		vecPickerChange.push_back(bPickerChange);
	}

	char InputFloatName[50] = "";
	sprintf_s(InputFloatName, 50, "##ColorInput%d", iCommonUIID);

	ImGui::PushItemWidth(210);
	ImGui::InputFloat3(InputFloatName, _vecColor);
	ImGui::PopItemWidth();

	ImGui::SameLine();

	char PickerName[50] = "";
	sprintf_s(PickerName, 50, "##Pciker%d", iCommonUIID);

	if (!vecPickerChange[iCommonUIID])
	{
		ImGui::ColorEdit3(PickerName, _vecColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_PickerHueBar);
	}
	else
	{
		ImGui::ColorEdit4(PickerName, _vecColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_PickerHueWheel);
	}
	ImGui::SameLine();

	char PickerboolName[50] = "";
	sprintf_s(PickerboolName, 50, "##Pickerbool%d", iCommonUIID);

	if (ImGui::Button(PickerboolName, ImVec2(18.f,18.f)))
	{
		vecPickerChange[iCommonUIID] = !vecPickerChange[iCommonUIID];
	}

	++iCommonUIID;
}

void CommonUI::vecPickerClear()
{
	vecPickerChange.clear();
}