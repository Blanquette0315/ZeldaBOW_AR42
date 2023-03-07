#include "pch.h"
#include "MeshRenderUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CGameObject.h>
#include <Engine/CMeshRender.h>

#include "CImGuiMgr.h"
#include "ListUI.h"
#include "TreeUI.h"

#include "ComboBoxUI.h"
#include "ParamUI.h"

MeshRenderUI::MeshRenderUI()
	: ComponentUI("MeshRender", COMPONENT_TYPE::MESHRENDER)
	, m_bIsDyanmicMtrl(false)
{
}

MeshRenderUI::~MeshRenderUI()
{
}

void MeshRenderUI::update()
{
	if (nullptr != GetTarget())
	{
		m_Mesh = GetTarget()->MeshRender()->GetMesh();
		m_Material = GetTarget()->MeshRender()->GetCurMaterial();
		m_bIsDyanmicMtrl = GetTarget()->MeshRender()->IsDynamicMtrl();
	}

	ComponentUI::update();
}

void MeshRenderUI::render_update()
{
	ComponentUI::render_update();

	// Ű �� ��������.
	// �̶�, GetKey()�� ��ȯ�� wstring�ε�, string()�� �����ڸ� �̿��� wstring�� string���� �������־���.
	string MeshName, MtrlName;
	if (nullptr != m_Mesh)
	{
		MeshName = string(m_Mesh->GetKey().begin(), m_Mesh->GetKey().end());
	}
	if (nullptr != m_Material)
	{
		MtrlName = string(m_Material->GetKey().begin(), m_Material->GetKey().end());
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
		pListUI->AddDynamicDBClicked(this, (FUNC_1)&MeshRenderUI::SetMesh);

		pListUI->Open();
	}

	// ���� ����
	ImGui::Text("Material");	ImGui::SameLine(); ImGui::InputText("##Mtrl", (char*)MtrlName.data(), MtrlName.length(), ImGuiInputTextFlags_ReadOnly);

	// ���üũ
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##ContentTree"))
		{
			TreeNode* pNode = (TreeNode*)payload->Data;
			CRes* pRes = (CRes*)pNode->GetData();

			if (RES_TYPE::MATERIAL == pRes->GetResType())
			{
				string strKey = string(pRes->GetKey().begin(), pRes->GetKey().end());
				SetMaterial((DWORD_PTR)strKey.c_str());
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("##MtrlBtn", Vec2(15.f, 15.f)))
	{
		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
		assert(pListUI);

		// ���� ����� �޾ƿ´�.
		const map<wstring, Ptr<CRes>> MapRes = CResMgr::GetInst()->GetResource(RES_TYPE::MATERIAL);
		static vector<wstring> vecRes;
		vecRes.clear();

		map<wstring, Ptr<CRes>>::const_iterator iter = MapRes.begin();
		for (; iter != MapRes.end(); ++iter)
		{
			vecRes.push_back(iter->first);
		}
		pListUI->SetItemList(vecRes);
		pListUI->AddDynamicDBClicked(this, (FUNC_1)&MeshRenderUI::SetMaterial);

		pListUI->Open();
	}

	// material type
	if (ImGui::RadioButton("##SharedMtrl", !m_bIsDyanmicMtrl)) { GetTarget()->MeshRender()->GetSharedMaterial(); };
	ImGui::SameLine();
	if(ImGui::RadioButton("##DynamicMtrl", m_bIsDyanmicMtrl)) { GetTarget()->MeshRender()->GetDynamicMaterial(); };

	if(m_bIsDyanmicMtrl)
	{
		ShowShaderParam(m_Material.Get());
	}
}

void MeshRenderUI::SetMesh(DWORD_PTR _strMeshKey)
{
	string strKey = (char*)_strMeshKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CMesh> pMesh = CResMgr::GetInst()->FindRes<CMesh>(wstrKey);
	assert(nullptr != pMesh);

	GetTarget()->MeshRender()->SetMesh(pMesh);
}

void MeshRenderUI::SetMaterial(DWORD_PTR _strMaterialKey)
{
	string strKey = (char*)_strMaterialKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(wstrKey);
	assert(nullptr != pMtrl);

	GetTarget()->MeshRender()->SetSharedMaterial(pMtrl);
}

void MeshRenderUI::SetTexture(DWORD_PTR _strTexKey)
{
	string strKey = (char*)_strTexKey;
	wstring wstrKey = wstring(strKey.begin(), strKey.end());

	Ptr<CTexture> pTex = CResMgr::GetInst()->FindRes<CTexture>(wstrKey);
	assert(nullptr != pTex);

	// �̶� �츮�� �����ߴ� TexParam�� ��� °���� �˱� ���ؼ� ��� ������ �߰����־��� ���̴�.
	m_Material->SetTexParam(m_eSelectTexParam, pTex);
}

void MeshRenderUI::ShowShaderParam(CMaterial* _pMtrl)
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

				// �ӽ÷� �صδ� ����. ���⼭ �Ұ� �ƴ϶� ���� ����Ű ���� ���� ������ �ؾ��Ѵ�.
				if (iInput != iOutput)
				{
					_pMtrl->Save(L"material\\mtrl.mtrl");
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
			if (ParamUI::Param_Tex(vecTex[i].strName, pTex, this, (FUNC_1)&MeshRenderUI::SetTexture))
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
