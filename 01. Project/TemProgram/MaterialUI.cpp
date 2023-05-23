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

			// �̸��� ����Ǿ��ٸ� ����� �̸��� �˸��� ��η� �����ϱ� ���ؼ� true�� �������ش�.
			// �ش� bool���� ������ �ϰԵǸ� false�� ���� �ȴ�.
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

	// Shader ���� �� ����
	ImGui::SameLine();
	if (ImGui::Button("##ShaderBtn", Vec2(15.f, 15.f)))
	{
		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
		assert(pListUI);

		// ����� �޾ƿͼ�, ListUI�� ����
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
		// ���� ���ҽ���� �����ϸ� �ȵȴ�.
		if (!pMtrl->IsEngineRes())
		{
			// ó�� �����ϴ� �Ŷ��, ��α��� ������ �ؼ� ������ ���־�� �Ѵ�.
			// ���� ���� �����Ѵٸ�, ��θ� ������ �ϸ� �ȵȴ�.
			if (L"" == pMtrl->GetRelativePath())
			{
				wstring strRelativePath;

				strRelativePath = L"material\\";
				strRelativePath += pMtrl->GetKey();
				strRelativePath += L".mtrl";
				pMtrl->Save(strRelativePath);

				// ��ΰ� ���� �ӽ� ������ ������ �־�� �Ѵ�.
				tEvent DelMtrl = {};
				DelMtrl.eType = EVENT_TYPE::DELETE_RES;
				DelMtrl.wParam = (DWORD_PTR)pMtrl->GetResType();
				DelMtrl.lParam = (DWORD_PTR)pMtrl;

				CEventMgr::GetInst()->AddEvent(DelMtrl);

			}
			else
			{
				// �̸��� ����Ǿ����ٸ�, �ٲ� �̸����� ��θ� ������ �����Ѵ�.
                wstring strNamePath = {};
                
				if (pMtrl->GetRelativePath().empty())
				{
					strNamePath = L"material\\";
					strNamePath += pMtrl->GetName();
					strNamePath += L".mtrl";
				}
				else
					strNamePath = pMtrl->GetRelativePath();

				if (strNamePath != pMtrl->GetKey())
				{
					pMtrl->Save(strNamePath);
					// ������ �ǰ����� �̸��� �ƿ� ������ ���̱� ������ �ٽ� false�� �־��־�� �Ѵ�.
					pMtrl->ChangeName(false);
				}

				// �̸��� Ű���� ��ġ�ϸ� ���� ��� �״�θ� �־� ������.
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

	// Material�� Shader�� ���õǾ� ���� �ʴٸ�
	ShowShaderParam(pMtrl);
}

void MaterialUI::SetTexture(DWORD_PTR _strTexKey)
{
	string strKey = (char*)_strTexKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstrKey);
	assert(nullptr != pTex);

	CMaterial* pMtrl = ((CMaterial*)GetTarget().Get());
	// �̶� �츮�� �����ߴ� TexParam�� ��� °���� �˱� ���ؼ� ��� ������ �߰����־��� ���̴�.
	pMtrl->SetTexParam(m_eSelectTexParam, pTex);
}

void MaterialUI::SetShader(DWORD_PTR _strShaderKey)
{
	string strKey = (char*)_strShaderKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CGraphicsShader> pShader = CResMgr::GetInst()->FindRes<CGraphicsShader>(wstrKey);
	assert(nullptr != pShader);

	((CMaterial*)GetTarget().Get())->SetShader(pShader);
}

void MaterialUI::ShowShaderParam(CMaterial* _pMtrl)
{
	if (nullptr != _pMtrl->GetShader())
	{
		const vector<tScalarParam> vecScalar = _pMtrl->GetShader()->GetScalarParam();
		for (size_t i = 0; i < vecScalar.size(); ++i)
		{
			switch (vecScalar[i].eParam)
			{
			case INT_0:
			case INT_1:
			case INT_2:
			case INT_3:
			{
				int iInput = 0;
				_pMtrl->GetScalarParam(vecScalar[i].eParam, &iInput);
				if (ParamUI::Param_Int(vecScalar[i].strName, &iInput))
				{
					_pMtrl->SetScalarParam(vecScalar[i].eParam, &iInput);
				}
			}
			break;
			case FLOAT_0:
			case FLOAT_1:
			case FLOAT_2:
			case FLOAT_3:
			{
				float fData = 0;
				_pMtrl->GetScalarParam(vecScalar[i].eParam, &fData);
				ParamUI::Param_Float(vecScalar[i].strName, &fData);
				_pMtrl->SetScalarParam(vecScalar[i].eParam, &fData);
			}
			break;
			case VEC2_0:
			case VEC2_1:
			case VEC2_2:
			case VEC2_3:
			{
				Vec2 data;
				_pMtrl->GetScalarParam(vecScalar[i].eParam, &data);
				ParamUI::Param_Vec2(vecScalar[i].strName, &data);
				_pMtrl->SetScalarParam(vecScalar[i].eParam, &data);
			}
			break;
			case VEC4_0:
			case VEC4_1:
			case VEC4_2:
			case VEC4_3:
			{
				Vec4 data;
				_pMtrl->GetScalarParam(vecScalar[i].eParam, &data);
				ParamUI::Param_Vec4(vecScalar[i].strName, &data);
				_pMtrl->SetScalarParam(vecScalar[i].eParam, &data);
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

		const vector<tTextureParam> vecTex = _pMtrl->GetShader()->GetTextureParam();
		for (size_t i = 0; i < vecTex.size(); ++i)
		{
			Ptr<CTexture> pTex = _pMtrl->GetTexParam(vecTex[i].eParam);
			if (ParamUI::Param_Tex(vecTex[i].strName, pTex, this, (FUNC_1)&MaterialUI::SetTexture))
			{
				m_eSelectTexParam = vecTex[i].eParam;
			}
			else
			{
				_pMtrl->SetTexParam(vecTex[i].eParam, pTex);
			}
		}
	}
}
