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

	// Preview 애니메이션 0번째 index로 변경해주지 않으면 터진다.
	m_iCurIdx = 0;
}

void Animator2DUI::update()
{
	if (GetTarget() != nullptr && m_bChanged == true)
	{
		// 만약 해당 에니메이터에 애니메이션이 하나도 없다면, 빈 애니메이션을 만들어 달라 요청해 그걸 현재 애니메이션으로 지정한다.
		if (0 == GetTarget()->Animator2D()->GetAllAnimation().size())
		{
			GetTarget()->Animator2D()->CreatebeginAnimation();
		}
		
		m_mapAnim = GetTarget()->Animator2D()->GetAllAnimation();
		m_pCurAnim = GetTarget()->Animator2D()->GetCurAnimation();

		// 현재 에니메이션이 있을 때만 접근해야함. (CAnimator2D의 begine호출로 null이 들어갈 수 있음)
		if(m_pCurAnim)
			m_vecFrm = m_pCurAnim->GetvecFrm();
		m_bChanged = false;
	}
	
	ComponentUI::update();
}

void Animator2DUI::render_update()
{
	ComponentUI::render_update();

	// CAnimator2D의 begine으로 인한 예외처리
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

	// Load를 하면서 CAnimator2D의 begine이 호출되어 현재 에니메이션에 null값이 들어가게된다. 그때는 접근하면 안되기 때문에 예외처리를 한다.
	// 변경 : Animation이 없더라도 해당 버튼이 있어야 Animation을 만들 수 있다.
	//        null에 접근하면 안되기 때문에 AnimEditor를 열기 전에 현재 Animation이 없다면 빈 Animtion을 하나 만들어서 넣어준 다음 열 것이다.
	ImGui::Text("Edit CurAnimation"); ImGui::SameLine();
	if (ImGui::Button("##ANIM2D_EditCurAnim", ImVec2(18.f, 18.f)))
	{
		// CurAnimEditor 열기
		CurAnimEditor* pEditor = dynamic_cast<CurAnimEditor*>(CImGuiMgr::GetInst()->FindUI("CurAnimEditor"));
		pEditor->clear();
		pEditor->SetAnimation(m_pCurAnim);
		pEditor->SetTarget(GetTarget());
		pEditor->SetOwnerUI(this);

		pEditor->Open();
	}

	ImGui::Text("Create Animation "); ImGui::SameLine();
	ImGui::Button("##ANIM2D_CreateAnim", ImVec2(18.f, 18.f));

	// 애니메이션 프리뷰 재생
	if (nullptr != m_pCurAnim)
	{
		ImGui::Text("Animation Preview");
		ImGui::GetCursorScreenPos();
		PlayPreview();
	}
}



void Animator2DUI::PlayPreview()
{
	// 시간 누적
	m_fAcctime += DT;
	// 누적 시간이 해당 프레임 유지시간을 넘어서면 다음 프레임으로 넘어간다.
	if (m_fAcctime > m_vecFrm[m_iCurIdx].fDuration)
	{
		m_fAcctime = 0.f;
		++m_iCurIdx;

		// 최대 프레임에 도달하면 Finish 상태로 전환한다.
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