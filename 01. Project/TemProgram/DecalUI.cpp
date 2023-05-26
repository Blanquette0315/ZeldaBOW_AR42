#include "pch.h"
#include "DecalUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CGameObject.h>
#include <Engine/CDecal.h>

#include "CImGuiMgr.h"
#include "ListUI.h"

#include "TreeUI.h"
#include "ParamUI.h"

DecalUI::DecalUI()
	: ComponentUI("DecalUI", COMPONENT_TYPE::DECAL)
	, m_eSelectTexParam(TEX_PARAM::TEX_END)
	, m_bIsDyanmicMtrl(false)
	, m_DecalTexture(nullptr)
	, m_bShowDebugDraw(false)
{
}

DecalUI::~DecalUI()
{
}

void DecalUI::update()
{
	if (GetTarget())
	{
		m_Mesh = GetTarget()->Decal()->GetMesh();
		m_Material = GetTarget()->Decal()->GetCurMaterial();
		m_DecalTexture = GetTarget()->Decal()->GetDecalTexture();
		m_bDeferred = GetTarget()->Decal()->IsDeferred();
		if (GetTarget()->Decal()->IsDynamicMtrl())
		{
			m_bIsDyanmicMtrl = true;
		}
		else
		{
			m_bIsDyanmicMtrl = false;
		}
		m_bShowDebugDraw = GetTarget()->Decal()->Is_ShowDebugDraw();
	}

	ComponentUI::update();
}

void DecalUI::render_update()
{
	ComponentUI::render_update();

	// Show DebugDraw
	ImGui::Text("ShowDebugDraw"); ImGui::SameLine(); ImGui::Checkbox("##DecalDebugDraw", &m_bShowDebugDraw);
	GetTarget()->Decal()->ShowDebugDraw(m_bShowDebugDraw);

	// Ű �� ��������.
	// �̶�, GetKey()�� ��ȯ�� wstring�ε�, string()�� �����ڸ� �̿��� wstring�� string���� �������־���.
	string MeshName, MtrlName;
	if (nullptr != m_Mesh)
	{
		MeshName = string(m_Mesh->GetName().begin(), m_Mesh->GetName().end());
	}
	if (nullptr != m_Material)
	{
		MtrlName = string(m_Material->GetName().begin(), m_Material->GetName().end());
	}

	// �޽� ����
	ImGui::Text("Mesh    ");	ImGui::SameLine(); ImGui::InputText("##MeshName", (char*)MeshName.data(), MeshName.length(), ImGuiInputTextFlags_ReadOnly);

	// ���üũ
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##ContentTree"))
		{
			TreeNode* pNode = (TreeNode*)payload->Data;
			CRes* pRes = (CRes*)pNode->GetData();

			if (RES_TYPE::MESH == pRes->GetResType())
			{
				string strKey = string(pRes->GetKey().begin(), pRes->GetKey().end());
				SetMesh((DWORD_PTR)strKey.c_str());
			}
		}
		ImGui::EndDragDropTarget();
	}


	ImGui::SameLine();

	// ImGui�� Button���� ������� ��ư�� ���� �����ٸ� return ������ true�� ��ȯ�ȴ�.
	// ���� ������ �� �ش� ��ư�� �ؾ��� �ϵ��� ���ο� �ۼ��صθ� �ȴ�.
	if (ImGui::Button("##MeshBtn", Vec2(15.f, 15.f)))
	{
		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
		assert(pListUI);

		// �޽� ����� �޾ƿ´�.
		const map<wstring, Ptr<CRes>> MapRes = CResMgr::GetInst()->GetResource(RES_TYPE::MESH);
		static vector<wstring> vecRes;
		vecRes.clear();

		map<wstring, Ptr<CRes>>::const_iterator iter = MapRes.begin();
		for (; iter != MapRes.end(); ++iter)
		{
			vecRes.push_back(iter->first);
		}
		pListUI->SetItemList(vecRes);
		pListUI->AddDynamicDBClicked(this, (FUNC_1)&DecalUI::SetMesh);

		pListUI->Open();
	}

	// material 
	ImGui::Text("Material ");
	ImGui::SameLine();
	ImGui::InputText("##MtrlName", (char*)MtrlName.data(), MtrlName.length(), ImGuiInputTextFlags_ReadOnly);

	if (ImGui::RadioButton("Forward", !m_bDeferred))
	{
		m_bDeferred = false;
		GetTarget()->Decal()->SetRenderType(m_bDeferred);
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Deferred", m_bDeferred))
	{
		m_bDeferred = true;
		GetTarget()->Decal()->SetRenderType(m_bDeferred);
	}

	// material type
	ImGui::Text("Shared"); ImGui::SameLine(); if (ImGui::RadioButton("##SharedMtrl", !m_bIsDyanmicMtrl)) { GetTarget()->Decal()->GetSharedMaterial(); };
	ImGui::SameLine();
	ImGui::Text("Dynamic"); ImGui::SameLine(); if (ImGui::RadioButton("##DynamicMtrl", m_bIsDyanmicMtrl)) { GetTarget()->Decal()->GetDynamicMaterial(); };

	ShowShaderParam(m_Material.Get());

	//if (m_bIsDyanmicMtrl)
	//{
	//	// ShowShaderParam(m_Material.Get());
	//}
	//else
	//{
	//	if (nullptr != m_DecalTexture)
	//	{
	//		ImGui::Image(m_DecalTexture->GetSRV().Get(), ImVec2(100.f, 100.f));
	//	}
	//	else
	//	{
	//		ImGui::Image(nullptr, ImVec2(100.f, 100.f));
	//	}
	//}
}

void DecalUI::SetMesh(DWORD_PTR _strMeshKey)
{
	string strKey = (char*)_strMeshKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(wstrKey);
	assert(nullptr != pMesh);

	GetTarget()->Decal()->SetMesh(pMesh);
}

void DecalUI::SetMaterial(DWORD_PTR _strMaterialKey)
{
	string strKey = (char*)_strMaterialKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(wstrKey);
	assert(nullptr != pMtrl);

	GetTarget()->Decal()->SetSharedMaterial(pMtrl);
}

void DecalUI::SetTexture(DWORD_PTR _strTexKey)
{
	string strKey = (char*)_strTexKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstrKey);
	assert(nullptr != pTex);

	// �̶� �츮�� �����ߴ� TexParam�� ��� °���� �˱� ���ؼ� ��� ������ �߰����־��� ���̴�.
	// m_Material->SetTexParam(m_eSelectTexParam, pTex);
	GetTarget()->Decal()->SetDecalTexture(pTex);
}

void DecalUI::ShowShaderParam(CMaterial* _pMtrl)
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
				int iInput, iOutput;
				_pMtrl->GetScalarParam(vecScalar[i].eParam, &iInput);
				_pMtrl->GetScalarParam(vecScalar[i].eParam, &iOutput);
				ParamUI::Param_Int(vecScalar[i].strName, &iOutput);
				_pMtrl->SetScalarParam(vecScalar[i].eParam, &iOutput);
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
			if (ParamUI::Param_Tex(vecTex[i].strName, pTex, this, (FUNC_1)&DecalUI::SetTexture))
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
