#include "pch.h"
#include "TileMapUI.h"

#include <Engine/CTileMap.h>
#include <Engine/CResMgr.h>

#include "CImGuiMgr.h"
#include "ListUI.h"
#include "TextureUI.h"
#include "TileMapTableUI.h"

TileMapUI::TileMapUI()
	: ComponentUI("TileMap", COMPONENT_TYPE::TILEMAP)
	, m_TileCount(0)
	, m_CurIdx{}
	, m_bFillOnce(false)
	, m_bFillSelectIdx(false)
{
}

TileMapUI::~TileMapUI()
{
}

void TileMapUI::update()
{
	if (nullptr != GetTarget())
	{
		m_TexName = string(GetTarget()->TileMap()->GetTextureName().begin(), GetTarget()->TileMap()->GetTextureName().end());
		
		m_TileIdx[0] = (int)GetTarget()->TileMap()->GetTileCount().x;
		m_TileIdx[1] = (int)GetTarget()->TileMap()->GetTileCount().y;
		m_TileCount = m_TileIdx[0] * m_TileIdx[1];

		m_vecTile = GetTarget()->TileMap()->GetTileVector();
		m_TexWH = GetTarget()->TileMap()->GetTexWH();
	}

	ComponentUI::update();
}

void TileMapUI::render_update()
{
	ComponentUI::render_update();

	ImGui::Text("Cur Texture   "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), m_TexName.c_str());
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
		pListUI->AddDynamicDBClicked(this, (FUNC_1)&TileMapUI::SetAtlasTex);

		pListUI->Open();
	}

	// 이전 인덱스 값을 알아두는 이유는 값이 변했을 때만 SetTileCount를 호출해서 초기화를 실행시켜주기 위함.
	static int PrevTileCount[2] = {};
	PrevTileCount[0] = m_TileIdx[0]; PrevTileCount[1] = m_TileIdx[1];
	ImGui::Text("Set Tile Count"); ImGui::SameLine(); ImGui::InputInt2("##TIM_SetTileCount", m_TileIdx, ImGuiInputTextFlags_EnterReturnsTrue);

	ImGui::Text("Tile Count    "); ImGui::SameLine(); ImGui::Text("%.0u", m_TileCount);
	ImGui::Text("Tile Index    "); ImGui::SameLine(); ImGui::Text("[%.0u] [%.0u]", m_TileIdx[0], m_TileIdx[1]);

	// 사용할 모드 별 Tab Bar
	ImGui::BeginTabBar("##TIM_WorkTabBar");
	{
		if (ImGui::BeginTabItem("Std Work"))
		{
			m_bFillOnce = false;
			m_bFillSelectIdx = false;

			ImGui::Text("Cur Index     "); ImGui::SameLine(); ImGui::InputInt2("##TIM_CurIdx", m_CurIdx);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Fill at Once"))
		{
			m_bFillOnce = true;
			m_bFillSelectIdx = false;

			ImGui::Text("Start Index   "); ImGui::SameLine(); ImGui::InputInt2("##TIM_FillStartIdx", m_StartIdx);
			ImGui::Text("End Index     "); ImGui::SameLine(); ImGui::InputInt2("##TIM_FillEndIdx", m_EndIdx);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Select Idx Fill"))
		{
			m_bFillOnce = false;
			m_bFillSelectIdx = true;

			// 인덱스 테이블 버튼
			ImGui::Text("Open Table    "); ImGui::SameLine(); //TempTileTable
			if (ImGui::Button("##TIM_SetOpenTBtn", Vec2(18.f, 18.f)))
			{
				TileMapTableUI* pTileMapTableUI = dynamic_cast<TileMapTableUI*>(CImGuiMgr::GetInst()->FindUI("TempTileTable"));
				assert(pTileMapTableUI);

				pTileMapTableUI->SetIdxCount(Vec2(m_TileIdx[0], m_TileIdx[1]));
				pTileMapTableUI->AddSetClicked(this, (FUNC_1)&TileMapUI::FillSelectIdx);

				pTileMapTableUI->Open();
			}

			ImGui::EndTabItem();
		}
	}
	ImGui::EndTabBar();

	ImGui::Text("Cur Index Info");
	Vec2 TemLeftTop = m_vecTile[(m_CurIdx[0] * m_TileIdx[1]) + m_CurIdx[1]].vLeftTop;
	TemLeftTop *= m_TexWH;
	ImGui::Text("     Tile LeftTop"); ImGui::SameLine(); ImGui::InputFloat2("##TIM_CurIdxLeftTop", TemLeftTop, nullptr, ImGuiInputTextFlags_ReadOnly);
	Vec2 TemSlice = m_vecTile[(m_CurIdx[0] * m_TileIdx[1]) + m_CurIdx[1]].vSlice;
	TemSlice *= m_TexWH;
	ImGui::Text("     Tile Slice  "); ImGui::SameLine(); ImGui::InputFloat2("##TIM_CurIdxSlice", TemSlice, nullptr, ImGuiInputTextFlags_ReadOnly);

	// 이미지 출력
	ImGui::Text("     Sample Image");
	ImGui::SameLine();
	
	ImTextureID my_tex_id = CResMgr::GetInst()->FindRes<CTexture>(wstring(m_TexName.begin(), m_TexName.end()))->GetSRV().Get();
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white ImVec2(m_TexWH.x, m_TexWH.y)
	if (m_bFillSelectIdx)
	{
		TemLeftTop = m_SelectIdxLeftTop * m_TexWH;
		TemSlice += TemLeftTop;
		TemSlice /= m_TexWH;

		ImVec2 uv_min = ImVec2(m_SelectIdxLeftTop);       // Top-left
		ImVec2 uv_max = ImVec2(TemSlice);                 // Lower-right
		ImGui::Image(my_tex_id, ImVec2(50, 50), uv_min, uv_max, tint_col, border_col);
	}
	else
	{
		TemSlice += TemLeftTop;
		TemLeftTop /= m_TexWH;
		TemSlice /= m_TexWH;

		ImVec2 uv_min = ImVec2(TemLeftTop);                 // Top-left
		ImVec2 uv_max = ImVec2(TemSlice);                 // Lower-right
		ImGui::Image(my_tex_id, ImVec2(50, 50), uv_min, uv_max, tint_col, border_col);
	}
	
	
	ImGui::SameLine();
	
	if (ImGui::Button("##TIM_SetSamTexBtn", Vec2(18.f, 18.f)))
	{
		TextureUI* pTextureUI = dynamic_cast<TextureUI*>(CImGuiMgr::GetInst()->FindUI("TextureUI"));
		assert(pTextureUI);
		
		pTextureUI->SetTexture(CResMgr::GetInst()->FindRes<CTexture>(wstring(m_TexName.begin(), m_TexName.end())));
		pTextureUI->SetTextureWH(m_TexWH);
		pTextureUI->SetTexUIType(TEXUI_TYPE::TILEMAP);
		pTextureUI->AddDynamicDBClicked(this, (FUNC_2)&TileMapUI::SetLeftTop);
	
		pTextureUI->Open();
	}

	
	if (GetTarget())
	{
		if (PrevTileCount[0] != m_TileIdx[0] || PrevTileCount[1] != m_TileIdx[1])
		{
			GetTarget()->TileMap()->SetTileCount(m_TileIdx[0], m_TileIdx[1]);
		}
	}
}

void TileMapUI::SetAtlasTex(DWORD_PTR _strTextureKey)
{
	string strKey = (char*)_strTextureKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(wstrKey);
	assert(nullptr != pTexture);

	GetTarget()->TileMap()->SetTileAtlas(pTexture);
	GetTarget()->TileMap()->SetTileCount(m_TileIdx[0], m_TileIdx[1]);
}

void TileMapUI::SetLeftTop(DWORD_PTR _Left, DWORD_PTR _Top)
{
	Vec2 TempLeftTop = Vec2((float)_Left, (float)_Top);
	// UV로 변환
	TempLeftTop /= m_TexWH;

	if (m_bFillSelectIdx)
	{
		m_SelectIdxLeftTop = TempLeftTop;
	}
	else
	{
		if (m_bFillOnce)
		{
			for (int i = m_StartIdx[0] * m_TileIdx[1] + m_StartIdx[1]; i < m_EndIdx[0] * m_TileIdx[1] + m_EndIdx[1]; ++i)
			{
				m_vecTile[i].vLeftTop = TempLeftTop;
			}
		}
		else
		{
			m_vecTile[m_CurIdx[0] * m_TileIdx[1] + m_CurIdx[1]].vLeftTop = TempLeftTop;
		}

		if (GetTarget())
		{
			GetTarget()->TileMap()->SetTileVector(m_vecTile);
		}
	}
}

void TileMapUI::FillSelectIdx(DWORD_PTR _vecIdx)
{
	int vecIdx = (int)_vecIdx;
	
	m_vecTile[vecIdx].vLeftTop = m_SelectIdxLeftTop;

	if (GetTarget())
	{
		GetTarget()->TileMap()->SetTileVector(m_vecTile);
	}
}
