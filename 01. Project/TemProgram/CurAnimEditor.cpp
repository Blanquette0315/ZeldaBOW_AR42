#include "pch.h"
#include "CurAnimEditor.h"

#include <Engine/CResMgr.h>
#include <Engine/CTimeMgr.h>

#include "UIdefine.h"

#include "Animator2DUI.h"
#include "CImGuiMgr.h"
#include "ListUI.h"
#include "TextureUI.h"

CurAnimEditor::CurAnimEditor()
	: UI("CurAnimEditor")
	, m_vSceneSize(100,100)
	, m_fGridPixelSize(10.f)
	, m_bPlayPreview(false)
	, m_bPlayOncePreview(false)
	, m_bGrid(false)
	, m_bGrid_Front(false)
	, m_bGrid_Center(false)
{
	Close();
}

CurAnimEditor::~CurAnimEditor()
{
}

void CurAnimEditor::update()
{


	UI::update();
}

void CurAnimEditor::render_update()
{
	// ���� �ִϸ��̼� �����Ӻ� ���� ���� �����̵�
	ImGui::Text("Animation Frm Composition");

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::Spacing();
	ImGui::PushID("##ANED_HorizonScroll");
	float child_height = m_vSceneSize.y + style.ScrollbarSize + style.WindowPadding.y * 2.0f; //ImGui::GetTextLineHeight()

	ImVec2 scrolling_child_size = ImVec2(-100, child_height);
	if (ImGui::BeginChild("##ANED_ScrollChild", scrolling_child_size, true, ImGuiWindowFlags_HorizontalScrollbar)) // Avoid calling SetScrollHereY when running with culled items
	{
		for (int i = 0; i < m_vecFrm.size(); i++)
		{
			// ���� ���� ����
			if (i > 0)
				ImGui::SameLine();


			ImVec2 CursorPos = ImGui::GetCursorPos();
			// ���� ������ ��ư ����
			// ���� ��ư id �����
			string SelectId = "";
			char id[50] = "";
			sprintf_s(id, 50, "[%d]", i);
			SelectId += id;
			SelectId += "##ANED_Frm";

			bool selected = (m_iSelectedIdx == i);
			if (ImGui::Selectable(SelectId.c_str(), selected, 0, m_vSceneSize))
			{
				// �ش� ��ư�� Ŭ���ϸ�, �ش� ������ ������ �Ʒ��� ���. (���⼭�� �ε��� ������)
				m_iSelectedIdx = i;
				m_AnimPreviewIdx = i;
			}

			if (selected)
				ImGui::SetItemDefaultFocus();

			// ���� Ŭ���� �߻��ϸ�, ��Ʋ�� �̹������� ���ϴ� �κи� ���ϴ� ������� �߶� �� �־�� �Ѵ�.
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				// �׷��� ���ؼ� TextureUI�� ����־�� �Ѵ�.
				m_pTextureUI = dynamic_cast<TextureUI*>(CImGuiMgr::GetInst()->FindUI("TextureUI"));
				assert(m_pTextureUI);

				m_pTextureUI->SetTexture(CResMgr::GetInst()->FindRes<CTexture>(wstring(m_AtlasTex->GetKey().begin(), m_AtlasTex->GetKey().end())));
				m_pTextureUI->SetTextureWH(m_vAtlasWH);
				m_pTextureUI->SetTexUIType(TEXUI_TYPE::ANIMATION);

				Vec2 FullSizePixel = m_vFullSize;
				FullSizePixel *= m_vAtlasWH;
				m_pTextureUI->SetFullSize(FullSizePixel);

				m_pTextureUI->AddDynamicOBClicked(this, (FUNC_2)&CurAnimEditor::SetSliceImage);

				m_pTextureUI->Open();
			}

			// Selected ��ġ�� �̹��� �׷��ֱ�
			ImGui::SetCursorPos(CursorPos);
			ImTextureID my_tex_id = m_AtlasTex->GetSRV().Get();
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
			ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white ImVec2(m_TexWH.x, m_TexWH.y)
			ImVec2 uv_min = ImVec2(m_vecFrm[i].vLeftTop);							// Top-left
			ImVec2 uv_max = ImVec2(m_vecFrm[i].vSlice + m_vecFrm[i].vLeftTop);		// Lower-right
			ImGui::Image(my_tex_id, m_vSceneSize, uv_min, uv_max, tint_col, border_col);


		}
	}
	float scroll_x = ImGui::GetScrollX();
	float scroll_max_x = ImGui::GetScrollMaxX();
	ImGui::EndChild();
	ImGui::SameLine();

	// ������ �߰� ��ư �����
	if (ImGui::Button("+##ANED_AddFrm", ImVec2(30.f, 30.f)))
	{
		// ���ο� �������� �߰��� �־��ش�.
		tAnim2DFrm NewFrm = {};
		m_vecFrm.push_back(NewFrm);
	}

	ImGui::Spacing();

	ImGui::PopID();


	// �� ���� �з��ϱ�
	ImGui::Text("\n");
	ImGui::BeginTabBar("##ANED_EditorTabBar");
	{

		// ========1��° ��=========
		// �ִϸ��̼� ���� ��
		// =========================
		if (ImGui::BeginTabItem("Animation Info"))
		{

			ImGui::Text("Cur Animation Info");
			ImGui::Separator();

			// ���� �ִϸ��̼� �̸� ���
			static char AnimName[128] = {};
			strcpy_s(AnimName, string(m_CurAnimName.begin(), m_CurAnimName.end()).c_str());
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);
			ImGui::Text("AnimationName :"); ImVec2 ANCursor = ImGui::GetCursorPos(); ImGui::SameLine();
			
			ImGui::InputText("##ANED_AnimationName", AnimName, sizeof(AnimName), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
			m_CurAnimName = string(AnimName);
			ImGui::PopItemWidth();

			ImGui::SameLine();
			if (ImGui::Button("##ANED_ChangeAnimName", ImVec2(18.f, 18.f)))
			{
				// ���� �̸��� �ٲ���� ���� ���� �ϵ��� ����ó���� ���ش�.
				if (m_pCurAnim->GetName() != wstring(m_CurAnimName.begin(), m_CurAnimName.end()))
				{
					m_pTarget->Animator2D()->ChangeAnimName(m_pCurAnim->GetName(), wstring(m_CurAnimName.begin(), m_CurAnimName.end()));
					m_pOwnerUI->SetCurAnim(m_pTarget->Animator2D()->FindAnimation(wstring(m_CurAnimName.begin(), m_CurAnimName.end())));
					m_pOwnerUI->ChangeInfo(true);
				}
			}

			// ���� ��ư�����
			ImGui::SameLine();  ImGui::Text("Animation Save:"); ImGui::SameLine();
			if (ImGui::Button("SAVE##ANED_AnimationSave", ImVec2(80.f, 30.f)))
			{
				// �����Ϸ��� �ִϸ��̼� �̸����� �̹� Ű���� �����Ѵٸ�, ��ϵ� �ִϸ��̼��� �����, �ٽ� �������ش�.
				CAnimation2D* pFindAnim = m_pTarget->Animator2D()->FindAnimation(wstring(m_CurAnimName.begin(), m_CurAnimName.end()));
				if (pFindAnim)
				{
					// �ٽ� �ִϸ������� ���� ���ϸ��̼��� ���� ���ŵ� �ִϸ��̼����� �������־�� �Ѵ�.
					m_pTarget->Animator2D()->OverWriteAnimation(wstring(m_CurAnimName.begin(), m_CurAnimName.end())
						, CResMgr::GetInst()->FindRes<CTexture>(m_AtlasTex.Get()->GetKey())
						, m_vecFrm);

					m_pOwnerUI->SetCurAnim(m_pTarget->Animator2D()->FindAnimation(wstring(m_CurAnimName.begin(), m_CurAnimName.end())));
				}

				// �ش� �ִϸ��̼� �̸��� Ű ������ ���� ����� �ش�.
				else
				{
					m_pTarget->Animator2D()->CreateAnimation(wstring(m_CurAnimName.begin(), m_CurAnimName.end())
						, CResMgr::GetInst()->FindRes<CTexture>(m_AtlasTex.Get()->GetKey())
						, m_vecFrm);
				}

				m_pOwnerUI->ChangeInfo(true);
			}

			// ���� �ִϸ��̼ǿ��� ������� ��Ʋ�� ��� & �ٲٱ�
			ANCursor.y += 5.f;
			ImGui::SetCursorPos(ANCursor);
			ImGui::Text("Atlas Texture :"); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), string(m_AtlasTex->GetKey().begin(), m_AtlasTex->GetKey().end()).c_str());
			
			// ���� ������ ����
			ImGui::Text("\n");
			ImGui::Text("Cur Frm Info");
			ImGui::Separator();

			// �»�� ���� UV ���� Pixel�� �Է¹ޱ�
			ImGui::Text("FrmLeftTop UV    :"); ImGui::SameLine(); ImGui::Text(" %.3f , %.3f", m_vecFrm[m_iSelectedIdx].vLeftTop.x, m_vecFrm[m_iSelectedIdx].vLeftTop.y);
			ImGui::Text("FrmLeftTopPixel  :"); ImGui::SameLine();
			Vec2 LeftTopPixel = m_vecFrm[m_iSelectedIdx].vLeftTop;
			LeftTopPixel *= m_vAtlasWH;
			ImGui::InputFloat2("##ANED_FrmLeftTop", LeftTopPixel, "%.f");
			m_vecFrm[m_iSelectedIdx].vLeftTop = LeftTopPixel / m_vAtlasWH;

			// �����̽� ���� UV ���� Pixel�� �Է¹ޱ�
			ImGui::Text("FrmSlice UV      :"); ImGui::SameLine(); ImGui::Text(" %.3f , %.3f", m_vecFrm[m_iSelectedIdx].vSlice.x, m_vecFrm[m_iSelectedIdx].vSlice.y);
			ImGui::Text("FrmSlicePixel    :"); ImGui::SameLine();
			Vec2 SlicePixel = m_vecFrm[m_iSelectedIdx].vSlice;
			SlicePixel *= m_vAtlasWH;
			ImGui::InputFloat2("##ANED_FrmSlice", SlicePixel, "%.f");
			m_vecFrm[m_iSelectedIdx].vSlice = SlicePixel / m_vAtlasWH;

			// FullSize ���� ���� �Է¹ޱ�
			ImGui::Text("FrmFullSize UV   :"); ImGui::SameLine(); ImGui::Text(" %.3f , %.3f", m_vecFrm[m_iSelectedIdx].vFullsize.x, m_vecFrm[m_iSelectedIdx].vFullsize.y, ImGuiInputTextFlags_ReadOnly);
			ImGui::Text("FrmFullSize Pixel:"); ImGui::SameLine();
			Vec2 FullSizePixel = m_vecFrm[m_iSelectedIdx].vFullsize;
			FullSizePixel *= m_vAtlasWH;
			if(ImGui::InputFloat2("##ANED_FullSize", FullSizePixel, "%.f", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				m_vecFrm[m_iSelectedIdx].vFullsize = FullSizePixel / m_vAtlasWH;
			}

			// Duration ���� ���� �Է¹ޱ�
			ImGui::Text("Frm FPS          :"); ImGui::SameLine(); ImGui::InputFloat("##ANED_FrmFPS", &m_vecFrm[m_iSelectedIdx].fDuration,0.01f,0.1f);
			ImGui::SameLine();
			if (ImGui::Button("##ANED_BTN_FPSALL", ImVec2(18.f, 18.f)))
			{
				for (size_t i = 0; i < m_vecFrm.size(); ++i)
				{
					m_vecFrm[i].fDuration = m_vecFrm[m_iSelectedIdx].fDuration;
				}
			}

			// OffSet ���� ���� �Է¹ޱ�
			// OffSet�� UV��ǥ�� ������ �־�� �Ѵ�.
			ImGui::Text("FrmOffSet UV     :"); ImGui::SameLine(); ImGui::InputFloat2("##ANED_OffSet", m_vecFrm[m_iSelectedIdx].vOffset, "%.3f", ImGuiInputTextFlags_ReadOnly);
			Vec2 OffsetPixel = m_vecFrm[m_iSelectedIdx].vOffset;
			OffsetPixel *= m_vAtlasWH;
			ImGui::Text("FrmOffSet Pixel  :"); ImGui::SameLine();
			if (ImGui::InputFloat2("##ANED_OffSetPixel", OffsetPixel, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				m_vecFrm[m_iSelectedIdx].vOffset = OffsetPixel / m_vAtlasWH;
			}
			

			// ���ݱ��� ���� �ִϸ��̼��� ����� �� �� �ִ� ������
			ImGui::Text("Animation Preview");

			// ���ĥ�ϱ� 
			static ImVec2 scrolling(0.0f, 0.0f);
			ImVec2 LTCursorPos = ImGui::GetCursorScreenPos();
			ImVec2 RBCursorPos = ImVec2(LTCursorPos.x + 302.f, LTCursorPos.y + 302.f);
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRectFilled(LTCursorPos, RBCursorPos, IM_COL32(50, 50, 50, 255));
			
			// Grid �߰��ϱ�
			draw_list->PushClipRect(LTCursorPos, RBCursorPos, true);
			if (m_bGrid)
			{
				float GRID_STEPX = m_fGridPixelSize;
				float GRID_STEPY = m_fGridPixelSize;
				Vec2 PixelRatio = Vec2(300.f, 300.f) / (m_vecFrm[m_AnimPreviewIdx].vSlice * m_vAtlasWH);
				GRID_STEPX *= PixelRatio.x;
				GRID_STEPY *= PixelRatio.y;

				for (float x = fmodf(scrolling.x, GRID_STEPX); x < 302.f; x += GRID_STEPX)
					draw_list->AddLine(ImVec2(LTCursorPos.x + x, LTCursorPos.y), ImVec2(LTCursorPos.x + x, RBCursorPos.y), IM_COL32(200, 200, 200, 40));
				for (float y = fmodf(scrolling.y, GRID_STEPY); y < 302.f; y += GRID_STEPY)
					draw_list->AddLine(ImVec2(LTCursorPos.x, LTCursorPos.y + y), ImVec2(RBCursorPos.x, LTCursorPos.y + y), IM_COL32(200, 200, 200, 40));

			}
			draw_list->PopClipRect();

			// ���� ���� ��ư�� �����ٸ�, �ִϸ��̼��� �����Ѵ�.
			if (m_bPlayPreview)
			{
				PlayWorkingAnim();
			}

			// ������ �ִϸ��̼� �׸���
			ImTextureID my_tex_id = m_AtlasTex->GetSRV().Get();
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
			ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white ImVec2(m_TexWH.x, m_TexWH.y)

			// FullSize ��ŭ �߶����
			Vec2 vDiff = (m_vecFrm[m_AnimPreviewIdx].vFullsize - m_vecFrm[m_AnimPreviewIdx].vSlice) / 2.f;
			Vec2 FullLiftTop = m_vecFrm[m_AnimPreviewIdx].vLeftTop - vDiff;
			Vec2 FullRightBot = m_vecFrm[m_AnimPreviewIdx].vSlice + m_vecFrm[m_AnimPreviewIdx].vLeftTop + vDiff;

			ImVec2 uv_min = ImVec2(FullLiftTop);                                    // Top-left
			ImVec2 uv_max = ImVec2(FullRightBot);        // Lower-right
			ImGui::Image(my_tex_id, ImVec2(300, 300), uv_min, uv_max, tint_col, border_col);

			// �ٸ� ��Ʋ�� �߶󳻱�
			

			// Front Grid �߰��ϱ�
			draw_list->PushClipRect(LTCursorPos, RBCursorPos, true);
			if (m_bGrid_Front)
			{
				float GRID_STEPX = m_fGridPixelSize;
				float GRID_STEPY = m_fGridPixelSize;
				Vec2 PixelRatio = Vec2(300.f, 300.f) / (m_vecFrm[m_AnimPreviewIdx].vSlice * m_vAtlasWH);
				GRID_STEPX *= PixelRatio.x;
				GRID_STEPY *= PixelRatio.y;

				for (float x = fmodf(scrolling.x, GRID_STEPX); x < 302.f; x += GRID_STEPX)
					draw_list->AddLine(ImVec2(LTCursorPos.x + x, LTCursorPos.y), ImVec2(LTCursorPos.x + x, RBCursorPos.y), IM_COL32(200, 200, 200, 40));
				for (float y = fmodf(scrolling.y, GRID_STEPY); y < 302.f; y += GRID_STEPY)
					draw_list->AddLine(ImVec2(LTCursorPos.x, LTCursorPos.y + y), ImVec2(RBCursorPos.x, LTCursorPos.y + y), IM_COL32(200, 200, 200, 40));

			}
			draw_list->PopClipRect();

			draw_list->PushClipRect(LTCursorPos, RBCursorPos, true);
			// Center Grid �߰��ϱ�
			if (m_bGrid_Center)
			{
				draw_list->AddLine(ImVec2(LTCursorPos.x + 150.f, LTCursorPos.y), ImVec2(LTCursorPos.x + 150.f, RBCursorPos.y + 300.f), IM_COL32(255, 0, 0, 40));
				draw_list->AddLine(ImVec2(LTCursorPos.x, LTCursorPos.y + 150.f), ImVec2(RBCursorPos.x + 300.f, LTCursorPos.y + 150.f), IM_COL32(255, 0, 0, 40));
			}
			draw_list->PopClipRect();

			// �ִϸ��̼� ������ ���� ��ư ����
			ImGui::SameLine();
			ANCursor = ImGui::GetCursorPos();
			if (ImGui::Button("Play##ANED_SE_PreviewPlay", ImVec2(80.f, 30.f)))
			{
				m_bPlayPreview = true;
			}
			

			ImGui::SameLine();
			if (ImGui::Button("Stop##ANED_SE_PreviewStop", ImVec2(80.f, 30.f)))
			{
				m_bPlayPreview = false;
			}

			// �ִϸ��̼� �ѹ��� ���� ��ư ����
			ImGui::SameLine();
			if (ImGui::Button("Play_Once##ANED_SE_PreviewStop", ImVec2(80.f, 30.f)))
			{
				m_bPlayPreview = false;
				m_bPlayOncePreview = true;
			}

			// �׸��� Enable ��ư ����
			ANCursor.y += 38.f;
			ImGui::SetCursorPos(ANCursor);
			ANCursor = ImGui::GetCursorPos();
			if (ImGui::Button("Grid##ANED_SE_Grid", ImVec2(80.f, 30.f)))
			{
				m_bGrid = !m_bGrid;
				m_bGrid_Front = false;
			}

			ImGui::SameLine();
			if (ImGui::Button("Grid_Front##ANED_SE_Grid", ImVec2(80.f, 30.f)))
			{
				m_bGrid_Front = !m_bGrid_Front;
				m_bGrid = false;
			}

			// ������ ���� �߾� ǥ��
			ImGui::SameLine();
			if (ImGui::Button("GridCenter##ANED_SE_Grid", ImVec2(80.f, 30.f)))
			{
				m_bGrid_Center = !m_bGrid_Center;
			}

			// �׸��� �ȼ� ������ �Է¹ް� �����ϱ�
			ANCursor.y += 38.f;
			ImGui::SetCursorPos(ANCursor);
			ANCursor = ImGui::GetCursorPos();
			ImGui::Text("GridSize of Pixel");

			ANCursor.y += 16.f;
			ImGui::SetCursorPos(ANCursor);
			ImGui::PushItemWidth(168.f);
			ImGui::InputFloat("##ANED_SE_GridSize", &m_fGridPixelSize, 1.f, 10.f, "%.f");
			ImGui::PopItemWidth();

			ImGui::EndTabItem();
		}


		// ========2��° ��=========
		// ���ο� ���ϸ��̼� �����
		// =========================
		if (ImGui::BeginTabItem("Create Animation"))
		{

			ImGui::Text("Same Slice Edit");
			ImGui::Separator();
			
			// ���� �ִϸ��̼ǿ��� ������� ��Ʋ�� ��� & �ٲٱ�
			ImGui::Text("Atlas Texture :"); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), string(m_AtlasTex->GetKey().begin(), m_AtlasTex->GetKey().end()).c_str());
			ImGui::SameLine();
			if (ImGui::Button("##TIM_SetTexBtn", Vec2(18.f, 18.f)))
			{
				ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
				assert(pListUI);

				const map<wstring, Ptr<CRes>> MapRes = CResMgr::GetInst()->GetResource(RES_TYPE::TEXTURE);
				static vector<wstring> vecRes;
				vecRes.clear();

				map<wstring, Ptr<CRes>>::const_iterator iter = MapRes.begin();
				for (; iter != MapRes.end(); ++iter)
				{
					vecRes.push_back(iter->first);
				}
				pListUI->SetItemList(vecRes);
				pListUI->AddDynamicDBClicked(this, (FUNC_1)&CurAnimEditor::SetAtlasTex);

				pListUI->Open();
			}

			// ���� ���� �ִ� ������ �� �Է�
			// -�� �������� ��Ȳ�� �߻��� �� ���� ����ó�� ���־�� ��.
			static int SEFrmCount = 0;
			ImGui::Text("FrmCount         :"); ImGui::SameLine();
			if (ImGui::InputInt("##ANED_SE_FrmCount", &SEFrmCount, 1, 10, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				ResetWorkingAnim();
				m_vecFrm.clear();
				m_vecFrm.resize(SEFrmCount);
				m_iSelectedIdx = 0;
			}

			// ��Ʋ�� �̹����� ��� ���� �Է�
			static int SEIdxCount[2] = {};
			ImGui::Text("Atlas Index Count:"); ImGui::SameLine();
			ImGui::InputInt2("##ANED_SE_IndexCount", SEIdxCount, ImGuiInputTextFlags_EnterReturnsTrue);

			// �����̽� ����
			static int NewSESlice[2] = {};
			ImGui::Text("Slice at Pixel   :"); ImGui::SameLine();
			ImGui::InputInt2("##ANED_SE_Slice", NewSESlice);

			// �»�� ��ġ
			static int NewSELeftTop[2] = {};
			ImGui::Text("LeftTop at Pixel :"); ImGui::SameLine();
			ImGui::InputInt2("##ANED_SE_LeftTop", NewSELeftTop);
			
			// FullSize ����

			// �ִϸ��̼� ��� �ð� ���� �Է�
			// �ش� ������ ������ �ٷ�vector�� �־��ֵ��� �����ϴ°� ���� �� ����.
			// �������� ����� ��� ������ �ʱ�ȭ�Ǵ� ������ �ִ�.
			static float NewSEFPS = 0.f;
			ImGui::Text("Animation FPS    :"); ImGui::SameLine();
			ImGui::InputFloat("##ANED_SE_FPS", &NewSEFPS);
			
			// ���� �Ϸ� ��ư
			ImGui::Text("Aplly Animation  :"); ImGui::SameLine();
			if(ImGui::Button("##ANED_SE_Btn", ImVec2(18.f,18.f)))
			{
				if (SEFrmCount == SEIdxCount[0] * SEIdxCount[1])
				{
					// ���⼭ ��� ���ϴ°� �ƴ϶� �̹����� ������ �� �ѹ��� �ϵ��� �����ؾ��Ѵ�.
					m_vAtlasWH = Vec2(m_AtlasTex->GetWidth(), m_AtlasTex->GetHeight());

					// �����̽� ����
					Vec2 UVSlice = Vec2(NewSESlice[0] / m_vAtlasWH.x, NewSESlice[1] / m_vAtlasWH.y);

					for (int i = 0; i < SEIdxCount[0]; i++)
					{
						for (int j = 0; j < SEIdxCount[1]; ++j)
						{
							Vec2 UVLeftTop = Vec2(NewSELeftTop[0], NewSELeftTop[1]);
							UVLeftTop.x += j * NewSESlice[0];
							UVLeftTop.y += i * NewSESlice[1];
							UVLeftTop /= m_vAtlasWH;

							m_vecFrm[i * SEIdxCount[1] + j].vSlice = UVSlice;
							m_vecFrm[i * SEIdxCount[1] + j].vLeftTop = UVLeftTop;
							m_vecFrm[i * SEIdxCount[1] + j].fDuration = 1.f / NewSEFPS;
						}
					}
				}

				else
				{
					
				}
			}

			ImGui::EndTabItem();
		}
	}
	ImGui::EndTabBar();

}






void CurAnimEditor::SetAnimation(CAnimation2D* _Anim)
{
	m_pCurAnim = _Anim;
	m_CurAnimName = string(m_pCurAnim->GetName().begin(), m_pCurAnim->GetName().end());
	m_vecFrm = m_pCurAnim->GetvecFrm();
	m_AtlasTex = m_pCurAnim->GetAtlas();

	m_vAtlasWH.x = m_AtlasTex->GetWidth();
	m_vAtlasWH.y = m_AtlasTex->GetHeight();

	// �ش� Ǯ����� 0�� �ε������� �������°� ������ �� ���� ����.(��� �������� Ǯ����� ������ ���� ���ػ���)
	//m_vFullSize = m_vecFrm[0].vFullsize;
}

void CurAnimEditor::PlayWorkingAnim()
{
	// �ð� ����
	m_fAcctime += DT;
	// ���� �ð��� �ش� ������ �����ð��� �Ѿ�� ���� ���������� �Ѿ��.
	if (m_fAcctime > m_vecFrm[m_AnimPreviewIdx].fDuration)
	{
		m_fAcctime = 0.f;
		++m_AnimPreviewIdx;

		// �ִ� �����ӿ� �����ϸ� Finish ���·� ��ȯ�Ѵ�.
		if (m_vecFrm.size() <= m_AnimPreviewIdx)
		{
			//m_iCurIdx = (int)m_vecFrm.size() - 1;
			//m_bFinish = true;

			m_AnimPreviewIdx = 0;
		}
	}

}

void CurAnimEditor::ResetWorkingAnim()
{
	m_AnimPreviewIdx = 0;
}


void CurAnimEditor::SetAtlasTex(DWORD_PTR _AtlasKey)
{
	string strKey = (char*)_AtlasKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(wstrKey);
	assert(nullptr != pTexture);

	m_AtlasTex = pTexture;

	m_vAtlasWH.x = m_AtlasTex->GetWidth();
	m_vAtlasWH.y = m_AtlasTex->GetHeight();
}

void CurAnimEditor::SetSliceImage(DWORD_PTR _LeftTop, DWORD_PTR _Slice)
{
	Vec2* LeftTopUV = (Vec2*)_LeftTop;
	Vec2* SliceUV = (Vec2*)_Slice;

	m_vecFrm[m_iSelectedIdx].vLeftTop = Vec2(LeftTopUV->x, LeftTopUV->y);
	m_vecFrm[m_iSelectedIdx].vSlice = Vec2(SliceUV->x, SliceUV->y);

	// �̹��� �߰��� �����̽��� Ǯ������� �� ũ�ٸ� �ش� ����� Ǯ������� ����
	static bool Change = false;
	if (m_vFullSize.x < m_vecFrm[m_iSelectedIdx].vSlice.x)
	{
		m_vFullSize.x = m_vecFrm[m_iSelectedIdx].vSlice.x;
		Change = true;
	}
	if (m_vFullSize.y < m_vecFrm[m_iSelectedIdx].vSlice.y)
	{
		m_vFullSize.y = m_vecFrm[m_iSelectedIdx].vSlice.y;
		Change = true;
	}
	if (Change)
	{

		for (size_t i = 0; i < m_vecFrm.size(); ++i)
		{
			m_vecFrm[i].vFullsize = m_vFullSize;
		}
		Change = false;
	}

	m_vecFrm[m_iSelectedIdx].vFullsize = m_vFullSize;
}