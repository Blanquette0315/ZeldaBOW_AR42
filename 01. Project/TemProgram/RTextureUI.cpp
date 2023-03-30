#include "pch.h"
#include "RTextureUI.h"

#include <Engine/CTexture.h>

RTextureUI::RTextureUI()
	: ResUI("Texture##UI", RES_TYPE::TEXTURE)
{
}

RTextureUI::~RTextureUI()
{
}

void RTextureUI::update()
{
	ResUI::update();
}

void RTextureUI::render_update()
{
	ResUI::render_update();

	CTexture* pTargetTex = ((CTexture*)GetTarget().Get());

	ImGui::Image(pTargetTex->GetSRV().Get(), ImVec2(150.f, 150.f));

	string strKey = string(pTargetTex->GetKey().begin(), pTargetTex->GetKey().end());
	string strRelativePath = string(pTargetTex->GetRelativePath().begin(), pTargetTex->GetRelativePath().end());

	ImGui::Text("Key    "); ImGui::SameLine();
	ImGui::InputText("##TexKey", (char*)strKey.data(), strKey.length(), ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Path   "); ImGui::SameLine();
	ImGui::InputText("##TexPath", (char*)strRelativePath.data(), strRelativePath.length(), ImGuiInputTextFlags_ReadOnly);

	char szNum[50] = {};
	_itoa_s(pTargetTex->GetWidth(), szNum, 10);
	
	ImGui::Text("Width  "); ImGui::SameLine();
	ImGui::InputText("##TexWidth", szNum, 50, ImGuiInputTextFlags_ReadOnly);

	_itoa_s(pTargetTex->GetHeight(), szNum, 10);
	ImGui::Text("Height "); ImGui::SameLine();
	ImGui::InputText("##TexHight", szNum, 50, ImGuiInputTextFlags_ReadOnly);

	if (ImGui::Button("SaveTexture##texture", ImVec2(100.f, 24.f)))
	{
		pTargetTex->SaveTexture(pTargetTex->GetKey());
	}
}