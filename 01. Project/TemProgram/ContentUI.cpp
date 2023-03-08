#include "pch.h"
#include "ContentUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CEventMgr.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CPathMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CSound.h>
#include <Engine/CGameObject.h>
#include <Engine/CPrefab.h>

#include "CImGuiMgr.h"
#include "InspectorUI.h"
#include "TreeUI.h"
#include "CSaveLoadMgr.h"

ContentUI::ContentUI()
	: UI("Content")
	, m_Tree(nullptr)
{
	m_Tree = new TreeUI("##ContentTree");
	AddChild(m_Tree);

	// Tree ����
	m_Tree->SetDummyRoot(true);

	// Tree�� Delegate ���
	m_Tree->AddDynamic_Selected(this, (FUNC_1)& ContentUI::SetResourceToInspector);

	// ���� ���ҽ� ���¸� ����
	ResetContent();
}

ContentUI::~ContentUI()
{

}

void ContentUI::update()
{
	// ResMgr�� ISChanged������ �ϵ��� �����غ��ƾ���.
	if (CEventMgr::GetInst()->IsLevelChanged() || CResMgr::GetInst()->IsChanged())
	{
		ResetContent();
	}

	if (KEY_PRESSED(KEY::LCTRL) && KEY_TAP(KEY::S))
	{
		TreeNode* pSelectedNode = m_Tree->GetSelectedNode();
		if (nullptr != pSelectedNode)
		{
			RES_TYPE type = ((CRes*)pSelectedNode->GetData())->GetResType();

			wstring strRelativePath;

			if (RES_TYPE::MATERIAL == type)
			{
				Ptr<CMaterial> pRes = (CMaterial*)pSelectedNode->GetData();

				strRelativePath = L"material\\";
				strRelativePath += pRes->GetKey();
				strRelativePath += L".mtrl";
				pRes->Save(strRelativePath);
			}
		}
	}
	UI::update();
}

void ContentUI::render_update()
{

}

void ContentUI::ResetContent()
{
	m_Tree->Clear();

	// ContentUI�� Res �߰�
	TreeNode* pRootNode = m_Tree->AddItem(nullptr, "Content", 0);

	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i) 
	{
		TreeNode* pResNode = m_Tree->AddItem(pRootNode, ToString((RES_TYPE)i), 0, true);

		const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResource((RES_TYPE)i);

		// Prefab
		if( i == (UINT)RES_TYPE::PREFAB)
		{
			map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.begin();
			for (; iter != mapRes.end(); ++iter)
			{
				wstring strName = iter->first;
				
				iter->second; // Prefab
				Ptr<CPrefab> pPref = (CPrefab*)iter->second.Get();
				CGameObject* pParentObj = pPref->GetProtoObj();
				AddGameObjectToTree(pResNode, pParentObj);
			}
		}

		// Other Res
		else
		{
			map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.begin();
			for (; iter != mapRes.end(); ++iter)
			{
				wstring strName = iter->first;
				m_Tree->AddItem(pResNode, string(strName.begin(), strName.end()), (DWORD_PTR)iter->second.Get());
			}
		}
	}

	// ContentUI�� Level �߰�
	TreeNode* pLevelNode = m_Tree->AddItem(pRootNode, "All Level", 0, true);

	for (size_t i = 0; i < m_vecLevelName.size(); ++i)
	{
		wstring strLevelName = m_vecLevelName[i];
		size_t idx = strLevelName.find(L"\\");
		strLevelName.erase(0, idx + 1);
		m_Tree->AddItem(pLevelNode, string(strLevelName.begin(), strLevelName.end()), (DWORD_PTR)&m_vecLevelName[i], false, true);
	}
}

void ContentUI::ReloadContent()
{
	m_vecContentName.clear();
	m_vecLevelName.clear();

	// A problem arises when Reload Content() is called when the InspectorUI target is a level.
	// This is because the address of the const wstring ("m_vecLevelName") is lost.
	// So, use Function SetTargetLevel(nullptr)
	InspectorUI* Inspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
	Inspector->SetTargetLevel(nullptr);

	// Content ������ �ִ� ��� ���ҽ����� �˻� �� �ε�
	wstring strFolderPath = CPathMgr::GetInst()->GetContentPath();
	FindContentFileName(strFolderPath);

	// ���ϸ��� Ȯ���ڸ� Ȯ�� �� �˸��� ���ҽ��� �������� �˾Ƴ���.
	for (size_t i = 0; i < m_vecContentName.size(); ++i)
	{
		RES_TYPE resType = GetResTypeByExt(m_vecContentName[i]);

		// ���ҽ� Ÿ�Կ� �ش��ϴ� ���ҽ��� ��ηκ��� �ε�
		// End�� ��ȯ�Ǿ��ٴ� �Ҹ��� �� �� ���� Ȯ���ڶ�� �ǹ̷� �ѱ��.
		if (RES_TYPE::END == resType)
			continue;

		switch (resType)
		{
		case RES_TYPE::PREFAB:
			CResMgr::GetInst()->Load<CPrefab>(m_vecContentName[i], m_vecContentName[i]);
			break;

		case RES_TYPE::MESHDATA:

			break;
		case RES_TYPE::MATERIAL:
			CResMgr::GetInst()->Load<CMaterial>(m_vecContentName[i], m_vecContentName[i]);
			break;

		case RES_TYPE::MESH:
			break;
		case RES_TYPE::TEXTURE:
			CResMgr::GetInst()->Load<CTexture>(m_vecContentName[i], m_vecContentName[i]);
			break;
		case RES_TYPE::SOUND:
			CResMgr::GetInst()->Load<CSound>(m_vecContentName[i], m_vecContentName[i]);
			break;

		case RES_TYPE::LEVEL:
		{
			// vector�� ��� �߰��Ǵ��� Ȯ���غ��� ��.
			m_vecLevelName.push_back(m_vecContentName[i]);
			ResetContent();
		}
			break;
		}
	}

	// �ε��� ���ҽ��� ���� ���Ϸ� �����ϴ��� Ȯ��
	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResource((RES_TYPE)i);

		map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.begin();
		for (iter; iter != mapRes.end(); ++iter)
		{
			// Engine Res Ÿ���̸� Ȯ������ �ʴ´�.
			if (iter->second->IsEngineRes())
			{
				continue;
			}

			// ���� ���ҽ��� �ƴѵ�, ��ΰ� ���ٴ� ���� ���� �ȵǱ� ������ �ߴ��� �ɾ��ش�.
			wstring strRelativePath = iter->second->GetRelativePath();
			assert(!strRelativePath.empty());

			// �޸𸮿� �ε��� ���ҽ��� �ش��ϴ� ���� ���� �� �������� �ʴ� ���
			// exists�Լ��� �Էµ� ��ο� ������ ������ �����ϴ����� Ȯ�����ش�.
			if (!filesystem::exists(strFolderPath + strRelativePath))
			{
				// ���� ������ �������� �ʴ´ٸ�, ResMgr���� ���־ �ε��� ���ҽ��� ���� ���־�� �Ѵ�.
				// ���ҽ� �޴������� �ش� ���ҽ��� �����Ѵ�.
				// ���ҽ��� ���۷��� ī��Ʈ�� Ȯ���ؼ� 1�̸�, ResMgr�� �����ϰ� �ִٴ� ���̹Ƿ� ������ ����.
				// ���� 1���� ���� ��쿡�� ���Ÿ� ���ش�.
				// ���� �ٸ� ������ �������ε�, ������ �ع����� ������ �Ǳ� ������ �̷� ����� ����ߴ�.
				if (iter->second->GetRefCount() <= 1)
				{
					tEvent evn = {};
					evn.eType = EVENT_TYPE::DELETE_RES;
					evn.wParam = i;
					evn.lParam = (DWORD_PTR)(iter->second.Get());

					CEventMgr::GetInst()->AddEvent(evn);

					MessageBox(nullptr, L"���� ���ҽ� ������", L"���ҽ� ���� Ȯ��", MB_OK);
				}
				else
				{
					// �ش� ���ҽ��� �����ϴ� ��ü�� ����
					MessageBox(nullptr, L"��� �� �� ���ҽ�/n���ҽ� ���� ����", L"���ҽ� ���� Ȯ��", MB_OK);
				}
			}
		}
	}

	// �ε��� ���� ������ ������ �ִ��� Ȯ��
	for (size_t i = 0; i < m_vecLevelName.size(); ++i)
	{
		wstring strRelativePath = m_vecLevelName[i];

		if (!filesystem::exists(strFolderPath + strRelativePath))
		{
			// Erase Level Relative Path to m_vecLevelName
			m_vecLevelName.erase(m_vecLevelName.begin() + i);
			ResetContent();
			MessageBox(nullptr, L"���� ���� ������", L"���ҽ� ���� Ȯ��", MB_OK);
		}
	}
}

bool ContentUI::Is_ValidLvPath(const wstring& _strLvRelativePath)
{
	for (size_t i = 0; i < m_vecLevelName.size(); ++i)
	{
		// If any of the files in the folder have the same file, FAILED!
		if (_strLvRelativePath == m_vecLevelName[i])
		{
			return false;
		}
	}
	return true;
}

void ContentUI::SetResourceToInspector(DWORD_PTR _res)
{
	// _res : Selected Node
	TreeNode* pSelectedNode = (TreeNode*)_res;

	InspectorUI* Inspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");

	// if Selected Node is LevelNode

	if (pSelectedNode->IsLevelData())
	{
		// TargetLevel's RelativePath impart to InspectorUI
		Inspector->SetTargetLevel((wstring*)pSelectedNode->GetData());
	}
	else
	{
		if (dynamic_cast<CGameObject*>((CEntity*)pSelectedNode->GetData()))
		{
			// Prefab child obj
			CGameObject* pPrefabObject = (CGameObject*)pSelectedNode->GetData();
			Inspector->SetTargetObject(pPrefabObject);
		}
		//else if (dynamic_cast<CPrefab*>((CEntity*)pSelectedNode->GetData()))
		//{
		//	Ptr<CPrefab> pPrefabObject = (CPrefab*)pSelectedNode->GetData();
		//	CGameObject* pProtoObject = pPrefabObject->GetProtoObj();
		//	Inspector->SetTargetPrefObject(pProtoObject);
		//}
		else if (dynamic_cast<CRes*>((CEntity*)pSelectedNode->GetData()))
		{
			Ptr<CRes> pRes = (CRes*)pSelectedNode->GetData();
			// InspectorUI�� Ŭ���� Resource�� �˷��ش�.
			Inspector->SetTargetResource(pRes.Get());
		}
	}

}

void ContentUI::FindContentFileName(const wstring& _strFolderPath)
{
	// ���� ������ Tree�����̱� ������ ��������� ȣ���ϸ鼭 ��� ���� ������ ��ȸ�� ���̴�.

	wstring strFolderPath = _strFolderPath + L"*.*";

	// ��� ���ϸ��� �˾Ƴ���.
	// �ڵ��� ����Ѵٴ� ���� Ŀ�� ������Ʈ�� ����ٴ� �ǹ̿� ����.
	HANDLE hFindHandle = nullptr;

	WIN32_FIND_DATA data = {};

	// Ž���� �ڵ� ���
	// FindFirstFile�� ������ ã�� �� �ִ� �Լ��̸�, �����쿡�� �������ش�.
	// ��Ȯ���� Ž������ �ڵ��� ��ȯ���ִ� �Լ��̴�.
	// ���� ���ڷ� �˻��� ������ ������ �����͸� ���� �ּҸ� �־��ָ� �ȴ�.
	hFindHandle = FindFirstFile(strFolderPath.c_str(), &data);

	if (INVALID_HANDLE_VALUE == hFindHandle)
		return;

	// Ž�� �ڵ��� �̿��ؼ� ��� ������ �� Ȯ���� �� ���� �ݺ�
	// ���� ���� Ž���� ������ ���ٸ� false�� ��ȯ���ش�.
	while (FindNextFile(hFindHandle, &data))
	{
		// �������� ������ ã�� ���� �츮���� �������� ���� ..������ �ٸ� �����鵵 �Բ� ã������ ������
		// ���ϸ� ��� ���ؼ��� ���� ���´� ���ȣ���ؼ� ���� ������ Ž���ؾ� �Ѵ�.
		// ���� ó�� : ..���� ���� Ž���� �ϸ� ��� ���ѷ����� ���� ������ ����ó�����־���.
		if (FILE_ATTRIBUTE_DIRECTORY == data.dwFileAttributes && wcscmp(data.cFileName, L".."))
		{
			FindContentFileName(_strFolderPath + data.cFileName + L"\\");
		}
		// ���� Ÿ���� �ƴϴ� == ���� Ÿ���̴�.
		else
		{
			wstring strRelative = GetRelativePath(CPathMgr::GetInst()->GetContentPath(), _strFolderPath + data.cFileName);
			m_vecContentName.push_back(strRelative);
		}
	}

	// Ž���� ����� �������� FindClose�� �ݵ��� ȣ���� �־�� �Ѵ�.
	FindClose(hFindHandle);
}

RES_TYPE ContentUI::GetResTypeByExt(wstring _filename)
{
	// path�� FileSystem ��� ������ ����� �����ϴ�.
	// _wsplitpath_s�� ������� �ʾƵ� �ȴٴ� ������ �ִ�.
	wstring strExt = path(_filename.c_str()).extension();

	// if������ Ȯ���ڸ� �˻��� �б�ó���ؼ� �˸��� RES_TYPE�� ��ȯ���ش�.
	// Shader���� �츮�� �ϵ��ڵ��� ����ϱ� ������ ������ �־���.
	if (strExt == L".pref")
		return RES_TYPE::PREFAB;
	else if (strExt == L".mdat")
		return RES_TYPE::MESHDATA;
	else if (strExt == L".mtrl")
		return RES_TYPE::MATERIAL;
	else if (strExt == L".mesh")
		return RES_TYPE::MESH;
	else if (strExt == L".dds" || strExt == L".DDS" || strExt == L".tga" || strExt == L".TGA"
		|| strExt == L".png" || strExt == L".PNG" || strExt == L".jpg" || strExt == L".JPG" || strExt == L".jepg" || strExt == L".JEPG"
		|| strExt == L".bmp" || strExt == L".BMP")
		return RES_TYPE::TEXTURE;
	else if (strExt == L".mp3" || strExt == L".MP3" || strExt == L".wav" || strExt == L".WAV" || strExt == L".ogg" || strExt == L".OGG")
		return RES_TYPE::SOUND;

	else if (strExt == L".lv")
		return RES_TYPE::LEVEL;
	else
		return RES_TYPE::END;
}

void ContentUI::AddGameObjectToTree(TreeNode* _ParentNode, CGameObject* _Object)
{
	string strObjectName = string(_Object->GetName().begin(), _Object->GetName().end());
	TreeNode* pCurNode = m_Tree->AddItem(_ParentNode, strObjectName.c_str(), (DWORD_PTR)_Object);

	// �θ� ������Ʈ�� �־ �ش� ������Ʈ�� �ڽı��� ����Լ��� ���� ��带 �߰����ش�.
	const vector<CGameObject*>& vecChild = _Object->GetChildObject();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		AddGameObjectToTree(pCurNode, vecChild[i]);
	}
}

void ContentUI::AddChildObject(DWORD_PTR _ChildObject, DWORD_PTR _ParentObject)
{
	TreeNode* pChildNode = (TreeNode*)_ChildObject;
	TreeNode* pParentNode = (TreeNode*)_ParentObject;

	tEvent evt = {};
	evt.eType = EVENT_TYPE::ADD_CHILD;
	evt.wParam = pChildNode->GetData();
	evt.lParam = pParentNode->GetData();
	CEventMgr::GetInst()->AddEvent(evt);
}
