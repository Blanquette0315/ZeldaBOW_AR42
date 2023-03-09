#include "pch.h"
#include "SoundUI.h"

#include <Engine/CSound.h>

SoundUI::SoundUI()
	: ResUI("Sound##UI", RES_TYPE::SOUND)
	, m_pTargetSound(nullptr)
	, m_strName{}
	, m_strPath{}
	, m_iChenal(-1)
	, m_fVolume(1.f)
	, m_bPlaySound(false)
	, m_bUpdateOnce(false)
{
}

SoundUI::~SoundUI()
{
}

void SoundUI::update()
{
	ResUI::update();

	if (GetTarget().Get() != nullptr)
	{
		if ((CSound*)GetTarget().Get() != m_pTargetSound)
		{
			// PrevSound Stop
			PrevSoundStop();
			
			m_pTargetSound = (CSound*)GetTarget().Get();
			m_bUpdateOnce = false;
		}

		if (!m_bUpdateOnce && m_pTargetSound != nullptr)
		{
			wstring strPath = m_pTargetSound->GetRelativePath();

			// Sound Name
			m_strName = string(strPath.begin(), strPath.end());
			size_t Idx = m_strName.rfind("\\");
			m_strName.erase(0, Idx + 1);

			// Sound Path
			m_strPath = string(strPath.begin(), strPath.end());

			m_bUpdateOnce = true;
		}
	}
}

void SoundUI::render_update()
{
	ResUI::render_update();

	// Sound Name Print
	ImGui::Text("Sound Name  "); ImGui::SameLine(); ImGui::InputText("##TargetSoundName", (char*)m_strName.data(), m_strName.size(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	// Sound Path Print
	ImGui::Text("Sound Path  "); ImGui::SameLine(); ImGui::InputText("##TargetSoundPath", (char*)m_strPath.data(), m_strPath.size(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

	// Sound Play Btn
	if (ImGui::Button("Play & Stop##SoundUI", ImVec2(90.f, 18.f)))
	{
		m_bPlaySound = !m_bPlaySound;

		if (m_bPlaySound)
		{
			m_iChenal = m_pTargetSound->Play(1, m_fVolume);
		}
		else
		{
			m_pTargetSound->Stop();
		}
	}

	// Sound Volume Slide
	ImGui::Text("Sound Volume"); ImGui::SameLine(); ImGui::SliderFloat("##SoundVolumeFloat", &m_fVolume, 0.f, 1.f);
	
	if (m_bPlaySound)
	{
		m_pTargetSound->SetVolume(m_fVolume, m_iChenal);
	}
	
}

void SoundUI::Close()
{
	ResUI::Close();

	// Stop Sound
	PrevSoundStop();
}

void SoundUI::PrevSoundStop()
{
	if (m_pTargetSound)
	{
		// PrevSound Stop
		m_pTargetSound->Stop();
		m_bPlaySound = false;
	}
}