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
	// 지금 애니메이션 프레임별 정보 수평 슬라이드
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
			// 동일 선상에 생성
			if (i > 0)
				ImGui::SameLine();


			ImVec2 CursorPos = ImGui::GetCursorPos();
			// 선택 가능한 버튼 생성
			// 선택 버튼 id 만들기
			string SelectId = "";
			char id[50] = "";
			sprintf_s(id, 50, "[%d]", i);
			SelectId += id;
			SelectId += "##ANED_Frm";

			bool selected = (m_iSelectedIdx == i);
			if (ImGui::Selectable(SelectId.c_str(), selected, 0, m_vSceneSize))
			{
				// 해당 버튼을 클릭하면, 해당 프레임 정보가 아래에 뜬다. (여기서는 인덱스 설정만)
				m_iSelectedIdx = i;
				m_AnimPreviewIdx = i;
			}

			if (selected)
				ImGui::SetItemDefaultFocus();

			// 더블 클릭이 발생하면, 아틀라스 이미지에서 원하는 부분만 원하는 사이즈로 잘라낼 수 있어야 한다.
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				// 그러기 위해서 TextureUI를 띄워주어야 한다.
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

			// Selected 위치에 이미지 그려주기
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

	// 프레임 추가 버튼 만들기
	if (ImGui::Button("+##ANED_AddFrm", ImVec2(30.f, 30.f)))
	{
		// 새로운 프레임을 추가해 넣어준다.
		tAnim2DFrm NewFrm = {};
		m_vecFrm.push_back(NewFrm);
	}

	ImGui::Spacing();

	ImGui::PopID();


	// 탭 만들어서 분류하기
	ImGui::Text("\n");
	ImGui::BeginTabBar("##ANED_EditorTabBar");
	{

		// ========1번째 탭=========
		// 애니메이션 정보 탭
		// =========================
		if (ImGui::BeginTabItem("Animation Info"))
		{

			ImGui::Text("Cur Animation Info");
			ImGui::Separator();

			// 지금 애니메이션 이름 출력
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
				// 만약 이름이 바뀌었을 때만 동작 하도록 예외처리를 해준다.
				if (m_pCurAnim->GetName() != wstring(m_CurAnimName.begin(), m_CurAnimName.end()))
				{
					m_pTarget->Animator2D()->ChangeAnimName(m_pCurAnim->GetName(), wstring(m_CurAnimName.begin(), m_CurAnimName.end()));
					m_pOwnerUI->SetCurAnim(m_pTarget->Animator2D()->FindAnimation(wstring(m_CurAnimName.begin(), m_CurAnimName.end())));
					m_pOwnerUI->ChangeInfo(true);
				}
			}

			// 저장 버튼만들기
			ImGui::SameLine();  ImGui::Text("Animation Save:"); ImGui::SameLine();
			if (ImGui::Button("SAVE##ANED_AnimationSave", ImVec2(80.f, 30.f)))
			{
				// 저장하려는 애니메이션 이름으로 이미 키값이 존재한다면, 등록된 애니메이션을 지우고, 다시 생성해준다.
				CAnimation2D* pFindAnim = m_pTarget->Animator2D()->FindAnimation(wstring(m_CurAnimName.begin(), m_CurAnimName.end()));
				if (pFindAnim)
				{
					// 다시 애니메이터의 현재 에니메이션을 새로 갱신된 애니메이션으로 변경해주어야 한다.
					m_pTarget->Animator2D()->OverWriteAnimation(wstring(m_CurAnimName.begin(), m_CurAnimName.end())
						, CResMgr::GetInst()->FindRes<CTexture>(m_AtlasTex.Get()->GetKey())
						, m_vecFrm);

					m_pOwnerUI->SetCurAnim(m_pTarget->Animator2D()->FindAnimation(wstring(m_CurAnimName.begin(), m_CurAnimName.end())));
				}

				// 해당 애니메이션 이름의 키 값으로 새로 만들어 준다.
				else
				{
					m_pTarget->Animator2D()->CreateAnimation(wstring(m_CurAnimName.begin(), m_CurAnimName.end())
						, CResMgr::GetInst()->FindRes<CTexture>(m_AtlasTex.Get()->GetKey())
						, m_vecFrm);
				}

				m_pOwnerUI->ChangeInfo(true);
			}

			// 현재 애니메이션에서 사용중인 아틀라스 출력 & 바꾸기
			ANCursor.y += 5.f;
			ImGui::SetCursorPos(ANCursor);
			ImGui::Text("Atlas Texture :"); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), string(m_AtlasTex->GetKey().begin(), m_AtlasTex->GetKey().end()).c_str());
			
			// 지금 프레임 정보
			ImGui::Text("\n");
			ImGui::Text("Cur Frm Info");
			ImGui::Separator();

			// 좌상단 정보 UV 띄우고 Pixel로 입력받기
			ImGui::Text("FrmLeftTop UV    :"); ImGui::SameLine(); ImGui::Text(" %.3f , %.3f", m_vecFrm[m_iSelectedIdx].vLeftTop.x, m_vecFrm[m_iSelectedIdx].vLeftTop.y);
			ImGui::Text("FrmLeftTopPixel  :"); ImGui::SameLine();
			Vec2 LeftTopPixel = m_vecFrm[m_iSelectedIdx].vLeftTop;
			LeftTopPixel *= m_vAtlasWH;
			ImGui::InputFloat2("##ANED_FrmLeftTop", LeftTopPixel, "%.f");
			m_vecFrm[m_iSelectedIdx].vLeftTop = LeftTopPixel / m_vAtlasWH;

			// 슬라이스 정보 UV 띄우고 Pixel로 입력받기
			ImGui::Text("FrmSlice UV      :"); ImGui::SameLine(); ImGui::Text(" %.3f , %.3f", m_vecFrm[m_iSelectedIdx].vSlice.x, m_vecFrm[m_iSelectedIdx].vSlice.y);
			ImGui::Text("FrmSlicePixel    :"); ImGui::SameLine();
			Vec2 SlicePixel = m_vecFrm[m_iSelectedIdx].vSlice;
			SlicePixel *= m_vAtlasWH;
			ImGui::InputFloat2("##ANED_FrmSlice", SlicePixel, "%.f");
			m_vecFrm[m_iSelectedIdx].vSlice = SlicePixel / m_vAtlasWH;

			// FullSize 정보 띄우고 입력받기
			ImGui::Text("FrmFullSize UV   :"); ImGui::SameLine(); ImGui::Text(" %.3f , %.3f", m_vecFrm[m_iSelectedIdx].vFullsize.x, m_vecFrm[m_iSelectedIdx].vFullsize.y, ImGuiInputTextFlags_ReadOnly);
			ImGui::Text("FrmFullSize Pixel:"); ImGui::SameLine();
			Vec2 FullSizePixel = m_vecFrm[m_iSelectedIdx].vFullsize;
			FullSizePixel *= m_vAtlasWH;
			if(ImGui::InputFloat2("##ANED_FullSize", FullSizePixel, "%.f", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				m_vecFrm[m_iSelectedIdx].vFullsize = FullSizePixel / m_vAtlasWH;
			}

			// Duration 정보 띄우고 입력받기
			ImGui::Text("Frm FPS          :"); ImGui::SameLine(); ImGui::InputFloat("##ANED_FrmFPS", &m_vecFrm[m_iSelectedIdx].fDuration,0.01f,0.1f);
			ImGui::SameLine();
			if (ImGui::Button("##ANED_BTN_FPSALL", ImVec2(18.f, 18.f)))
			{
				for (size_t i = 0; i < m_vecFrm.size(); ++i)
				{
					m_vecFrm[i].fDuration = m_vecFrm[m_iSelectedIdx].fDuration;
				}
			}

			// OffSet 정보 띄우고 입력받기
			// OffSet도 UV좌표로 전달해 주어야 한다.
			ImGui::Text("FrmOffSet UV     :"); ImGui::SameLine(); ImGui::InputFloat2("##ANED_OffSet", m_vecFrm[m_iSelectedIdx].vOffset, "%.3f", ImGuiInputTextFlags_ReadOnly);
			Vec2 OffsetPixel = m_vecFrm[m_iSelectedIdx].vOffset;
			OffsetPixel *= m_vAtlasWH;
			ImGui::Text("FrmOffSet Pixel  :"); ImGui::SameLine();
			if (ImGui::InputFloat2("##ANED_OffSetPixel", OffsetPixel, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				m_vecFrm[m_iSelectedIdx].vOffset = OffsetPixel / m_vAtlasWH;
			}
			

			// 지금까지 만든 애니메이션을 재생해 볼 수 있는 프리뷰
			ImGui::Text("Animation Preview");

			// 배경칠하기 
			static ImVec2 scrolling(0.0f, 0.0f);
			ImVec2 LTCursorPos = ImGui::GetCursorScreenPos();
			ImVec2 RBCursorPos = ImVec2(LTCursorPos.x + 302.f, LTCursorPos.y + 302.f);
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRectFilled(LTCursorPos, RBCursorPos, IM_COL32(50, 50, 50, 255));
			
			// Grid 추가하기
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

			// 만약 실행 버튼이 눌린다면, 애니메이션을 실행한다.
			if (m_bPlayPreview)
			{
				PlayWorkingAnim();
			}

			// 프리뷰 애니메이션 그리기
			ImTextureID my_tex_id = m_AtlasTex->GetSRV().Get();
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
			ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white ImVec2(m_TexWH.x, m_TexWH.y)

			// FullSize 만큼 잘라오기
			Vec2 vDiff = (m_vecFrm[m_AnimPreviewIdx].vFullsize - m_vecFrm[m_AnimPreviewIdx].vSlice) / 2.f;
			Vec2 FullLiftTop = m_vecFrm[m_AnimPreviewIdx].vLeftTop - vDiff;
			Vec2 FullRightBot = m_vecFrm[m_AnimPreviewIdx].vSlice + m_vecFrm[m_AnimPreviewIdx].vLeftTop + vDiff;

			ImVec2 uv_min = ImVec2(FullLiftTop);                                    // Top-left
			ImVec2 uv_max = ImVec2(FullRightBot);        // Lower-right
			ImGui::Image(my_tex_id, ImVec2(300, 300), uv_min, uv_max, tint_col, border_col);

			// 다른 아틀라스 잘라내기
			

			// Front Grid 추가하기
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
			// Center Grid 추가하기
			if (m_bGrid_Center)
			{
				draw_list->AddLine(ImVec2(LTCursorPos.x + 150.f, LTCursorPos.y), ImVec2(LTCursorPos.x + 150.f, RBCursorPos.y + 300.f), IM_COL32(255, 0, 0, 40));
				draw_list->AddLine(ImVec2(LTCursorPos.x, LTCursorPos.y + 150.f), ImVec2(RBCursorPos.x + 300.f, LTCursorPos.y + 150.f), IM_COL32(255, 0, 0, 40));
			}
			draw_list->PopClipRect();

			// 애니메이션 프리뷰 실행 버튼 생성
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

			// 애니메이션 한번만 실행 버튼 생성
			ImGui::SameLine();
			if (ImGui::Button("Play_Once##ANED_SE_PreviewStop", ImVec2(80.f, 30.f)))
			{
				m_bPlayPreview = false;
				m_bPlayOncePreview = true;
			}

			// 그리드 Enable 버튼 생성
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

			// 프리뷰 내에 중앙 표시
			ImGui::SameLine();
			if (ImGui::Button("GridCenter##ANED_SE_Grid", ImVec2(80.f, 30.f)))
			{
				m_bGrid_Center = !m_bGrid_Center;
			}

			// 그리드 픽셀 사이즈 입력받고 적용하기
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


		// ========2번째 탭=========
		// 새로운 에니메이션 만들기
		// =========================
		if (ImGui::BeginTabItem("Create Animation"))
		{

			ImGui::Text("Same Slice Edit");
			ImGui::Separator();
			
			// 현재 애니메이션에서 사용중인 아틀라스 출력 & 바꾸기
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

			// 새로 만들 최대 프레임 수 입력
			// -로 내려가는 상황이 발생할 수 있음 예외처리 해주어야 함.
			static int SEFrmCount = 0;
			ImGui::Text("FrmCount         :"); ImGui::SameLine();
			if (ImGui::InputInt("##ANED_SE_FrmCount", &SEFrmCount, 1, 10, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				ResetWorkingAnim();
				m_vecFrm.clear();
				m_vecFrm.resize(SEFrmCount);
				m_iSelectedIdx = 0;
			}

			// 아틀라스 이미지의 행렬 갯수 입력
			static int SEIdxCount[2] = {};
			ImGui::Text("Atlas Index Count:"); ImGui::SameLine();
			ImGui::InputInt2("##ANED_SE_IndexCount", SEIdxCount, ImGuiInputTextFlags_EnterReturnsTrue);

			// 슬라이스 정보
			static int NewSESlice[2] = {};
			ImGui::Text("Slice at Pixel   :"); ImGui::SameLine();
			ImGui::InputInt2("##ANED_SE_Slice", NewSESlice);

			// 좌상단 위치
			static int NewSELeftTop[2] = {};
			ImGui::Text("LeftTop at Pixel :"); ImGui::SameLine();
			ImGui::InputInt2("##ANED_SE_LeftTop", NewSELeftTop);
			
			// FullSize 정보

			// 애니메이션 재생 시간 정보 입력
			// 해당 정보는 누르면 바로vector에 넣어주도록 변경하는게 좋을 것 같다.
			// 셋팅으로 만들면 모든 정보가 초기화되는 문제가 있다.
			static float NewSEFPS = 0.f;
			ImGui::Text("Animation FPS    :"); ImGui::SameLine();
			ImGui::InputFloat("##ANED_SE_FPS", &NewSEFPS);
			
			// 설정 완료 버튼
			ImGui::Text("Aplly Animation  :"); ImGui::SameLine();
			if(ImGui::Button("##ANED_SE_Btn", ImVec2(18.f,18.f)))
			{
				if (SEFrmCount == SEIdxCount[0] * SEIdxCount[1])
				{
					// 여기서 계속 구하는게 아니라 이미지를 지정할 때 한번만 하도록 변경해야한다.
					m_vAtlasWH = Vec2(m_AtlasTex->GetWidth(), m_AtlasTex->GetHeight());

					// 슬라이스 지정
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

	// 해당 풀사이즈를 0번 인덱스에서 가져오는건 문제가 될 수도 있음.(모든 프레임의 풀사이즈가 동일한 것을 기준삼음)
	//m_vFullSize = m_vecFrm[0].vFullsize;
}

void CurAnimEditor::PlayWorkingAnim()
{
	// 시간 누적
	m_fAcctime += DT;
	// 누적 시간이 해당 프레임 유지시간을 넘어서면 다음 프레임으로 넘어간다.
	if (m_fAcctime > m_vecFrm[m_AnimPreviewIdx].fDuration)
	{
		m_fAcctime = 0.f;
		++m_AnimPreviewIdx;

		// 최대 프레임에 도달하면 Finish 상태로 전환한다.
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

	// 이번에 추가된 슬라이스가 풀사이즈보다 더 크다면 해당 사이즈를 풀사이즈로 지정
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