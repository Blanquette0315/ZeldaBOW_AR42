#include "pch.h"
#include "ParamUI.h"

#include <Engine/CResMgr.h>

#include "imgui.h"
#include "CImGuiMgr.h"
#include "ListUI.h"
#include "TreeUI.h"

// ī��Ʈ �ʱ�ȭ�κ�
UINT ParamUI::ParamCount = 0;


void ParamUI::Param_Int(const string& _ParamName, int* _pInOut)
{
	char szName[50] = "";
	sprintf_s(szName, 50, "##Int%d", ParamCount++);

	ImGui::Text(_ParamName.c_str());
	ImGui::SameLine();
	ImGui::InputInt(szName, _pInOut);
}

void ParamUI::Param_Float(const string& _ParamName, float* _pInOut)
{
	char szName[50] = "";
	sprintf_s(szName, 50, "##Float%d", ParamCount++);

	ImGui::Text(_ParamName.c_str());
	ImGui::SameLine();

	if (ImGui::DragFloat(szName, _pInOut, 0.01f, 0.f, 1.f, "%6.3f"))
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

	float arrData[2] = { _pInOut->x, _pInOut->y };
	ImGui::InputFloat2(szName, arrData);
	_pInOut->x = arrData[0];
	_pInOut->y = arrData[1];

}

void ParamUI::Param_Vec4(const string& _ParamName, Vec4* _pInOut)
{
	char szName[50] = "";
	sprintf_s(szName, 50, "##Vec4%d", ParamCount++);

	ImGui::Text(_ParamName.c_str());
	ImGui::SameLine();
	ImGui::InputFloat4(szName, *_pInOut);
}

void ParamUI::Param_Mat(const string& _ParamName, Matrix* _pInOut)
{
}

bool ParamUI::Param_Tex(const string& _ParamName, Ptr<CTexture>& _Tex, UI* _Inst, FUNC_1 _Func)
{
	// Ű�� �ؽ�Ʈ�� ����ش�.
	ImGui::Text(_ParamName.c_str());

	// �ش� Ű������ ã�� �̹����� ����ش�.
	if (nullptr != _Tex)
	{
		ImGui::Image(_Tex->GetSRV().Get(), ImVec2(100.f, 100.f));
	}
	else
	{
		ImGui::Image(nullptr, ImVec2(100.f, 100.f));
	}


	// ��� üũ
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

	// ��ư�� �̸��� ��ġ�� �ȵ��� ����� ���� �߰���.
	char szName[50] = "";
	sprintf_s(szName, 50, "##TexBtn%d", ParamCount++);

	ImGui::SameLine();
	if (ImGui::Button(szName, Vec2(15.f, 15.f)))
	{
		ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
		assert(pListUI);

		// �޽� ����� �޾ƿͼ�, ListUI �� ����
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