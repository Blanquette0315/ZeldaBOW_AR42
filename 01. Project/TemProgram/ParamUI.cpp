#include "pch.h"
#include "ParamUI.h"

#include <Engine/CResMgr.h>

#include "imgui.h"
#include "CImGuiMgr.h"
#include "ListUI.h"
#include "TreeUI.h"
#include "ScriptUI.h"

// 카운트 초기화부분
UINT ParamUI::ParamCount = 0;


bool ParamUI::Param_Int(const string& _ParamName, int* _pInOut)
{
	char szName[50] = "";
	sprintf_s(szName, 50, "##Int%d", ParamCount++);

	ImGui::Text(_ParamName.c_str());
	ImGui::SameLine();
	if (ImGui::InputInt(szName, _pInOut))
	{
		return true;
	}

	return false;
}

void ParamUI::Param_Float(const string& _ParamName, float* _pInOut, float _fMin, float _fMax, float _fStep)
{
	char szName[50] = "";
	sprintf_s(szName, 50, "##Float%d", ParamCount++);

	ImGui::Text(_ParamName.c_str());
	ImGui::SameLine();

	if (ImGui::DragFloat(szName, _pInOut, _fStep, _fMin, _fMax, "%6.3f"))
	{
		if (ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			UI::ResetFocus();
		}
	}

	//if (ImGui::InputFloat(szName, _pInOut, 0.f, 0.f, "%6.3f", ImGuiInputTextFlags_EnterReturnsTrue))
	//{
	//	UI::ResetFocus();
	//}
}

void ParamUI::Param_Vec2(const string& _ParamName, Vec2* _pInOut)
{
	char szName[50] = "";
	sprintf_s(szName, 50, "##Vec2%d", ParamCount++);

	ImGui::Text(_ParamName.c_str());
	ImGui::SameLine();

	if (ImGui::DragFloat2(szName, *_pInOut, 0.01f, 0.f, 1.f, "%6.3f"))
	{
		if (ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			UI::ResetFocus();
		}
	}

}

void ParamUI::Param_Vec3(const string& _ParamName, Vec3* _pInOut)
{
	char szName[50] = "";
	sprintf_s(szName, 50, "##Vec3%d", ParamCount++);

	ImGui::Text(_ParamName.c_str());
	ImGui::SameLine();

	if (ImGui::DragFloat3(szName, *_pInOut, 0.01f, 0.f, 1.f, "%6.3f"))
	{
		if (ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			UI::ResetFocus();
		}
	}
}

void ParamUI::Param_Vec4(const string& _ParamName, Vec4* _pInOut)
{
	char szName[50] = "";
	sprintf_s(szName, 50, "##Vec4%d", ParamCount++);

	ImGui::Text(_ParamName.c_str());
	ImGui::SameLine();

	if (ImGui::DragFloat4(szName, *_pInOut, 0.01f, 0.f, 1.f, "%6.3f"))
	{
		if (ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			UI::ResetFocus();
		}
	}
}

void ParamUI::Param_Mat(const string& _ParamName, Matrix* _pInOut)
{
}

bool ParamUI::Param_Tex(const string& _ParamName, Ptr<CTexture>& _Tex, UI* _Inst, FUNC_1 _Func)
{
	// 키를 텍스트로 띄워준다.
	ImGui::Text(_ParamName.c_str());

	// 해당 키값으로 찾아 이미지를 띄워준다.
	if (nullptr != _Tex)
	{
		ImGui::Text(string(_Tex->GetRelativePath().begin(), _Tex->GetRelativePath().end()).c_str());
		ImGui::Image(_Tex->GetSRV().Get(), ImVec2(100.f, 100.f));
	}
	else
	{
		ImGui::Image(nullptr, ImVec2(100.f, 100.f));
	}


	// 드랍 체크
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##ContentTree"))
		{
			TreeNode* pNode = (TreeNode*)payload->Data;
			CRes* pRes = (CRes*)pNode->GetData();

			if (RES_TYPE::TEXTURE == pRes->GetResType())
			{
				_Tex = (CTexture*)pRes;
			}
		}
		ImGui::EndDragDropTarget();
	}

	// 버튼의 이름이 겹치지 안도록 만들기 위해 추가함.
	char szName[50] = "";
	sprintf_s(szName, 50, "##TexBtn%d", ParamCount++);

	ImGui::SameLine();
	if (ImGui::Button(szName, Vec2(18.f, 18.f)))
	{
		ScriptUI* pScriptUI = dynamic_cast<ScriptUI*>(_Inst);
		if (pScriptUI != nullptr)
		{
			pScriptUI->SetParamTex(&_Tex);
		}

		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
		assert(pListUI);

		// 메쉬 목록을 받아와서, ListUI 에 전달
		const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResource(RES_TYPE::TEXTURE);
		static vector<wstring> vecRes;
		vecRes.clear();

		map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.begin();
		for (; iter != mapRes.end(); ++iter)
		{
			vecRes.push_back(iter->first);
		}
		pListUI->SetItemList(vecRes);
		pListUI->AddDynamicDBClicked(_Inst, _Func);

		pListUI->Open();

		return true;
	}

	return false;
}

bool ParamUI::Param_Prefab(const string& _ParamName, Ptr<CPrefab>& _Prefab, UI* _Inst, FUNC_1 _Func)
{
	// 키를 텍스트로 띄워준다.
	ImGui::Text(_ParamName.c_str());
	
	ImGui::SameLine();

	// Prafab의 이름을 띄워준다.
	ImGui::PushItemWidth(250.f);
	char PrefabNameID[50] = "";
	sprintf_s(PrefabNameID, 50, "##PrefabName%d", ParamCount++);
	char PrefabName[50] = {};
	sprintf_s(PrefabName, 50, WStringToString(_Prefab->GetName()).c_str());
	ImGui::InputText(PrefabNameID, PrefabName, 50, ImGuiInputTextFlags_ReadOnly);
	ImGui::PopItemWidth();

	// 버튼의 이름이 겹치지 안도록 만들기 위해 추가함.
	char szName[50] = "";
	sprintf_s(szName, 50, "##PrefabBtn%d", ParamCount++);

	ImGui::SameLine();
	if (ImGui::Button(szName, Vec2(18.f, 18.f)))
	{
		ScriptUI* pScriptUI = dynamic_cast<ScriptUI*>(_Inst);
		if (pScriptUI != nullptr)
		{
			pScriptUI->SetParamPrefab(&_Prefab);
		}

		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
		assert(pListUI);

		// 메쉬 목록을 받아와서, ListUI 에 전달
		const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResource(RES_TYPE::PREFAB);
		static vector<wstring> vecRes;
		vecRes.clear();

		map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.begin();
		for (; iter != mapRes.end(); ++iter)
		{
			vecRes.push_back(iter->first);
		}
		pListUI->SetItemList(vecRes);
		pListUI->AddDynamicDBClicked(_Inst, _Func);

		pListUI->Open();

		return true;
	}

	return false;
}