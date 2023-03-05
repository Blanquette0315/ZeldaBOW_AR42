#include "pch.h"
#include "Animator2DUI.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CResMgr.h>
#include <Engine/CTimeMgr.h>

#include "CurAnimEditor.h"
#include "CImGuiMgr.h"

Animator2DUI::Animator2DUI()
	: ComponentUI("Animator2DUI", COMPONENT_TYPE::ANIMATOR2D)
	, m_pCurAnim(nullptr)
	, m_bChanged(true)
	, m_fAcctime(0)
	, m_iCurComboIdx(0)
	, m_iCurIdx(0)
{
}

Animator2DUI::~Animator2DUI()
{
}

void Animator2DUI::Open()
{
	UI::Open();
	m_bChanged = true;

	// Preview �ִϸ��̼� 0��° index�� ���������� ������ ������.
	m_iCurIdx = 0;
}

void Animator2DUI::update()
{
	if (GetTarget() != nullptr && m_bChanged == true)
	{
		// ���� �ش� ���ϸ����Ϳ� �ִϸ��̼��� �ϳ��� ���ٸ�, �� �ִϸ��̼��� ����� �޶� ��û�� �װ� ���� �ִϸ��̼����� �����Ѵ�.
		if (0 == GetTarget()->Animator2D()->GetAllAnimation().size())
		{
			GetTarget()->Animator2D()->CreatebeginAnimation();
		}
		
		m_mapAnim = GetTarget()->Animator2D()->GetAllAnimation();
		m_pCurAnim = GetTarget()->Animator2D()->GetCurAnimation();

		// ���� ���ϸ��̼��� ���� ���� �����ؾ���. (CAnimator2D�� begineȣ��� null�� �� �� ����)
		if(m_pCurAnim)
			m_vecFrm = m_pCurAnim->GetvecFrm();
		m_bChanged = false;
	}
	
	ComponentUI::update();
}

void Animator2DUI::render_update()
{
	ComponentUI::render_update();

	// CAnimator2D�� begine���� ���� ����ó��
	static string Name;
	if (nullptr == m_pCurAnim)
		Name = "";
	else
		Name = string(m_pCurAnim->GetName().begin(), m_pCurAnim->GetName().end());

	ImGui::Text("Animation"); ImGui::SameLine();
	if (ImGui::BeginCombo("##ANIM2D_AnimCombo", Name.c_str(), 0))
	{
		int Idxnum = 0;
		map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.begin();
		for (; iter != m_mapAnim.end(); ++iter)
		{
			Idxnum++;
			string strName = string(iter->first.begin(), iter->first.end());
			char szTag[50] = "";
			sprintf_s(szTag, 50, "##%d", Idxnum);
			strName += szTag;

			bool Selectable = (m_iCurComboIdx == Idxnum);
			if (ImGui::Selectable(strName.c_str(), Selectable))
			{
				m_iCurComboIdx = Idxnum;
				m_pCurAnim = iter->second;
				m_vecFrm = m_pCurAnim->GetvecFrm();
				m_iCurIdx = 0;
				GetTarget()->Animator2D()->Play(m_pCurAnim->GetName(), true);
			}

			if (Selectable)
				ImGui::SetItemDefaultFocus();	
		}
		ImGui::EndCombo();
	}

	// Load�� �ϸ鼭 CAnimator2D�� begine�� ȣ��Ǿ� ���� ���ϸ��̼ǿ� null���� ���Եȴ�. �׶��� �����ϸ� �ȵǱ� ������ ����ó���� �Ѵ�.
	// ���� : Animation�� ������ �ش� ��ư�� �־�� Animation�� ���� �� �ִ�.
	//        null�� �����ϸ� �ȵǱ� ������ AnimEditor�� ���� ���� ���� Animation�� ���ٸ� �� Animtion�� �ϳ� ���� �־��� ���� �� ���̴�.
	ImGui::Text("Edit CurAnimation"); ImGui::SameLine();
	if (ImGui::Button("##ANIM2D_EditCurAnim", ImVec2(18.f, 18.f)))
	{
		// CurAnimEditor ����
		CurAnimEditor* pEditor = dynamic_cast<CurAnimEditor*>(CImGuiMgr::GetInst()->FindUI("CurAnimEditor"));
		pEditor->clear();
		pEditor->SetAnimation(m_pCurAnim);
		pEditor->SetTarget(GetTarget());
		pEditor->SetOwnerUI(this);

		pEditor->Open();
	}

	ImGui::Text("Create Animation "); ImGui::SameLine();
	ImGui::Button("##ANIM2D_CreateAnim", ImVec2(18.f, 18.f));

	// �ִϸ��̼� ������ ���
	if (nullptr != m_pCurAnim)
	{
		ImGui::Text("Animation Preview");
		ImGui::GetCursorScreenPos();
		PlayPreview();
	}
}



void Animator2DUI::PlayPreview()
{
	// �ð� ����
	m_fAcctime += DT;
	// ���� �ð��� �ش� ������ �����ð��� �Ѿ�� ���� ���������� �Ѿ��.
	if (m_fAcctime > m_vecFrm[m_iCurIdx].fDuration)
	{
		m_fAcctime = 0.f;
		++m_iCurIdx;

		// �ִ� �����ӿ� �����ϸ� Finish ���·� ��ȯ�Ѵ�.
		if (m_vecFrm.size() <= m_iCurIdx)
		{
			//m_iCurIdx = (int)m_vecFrm.size() - 1;
			//m_bFinish = true;

			m_iCurIdx = 0;
		}
	}
	ImTextureID my_tex_id = m_pCurAnim->GetAtlas()->GetSRV().Get();
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white ImVec2(m_TexWH.x, m_TexWH.y)
	ImVec2 uv_min = ImVec2(m_vecFrm[m_iCurIdx].vLeftTop);	// Top-left
	ImVec2 uv_max = ImVec2(m_vecFrm[m_iCurIdx].vSlice + m_vecFrm[m_iCurIdx].vLeftTop);		// Lower-right
	ImGui::Image(my_tex_id, ImVec2(300, 300), uv_min, uv_max, tint_col, border_col);
}