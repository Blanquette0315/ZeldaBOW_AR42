#include "pch.h"
#include "MaterialUI.h"

#include <Engine/CMaterial.h>
#include <Engine/CResMgr.h>
#include <Engine/CEventMgr.h>
#include "ParamUI.h"

#include "CImGuiMgr.h"
#include "ListUI.h"

MaterialUI::MaterialUI()
	: ResUI("Material##UI", RES_TYPE::MATERIAL)
	, m_eSelectTexParam(TEX_PARAM::TEX_END)
{
}

MaterialUI::~MaterialUI()
{
}

void MaterialUI::update()
{
	ResUI::update();
}

void MaterialUI::render_update()
{
	ResUI::render_update();

	CMaterial* pMtrl = ((CMaterial*)GetTarget().Get());

	string strKey = string(pMtrl->GetKey().begin(), pMtrl->GetKey().end());
	string strRelativePath = string(pMtrl->GetRelativePath().begin(), pMtrl->GetRelativePath().end());
	string strName = string(pMtrl->GetName().begin(), pMtrl->GetName().end());
	ImGui::Text("Name          ");
	ImGui::SameLine();
	ImGui::InputText("##MtrlName", (char*)strName.data(), strName.length());

	ImGui::SameLine();
	if (ImGui::Button("##MtrlNameBtn", Vec2(15.f, 15.f)))
	{
		ImGui::OpenPopup("Change Mtrl Name?");
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Change Mtrl Name?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char TemName[128] = "";
		ImGui::InputText("Input Name##Mtrl", TemName, IM_ARRAYSIZE(TemName));

		ImGui::Separator();

		if (ImGui::Button("OK##Mtrl", ImVec2(120, 0)))
		{
			string NewName = TemName;
			wstring wNewName = wstring(NewName.begin(), NewName.end());
			pMtrl->SetName(wNewName);

			// 이름이 변경되었다면 변경된 이름에 알맞은 경로로 저장하기 위해서 true로 변경해준다.
			// 해당 bool값은 저장을 하게되면 false가 들어가게 된다.
			pMtrl->ChangeName(true);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel##Mtrl", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	ImGui::Text("Key           ");
	ImGui::SameLine();
	ImGui::InputText("##MtrlKey", (char*)strKey.data(), strKey.length(), ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Path          ");
	ImGui::SameLine();
	ImGui::InputText("##MtrlPath", (char*)strRelativePath.data(), strRelativePath.length(), ImGuiInputTextFlags_ReadOnly);

	string strShaderKey;
	if (nullptr != pMtrl->GetShader())
	{
		strShaderKey = string(pMtrl->GetShader()->GetKey().begin(), pMtrl->GetShader()->GetKey().end());
	}

	ImGui::Text("GraphicsShader");
	ImGui::SameLine();
	ImGui::InputText("##ShaderName", (char*)strShaderKey.data(), strShaderKey.length(), ImGuiInputTextFlags_ReadOnly);

	// Shader 변경 및 선택
	ImGui::SameLine();
	if (ImGui::Button("##ShaderBtn", Vec2(15.f, 15.f)))
	{
		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
		assert(pListUI);

		// 목록을 받아와서, ListUI에 전달
		const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResource(RES_TYPE::GRAPHICS_SHADER);
		static vector<wstring> vecRes;
		vecRes.clear();

		map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.begin();
		for (; iter != mapRes.end(); ++iter)
		{
			vecRes.push_back(iter->first);
		}
		pListUI->SetItemList(vecRes);
		pListUI->AddDynamicDBClicked(this, (FUNC_1)&MaterialUI::SetShader);

		pListUI->Open();
	}

	if (ImGui::Button("Save Mtrl", ImVec2(90.f, 30.f)))
	{
		// 앤진 리소스라면 저장하면 안된다.
		if (!pMtrl->IsEngineRes())
		{
			// 처음 저장하는 거라면, 경로까지 지정을 해서 저장을 해주어야 한다.
			// 덮어 씌워 저장한다면, 경로를 재지정 하면 안된다.
			if (L"" == pMtrl->GetRelativePath())
			{
				wstring strRelativePath;

				strRelativePath = L"material\\";
				strRelativePath += pMtrl->GetKey();
				strRelativePath += L".mtrl";
				pMtrl->Save(strRelativePath);

				// 경로가 없던 임시 재질은 해제해 주어야 한다.
				tEvent DelMtrl = {};
				DelMtrl.eType = EVENT_TYPE::DELETE_RES;
				DelMtrl.wParam = (DWORD_PTR)pMtrl->GetResType();
				DelMtrl.lParam = (DWORD_PTR)pMtrl;

				CEventMgr::GetInst()->AddEvent(DelMtrl);

			}
			else
			{
				// 이름과 키가 일치하지 않는다면 이름 값으로 변경해주어야 한다.
				// 이름을 경로 처럼 변경 하거나 경로를 이름 처럼 변경해서 비교해야한다.
				// 우선은 이름을 경로처럼 변경하는 방식으로 진행할 것이다.

				wstring strNamePath = {};

				strNamePath = L"material\\";
				strNamePath += pMtrl->GetName();
				strNamePath += L".mtrl";

				if (strNamePath != pMtrl->GetKey())
				{
					pMtrl->Save(strNamePath);
					// 저장이 되고나면 이름이 아에 정해진 것이기 때문에 다시 false를 넣어주어야 한다.
					pMtrl->ChangeName(false);
				}

				// 이름과 키값이 일치하면 이전 경로 그대로를 넣어 덮어씌운다.
				else
				{
					pMtrl->Save(pMtrl->GetRelativePath());
				}
			}
		}
	}

	// Shader Parameter
	ImGui::NewLine();
	ImGui::Text("Shader Parameter");

	// Material에 Shader가 셋팅되어 있지 않다면
	if (nullptr != pMtrl->GetShader())
	{
		const vector<tScalarParam> vecScalar = pMtrl->GetShader()->GetScalarParam();
		for (size_t i = 0; i < vecScalar.size(); ++i)
		{
			switch (vecScalar[i].eParam)
			{
			case INT_0:
			case INT_1:
			case INT_2:
			case INT_3:
			{
				int iInput, iOutput;
				pMtrl->GetScalarParam(vecScalar[i].eParam, &iInput);
				pMtrl->GetScalarParam(vecScalar[i].eParam, &iOutput);
				ParamUI::Param_Int(vecScalar[i].strName, &iOutput);
				pMtrl->SetScalarParam(vecScalar[i].eParam, &iOutput);

				// 임시로 해두는 저장. 여기서 할게 아니라 이제 단축키 등을 만들어서 저장을 해야한다.
				if (iInput != iOutput)
				{
					pMtrl->Save(L"material\\mtrl.mtrl");
				}
			}
			break;
			case FLOAT_0:
			case FLOAT_1:
			case FLOAT_2:
			case FLOAT_3:
			{
				float fData = 0;
				pMtrl->GetScalarParam(vecScalar[i].eParam, &fData);
				ParamUI::Param_Float(vecScalar[i].strName, &fData);
				pMtrl->SetScalarParam(vecScalar[i].eParam, &fData);
			}
			break;
			case VEC2_0:
			case VEC2_1:
			case VEC2_2:
			case VEC2_3:
			{
				Vec2 data;
				pMtrl->GetScalarParam(vecScalar[i].eParam, &data);
				ParamUI::Param_Vec2(vecScalar[i].strName, &data);
				pMtrl->SetScalarParam(vecScalar[i].eParam, &data);
			}
			break;
			case VEC4_0:
			case VEC4_1:
			case VEC4_2:
			case VEC4_3:
			{
				Vec4 data;
				pMtrl->GetScalarParam(vecScalar[i].eParam, &data);
				ParamUI::Param_Vec4(vecScalar[i].strName, &data);
				pMtrl->SetScalarParam(vecScalar[i].eParam, &data);
			}
			break;
			case MAT_0:
			case MAT_1:
			case MAT_2:
			case MAT_3:
			{
				break;
			}
			}
		}

		const vector<tTextureParam> vecTex = pMtrl->GetShader()->GetTextureParam();
		for (size_t i = 0; i < vecTex.size(); ++i)
		{
			Ptr<CTexture> pTex = pMtrl->GetTexParam(vecTex[i].eParam);
			if (ParamUI::Param_Tex(vecTex[i].strName, pTex, this, (FUNC_1)&MaterialUI::SetTexture))
			{
				m_eSelectTexParam = vecTex[i].eParam;
			}
			else
			{
				pMtrl->SetTexParam(vecTex[i].eParam, pTex);
			}
		}
	}
	
}

void MaterialUI::SetTexture(DWORD_PTR _strTexKey)
{
	string strKey = (char*)_strTexKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstrKey);
	assert(nullptr != pTex);

	CMaterial* pMtrl = ((CMaterial*)GetTarget().Get());
	// 이때 우리가 선택했던 TexParam이 몇번 째인지 알기 위해서 멤버 변수로 추가해주었던 것이다.
	pMtrl->SetTexParam(m_eSelectTexParam, pTex);
}

void MaterialUI::SetShader(DWORD_PTR _strShaderKey)
{
	string strKey = (char*)_strShaderKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CGrapicsShader> pShader = CResMgr::GetInst()->FindRes<CGrapicsShader>(wstrKey);
	assert(nullptr != pShader);

	((CMaterial*)GetTarget().Get())->SetShader(pShader);
}