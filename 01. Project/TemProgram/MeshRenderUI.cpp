#include "pch.h"
#include "MeshRenderUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CGameObject.h>
#include <Engine/CMeshRender.h>

#include "CImGuiMgr.h"
#include "ListUI.h"
#include "TreeUI.h"

MeshRenderUI::MeshRenderUI()
	: ComponentUI("MeshRender", COMPONENT_TYPE::MESHRENDER)
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
	}

	ComponentUI::update();
}

void MeshRenderUI::render_update()
{
	ComponentUI::render_update();

	// 키 값 가져오기.
	// 이때, GetKey()의 반환은 wstring인데, string()의 생성자를 이용해 wstring을 string으로 변경해주었다.
	string MeshName, MtrlName;
	if (nullptr != m_Mesh)
	{
		MeshName = string(m_Mesh->GetKey().begin(), m_Mesh->GetKey().end());
	}
	if (nullptr != m_Material)
	{
		MtrlName = string(m_Material->GetKey().begin(), m_Material->GetKey().end());
	}

	// 메시 정보
	ImGui::Text("Mesh    ");	ImGui::SameLine(); ImGui::InputText("##MeshName", (char*)MeshName.data(), MeshName.length(), ImGuiInputTextFlags_ReadOnly);

	// 드랍체크
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
	// ImGui의 Button으로 만들어진 버튼은 만약 눌린다면 return 값으로 true가 반환된다.
	// 따라서 눌렸을 때 해당 버튼이 해야할 일들을 내부에 작성해두면 된다.
	if (ImGui::Button("##MeshBtn", Vec2(15.f, 15.f)))
	{
		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
		assert(pListUI);

		// 메쉬 목록을 받아온다.
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

	// 재질 정보
	ImGui::Text("Material");	ImGui::SameLine(); ImGui::InputText("##Mtrl", (char*)MtrlName.data(), MtrlName.length(), ImGuiInputTextFlags_ReadOnly);

	// 드랍체크
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

		// 재질 목록을 받아온다.
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
