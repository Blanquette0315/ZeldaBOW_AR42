#include "pch.h"
#include "TextureUI.h"


TextureUI::TextureUI()
	: UI("TextureUI")
	, m_eTexUIType(TEXUI_TYPE::TILEMAP)
	, m_vSlicePixel(Vec2(120.f, 130.f))
{
	Close();
}

TextureUI::~TextureUI()
{
}

void TextureUI::update()
{
}

void TextureUI::render_update()
{
	if (nullptr != m_pTexture)
	{
		if (m_eTexUIType == TEXUI_TYPE::TILEMAP)
		{
			ImTextureID my_tex_id = m_pTexture->GetSRV().Get();
			ImVec2 Screenpos = ImGui::GetCursorScreenPos();
			ImVec2 uv_min = ImVec2(0, 0);                 // Top-left
			ImVec2 uv_max = ImVec2(1, 1);                 // Lower-right
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
			ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white ImVec2(m_TexWH.x, m_TexWH.y)
			ImGui::Image(my_tex_id, ImVec2(m_pTexture->GetWidth(), m_pTexture->GetHeight()), uv_min, uv_max, tint_col, border_col);

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (m_DBCInst && m_DBCFunc)
				{
					ImGuiIO& io = ImGui::GetIO();
					// �̹��� �������� ���콺 ��ǥ ã��
					//ImVec2 MousePos = ImGui::GetMousePos();

					float region_x = io.MousePos.x - Screenpos.x;
					float region_y = io.MousePos.y - Screenpos.y;

					// �ش� ��ǥ�� �˷��ָ�, �ε����� ã���ش�.
					// ������ / ��Ʋ�� Ÿ�� ������ �� = �ε��� [] []
					// ��Ʋ�󽺸��� ���� ����� �ٸ��� ������ ����ȭ �ؾ���.
					int idx_x = (int)region_x / 64;
					int idx_y = (int)region_y / 64;
					int idx = idx_x * 10 + idx_y;

					(m_DBCInst->*m_DBCFunc)((DWORD_PTR)idx_x * 64, (DWORD_PTR)idx_y * 64);
					int i = 0;
				}
				Close();
			}
		}

		else if (m_eTexUIType == TEXUI_TYPE::ANIMATION)
		{
			ImGuiStyle& style = ImGui::GetStyle();

			ImGui::Spacing();
			ImGui::PushID("##TEXUI_Scroll");
			ImVec2 CurWinSize = ImGui::GetWindowSize();
			ImVec2 scrolling_child_size = ImVec2(0, -(CurWinSize.x*0.5f));
			if (ImGui::BeginChild("##TEXUI_ScrollChild", scrolling_child_size, true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar))
			{

				// �̹��� ����
				ImGuiIO& io = ImGui::GetIO();
				ImTextureID my_tex_id = m_pTexture->GetSRV().Get();
				ImVec2 Screenpos = ImGui::GetCursorScreenPos();
				ImVec2 uv_min = ImVec2(0, 0);                 // Top-left
				ImVec2 uv_max = ImVec2(1, 1);                 // Lower-right
				ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
				ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white ImVec2(m_TexWH.x, m_TexWH.y)
				ImGui::Image(my_tex_id, m_vTexWH, uv_min, uv_max, tint_col, border_col);

				// �̹��� ������ Ŭ���� �߻��� �����̽���� �߶��ش�.
				if (ImGui::IsItemHovered())
				{
					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					{
						// Ŭ���� �߻��� ���� �ȼ� ��ǥ�� ����.
						Vec2 TempClickMousePos = Vec2(io.MousePos.x - Screenpos.x, io.MousePos.y - Screenpos.y);
						// Ŭ���� �ȼ��� ���� ���� �켱 Ž���� �����ؼ� �»�� ���ϴ� �ּ� ��ǥ�� ���� �Ѵ�.
						queue<Vec2>		BFS_Tex;
						m_vTemLeftTop = TempClickMousePos;		// �� ��� ��ǥ �ʱ�ȭ
						m_vTemRigthBot = TempClickMousePos;		// �� �ϴ� ��ǥ �ʱ�ȭ

						// �̹��� ������ ��ŭ �湮üũ�� �� �� �ִ� ������ �޾ƿ´�.
						vector<vector<bool>> vecVisitChk(m_pTexture->GetWidth(), vector<bool>(m_pTexture->GetHeight(), false));


						// �ȼ� ���� ������
						vector<vector<tBGRA>> PixelColorData;
						m_pTexture->GetPixelVector(PixelColorData);

						// ���� �ȼ���ǥ�� queue�� �־ Root�� ��.
						BFS_Tex.push(TempClickMousePos);
						vecVisitChk[BFS_Tex.front().x][BFS_Tex.front().y] = true;

						// Queue�� �������� �ݺ� ����
						while (!BFS_Tex.empty())
						{
							Vec2 NextLeft = BFS_Tex.front(); // 1
							if (0 < NextLeft.x)
								NextLeft.x -= 1.f;

							Vec2 NextTop = BFS_Tex.front(); // 2
							if (0 < NextTop.y)
								NextTop.y -= 1.f;

							Vec2 NextRight = BFS_Tex.front(); // 3
							if (NextRight.x < m_pTexture->GetWidth() - 1)
								NextRight.x += 1.f;
							else
								m_vTemRigthBot.x = m_pTexture->GetWidth();

							Vec2 NextBot = BFS_Tex.front(); // 4
							if (NextBot.y < m_pTexture->GetHeight() - 1)
								NextBot.y += 1.f;
							else
								m_vTemRigthBot.y = m_pTexture->GetHeight();


							if (!vecVisitChk[NextLeft.x][NextLeft.y])
							{
								// ���� üũ
								if (PixelColorData[NextLeft.x][NextLeft.y].a == 0.f)
								{
									if (NextLeft.x < m_vTemLeftTop.x)
									{
										m_vTemLeftTop.x = NextLeft.x;
									}
								}
								else
								{
									BFS_Tex.push(NextLeft);
									vecVisitChk[NextLeft.x][NextLeft.y] = true;
								}
							}

							if (!vecVisitChk[NextTop.x][NextTop.y])
							{
								if (PixelColorData[NextTop.x][NextTop.y].a == 0.f)
								{
									if (NextTop.y < m_vTemLeftTop.y)
									{
										m_vTemLeftTop.y = NextTop.y;
									}
								}
								else
								{
									BFS_Tex.push(NextTop);
									vecVisitChk[NextTop.x][NextTop.y] = true;
								}
							}

							if (!vecVisitChk[NextRight.x][NextRight.y])
							{
								if (PixelColorData[NextRight.x][NextRight.y].a == 0.f)
								{
									if (NextRight.x > m_vTemRigthBot.x)
									{
										m_vTemRigthBot.x = NextRight.x;
									}
								}
								else
								{
									BFS_Tex.push(NextRight);
									vecVisitChk[NextRight.x][NextRight.y] = true;
								}
							}

							if (!vecVisitChk[NextBot.x][NextBot.y])
							{
								if (PixelColorData[NextBot.x][NextBot.y].a == 0.f)
								{
									if (NextBot.y > m_vTemRigthBot.y)
									{
										m_vTemRigthBot.y = NextBot.y;
									}
								}
								else
								{
									BFS_Tex.push(NextBot);
									vecVisitChk[NextBot.x][NextBot.y] = true;
								}
							}

							BFS_Tex.pop();
						}

						// Ŭ���� �߻��Ǿ������� ã�Ƽ� �»�� ���ϴ��� ����ϰ� �����̽��� �����־�� �Ѵ�.
						m_vSliceLeftTop = m_vTemLeftTop;
						m_vSliceRightBot = m_vTemRigthBot;
						m_vSlicePixel = m_vSliceRightBot - m_vSliceLeftTop;
					}
				}

				// �����̽��� ����Ǹ� ������ ������ �������ش�.
				m_vTemRigthBot = m_vSliceLeftTop + m_vSlicePixel;
				m_vSliceRightBot = m_vTemRigthBot;

				// ���õ� �̹��� ǥ�����ֱ�
				Vec2 TEMLeftTop = Vec2(m_vSliceLeftTop.x + Screenpos.x, m_vSliceLeftTop.y + Screenpos.y);
				Vec2 TEMRightBot = Vec2(m_vTemRigthBot.x + Screenpos.x, m_vTemRigthBot.y + Screenpos.y);
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				draw_list->AddRect(TEMLeftTop, TEMRightBot, IM_COL32(0, 255, 0, 255));



				// Full Size �����ֱ�
				Vec2 TEMCenter = {};
				TEMCenter.x = TEMLeftTop.x + ((TEMRightBot.x - TEMLeftTop.x) * 0.5);
				TEMCenter.y = TEMLeftTop.y + ((TEMRightBot.y - TEMLeftTop.y) * 0.5);
				Vec2 SliceLeftTop = Vec2(TEMCenter.x - (m_vFullSize.x * 0.5f), TEMCenter.y - (m_vFullSize.y * 0.5f));
				Vec2 SliceRightBot = Vec2(TEMCenter.x + (m_vFullSize.x * 0.5f), TEMCenter.y + (m_vFullSize.y * 0.5f));
				draw_list = ImGui::GetWindowDrawList();
				draw_list->AddRect(SliceLeftTop, SliceRightBot, IM_COL32(255, 255, 0, 255));

				// ���� �����̽� ��ġ ���
				//TEMCenter.x = m_vTemLeftTop.x + ((m_vTemRigthBot.x - m_vTemLeftTop.x) * 0.5);
				//TEMCenter.y = m_vTemLeftTop.y + ((m_vTemRigthBot.y - m_vTemLeftTop.y) * 0.5);
				//m_vSliceLeftTop = Vec2(TEMCenter.x - (m_vSlicePixel.x * 0.5f), TEMCenter.y - (m_vSlicePixel.y * 0.5f));
				//m_vSliceRightBot = Vec2(TEMCenter.x + (m_vSlicePixel.x * 0.5f), TEMCenter.y + (m_vSlicePixel.y * 0.5f));

			}
			ImGui::EndChild();

			ImGui::Spacing();
			ImGui::PopID();

			ImGui::Text("Sample Image");
			ImTextureID my_tex_id = m_pTexture->GetSRV().Get();
			ImVec2 uv_min = ImVec2(m_vSliceLeftTop / m_vTexWH);		// Top-left
			ImVec2 uv_max = ImVec2(m_vSliceRightBot / m_vTexWH);	// Lower-right
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);		// No tint
			ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);		// 50% opaque white
			ImGui::Image(my_tex_id, ImVec2(300.f, 300.f), uv_min, uv_max, tint_col, border_col);

			ImGui::SameLine();
			m_vAnimCursor = ImGui::GetCursorPos();
			ImGui::Text("Slice Size");

			m_vAnimCursor.y += 16.f;
			ImGui::SetCursorPos(m_vAnimCursor);
			ImGui::PushItemWidth(200.f);
			ImGui::InputFloat2("##TEXUI_SliceSize", m_vSlicePixel);

			m_vAnimCursor.y += 30.f;
			ImGui::SetCursorPos(m_vAnimCursor);
			if (ImGui::Button("SET##TEXUI_SET", ImVec2(80.f, 30.f)))
			{
				if (m_OBCInst && m_OBCFunc)
				{
					// ���콺 ��ǥ : LeftTop
					m_vLeftTopUV = m_vSliceLeftTop / m_vTexWH;

					// �簢�� ��   : Slice
					m_vSliceUV = m_vSlicePixel / m_vTexWH;

					(m_OBCInst->*m_OBCFunc)((DWORD_PTR)&m_vLeftTopUV, (DWORD_PTR)&m_vSliceUV);
				}
			}
		}

	}
}