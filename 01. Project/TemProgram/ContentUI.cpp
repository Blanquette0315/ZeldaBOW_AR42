#include "pch.h"
#include "ContentUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CEventMgr.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CPathMgr.h>
#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CSound.h>

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

		map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.begin();
		for (; iter != mapRes.end(); ++iter)
		{
			wstring strName = iter->first;
			m_Tree->AddItem(pResNode, string(strName.begin(), strName.end()), (DWORD_PTR)iter->second.Get());
		}
	}

	// ContentUI�� Level �߰�
	TreeNode* pLevelNode = m_Tree->AddItem(pRootNode, "All Level", 0, true);

	const map<wstring, CLevel*>& mapLevels = CLevelMgr::GetInst()->GetLevels();

	map<wstring, CLevel*>::const_iterator liter = mapLevels.begin();
	for (; liter != mapLevels.end(); ++liter)
	{
		wstring strLevelName = liter->first;
		m_Tree->AddItem(pLevelNode, string(strLevelName.begin(), strLevelName.end()), (DWORD_PTR)liter->second);
	}

}

void ContentUI::ReloadContent()
{
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
			if (nullptr == CLevelMgr::GetInst()->FindLevel(m_vecContentName[i]))
			{
				CLevel* pLoadLevel = CSaveLoadMgr::GetInst()->LoadLevel(m_vecContentName[i]);
				CLevelMgr::GetInst()->RegisterLevel(m_vecContentName[i], pLoadLevel);
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
	const map<wstring, CLevel*>& mapLevel = CLevelMgr::GetInst()->GetLevels();

	map<wstring, CLevel*>::const_iterator iter = mapLevel.begin();
	for (iter; iter != mapLevel.end(); ++iter)
	{
		wstring strRelativePath = iter->second->GetRelativePath();

		if (!filesystem::exists(strFolderPath + strRelativePath))
		{
			// ������ ���� ������ ���� ������ �ƴ϶�� �ε��� ���Ͽ��� �����Ѵ�.
			if (iter->second != CLevelMgr::GetInst()->GetCurLevel())
			{
				// �̹�Ʈ ó���� �ٲپ� �־�� �Ѵ�.
				tEvent DeRegiLevel = {};
				DeRegiLevel.eType = EVENT_TYPE::DELETE_LEVEL;
				DeRegiLevel.wParam = (DWORD_PTR)iter->second;
				
				CEventMgr::GetInst()->AddEvent(DeRegiLevel);

				MessageBox(nullptr, L"���� ���ҽ� ������", L"���ҽ� ���� Ȯ��", MB_OK);
			}
			else
			{
				// ���� ���� ��ü�� ���Ϸν�� ���� ���ٸ� �ε��� ������ ���� �ȵȴ�.
				MessageBox(nullptr, L"��� �� �� ���ҽ�/n���ҽ� ���� ����", L"���ҽ� ���� Ȯ��", MB_OK);
			}
		}
	}
}

void ContentUI::SetResourceToInspector(DWORD_PTR _res)
{
	// _res : Ŭ���� ���
	TreeNode* pSelectedNode = (TreeNode*)_res;
	Ptr<CRes> pRes = (CRes*)pSelectedNode->GetData();

	InspectorUI* Inspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");

	// ���õ� ��尡 Level�� ���
	if (dynamic_cast<CLevel*>((CEntity*)pSelectedNode->GetData()))
	{
		// InspectorUI�� Ŭ���� Level�� �˷��ش�.
		CLevel* pLevel = (CLevel*)pSelectedNode->GetData();
		Inspector->SetTargetLevel(pLevel);

	}

	// Prefab
	else if (pRes->GetResType() == RES_TYPE::PREFAB)
	{
		Ptr<CPrefab> pref = (CPrefab*)pRes.Get();
		Inspector->SetTargetObject(pref->GetProtoObj());
	}

	// ���õ� ��尡 Res�� ���
	else
	{
		CRes* pRes = (CRes*)pSelectedNode->GetData();

		// InspectorUI�� Ŭ���� Resource�� �˷��ش�.
		Inspector->SetTargetResource(pRes);
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
