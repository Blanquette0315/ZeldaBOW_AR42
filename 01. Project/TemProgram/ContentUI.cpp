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

	// Tree 설정
	m_Tree->SetDummyRoot(true);

	// Tree에 Delegate 등록
	m_Tree->AddDynamic_Selected(this, (FUNC_1)& ContentUI::SetResourceToInspector);

	// 현재 리소스 상태를 갱신
	ResetContent();
}

ContentUI::~ContentUI()
{
}

void ContentUI::update()
{
	// ResMgr의 ISChanged쪽으로 하도록 변경해보아야함.
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

	// ContentUI에 Res 추가
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

	// ContentUI에 Level 추가
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
	// Content 폴더에 있는 모든 리소스들을 검사 및 로딩
	wstring strFolderPath = CPathMgr::GetInst()->GetContentPath();
	FindContentFileName(strFolderPath);

	// 파일명의 확장자를 확인 후 알맞은 리소스가 무엇인지 알아낸다.
	for (size_t i = 0; i < m_vecContentName.size(); ++i)
	{
		RES_TYPE resType = GetResTypeByExt(m_vecContentName[i]);

		// 리소스 타입에 해당하는 리소스를 경로로부터 로딩
		// End가 반환되었다는 소리는 알 수 없는 확장자라는 의미로 넘긴다.
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

	// 로딩된 리소스가 실제 파일로 존재하는지 확인
	for (UINT i = 0; i < (UINT)RES_TYPE::END; ++i)
	{
		const map<wstring, Ptr<CRes>>& mapRes = CResMgr::GetInst()->GetResource((RES_TYPE)i);

		map<wstring, Ptr<CRes>>::const_iterator iter = mapRes.begin();
		for (iter; iter != mapRes.end(); ++iter)
		{
			// Engine Res 타입이면 확인하지 않는다.
			if (iter->second->IsEngineRes())
			{
				continue;
			}

			// 엔진 리소스가 아닌데, 경로가 없다는 것은 말이 안되기 때문에 중단을 걸어준다.
			wstring strRelativePath = iter->second->GetRelativePath();
			assert(!strRelativePath.empty());

			// 메모리에 로딩된 리소스에 해당하는 실제 파일 이 존재하지 않는 경우
			// exists함수는 입력된 경로에 파일이 실제로 존재하는지를 확인해준다.
			if (!filesystem::exists(strFolderPath + strRelativePath))
			{
				// 실제 파일이 존재하지 않는다면, ResMgr에서 빼주어서 로딩된 리소스를 제거 해주어야 한다.
				// 리소스 메니져에서 해당 리소스를 해제한다.
				// 리소스의 레퍼런스 카운트를 확인해서 1이면, ResMgr만 참조하고 있다는 뜻이므로 문제가 없다.
				// 따라서 1보다 작은 경우에만 제거를 해준다.
				// 만약 다른 곳에서 참조중인데, 삭제를 해버리면 문제가 되기 때문에 이런 방법을 사용했다.
				if (iter->second->GetRefCount() <= 1)
				{
					tEvent evn = {};
					evn.eType = EVENT_TYPE::DELETE_RES;
					evn.wParam = i;
					evn.lParam = (DWORD_PTR)(iter->second.Get());

					CEventMgr::GetInst()->AddEvent(evn);

					MessageBox(nullptr, L"원본 리소스 삭제됨", L"리소스 변경 확인", MB_OK);
				}
				else
				{
					// 해당 리소스를 참조하는 객체가 있음
					MessageBox(nullptr, L"사용 중 인 리소스/n리소스 삭제 실패", L"리소스 변경 확인", MB_OK);
				}
			}
		}
	}

	// 로딩된 레벨 파일이 실제로 있는지 확인
	const map<wstring, CLevel*>& mapLevel = CLevelMgr::GetInst()->GetLevels();

	map<wstring, CLevel*>::const_iterator iter = mapLevel.begin();
	for (iter; iter != mapLevel.end(); ++iter)
	{
		wstring strRelativePath = iter->second->GetRelativePath();

		if (!filesystem::exists(strFolderPath + strRelativePath))
		{
			// 지워진 레벨 파일이 현재 레벨이 아니라면 로딩한 파일에서 제거한다.
			if (iter->second != CLevelMgr::GetInst()->GetCurLevel())
			{
				// 이밴트 처리로 바꾸어 주어야 한다.
				tEvent DeRegiLevel = {};
				DeRegiLevel.eType = EVENT_TYPE::DELETE_LEVEL;
				DeRegiLevel.wParam = (DWORD_PTR)iter->second;
				
				CEventMgr::GetInst()->AddEvent(DeRegiLevel);

				MessageBox(nullptr, L"원본 리소스 삭제됨", L"리소스 변경 확인", MB_OK);
			}
			else
			{
				// 현재 레벨 객체가 파일로써는 지워 졌다면 로딩한 곳에서 빼면 안된다.
				MessageBox(nullptr, L"사용 중 인 리소스/n리소스 삭제 실패", L"리소스 변경 확인", MB_OK);
			}
		}
	}
}

void ContentUI::SetResourceToInspector(DWORD_PTR _res)
{
	// _res : 클릭된 노드
	TreeNode* pSelectedNode = (TreeNode*)_res;

	InspectorUI* Inspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");

	// 선택된 노드가 Level일 경우
	if (dynamic_cast<CLevel*>((CEntity*)pSelectedNode->GetData()))
	{
		// InspectorUI에 클릭된 Level을 알려준다.
		CLevel* pLevel = (CLevel*)pSelectedNode->GetData();
		Inspector->SetTargetLevel(pLevel);
	}

	// 선택된 노드가 Res일 경우
	else
	{
		CRes* pRes = (CRes*)pSelectedNode->GetData();

		// InspectorUI에 클릭된 Resource를 알려준다.
		Inspector->SetTargetResource(pRes);
	}
}

void ContentUI::FindContentFileName(const wstring& _strFolderPath)
{
	// 폴더 구조도 Tree구조이기 때문에 재귀적으로 호출하면서 모든 하위 폴더를 순회할 것이다.

	wstring strFolderPath = _strFolderPath + L"*.*";

	// 모든 파일명을 알아낸다.
	// 핸들을 사용한다는 뜻은 커널 오브젝트를 만든다는 의미와 같다.
	HANDLE hFindHandle = nullptr;

	WIN32_FIND_DATA data = {};

	// 탐색용 핸들 얻기
	// FindFirstFile는 파일을 찾을 수 있는 함수이며, 윈도우에서 제공해준다.
	// 정확히는 탐색기의 핸들을 반환해주는 함수이다.
	// 따라서 인자로 검색을 시작할 폴더와 데이터를 받을 주소를 넣어주면 된다.
	hFindHandle = FindFirstFile(strFolderPath.c_str(), &data);

	if (INVALID_HANDLE_VALUE == hFindHandle)
		return;

	// 탐색 핸들을 이용해서 모든 파일을 다 확인할 때 까지 반복
	// 만약 다음 탐색할 파일이 없다면 false를 반환해준다.
	while (FindNextFile(hFindHandle, &data))
	{
		// 폴더에서 파일을 찾을 때는 우리에게 공개되지 않은 ..폴더와 다른 폴더들도 함께 찾아지기 때문에
		// 파일만 얻기 위해서는 폴더 형태는 재귀호출해서 하위 폴더를 탐색해야 한다.
		// 예외 처리 : ..폴더 하위 탐색을 하면 계속 무한루프를 돌기 때문에 예외처리해주었다.
		if (FILE_ATTRIBUTE_DIRECTORY == data.dwFileAttributes && wcscmp(data.cFileName, L".."))
		{
			FindContentFileName(_strFolderPath + data.cFileName + L"\\");
		}
		// 폴더 타입이 아니다 == 파일 타입이다.
		else
		{
			wstring strRelative = GetRelativePath(CPathMgr::GetInst()->GetContentPath(), _strFolderPath + data.cFileName);
			m_vecContentName.push_back(strRelative);
		}
	}

	// 탐색기 사용이 끝났으면 FindClose을 반듯이 호출해 주어야 한다.
	FindClose(hFindHandle);
}

RES_TYPE ContentUI::GetResTypeByExt(wstring _filename)
{
	// path는 FileSystem 헤더 참조로 사용이 가능하다.
	// _wsplitpath_s를 사용하지 않아도 된다는 장점이 있다.
	wstring strExt = path(_filename.c_str()).extension();

	// if문으로 확장자를 검사해 분기처리해서 알맞은 RES_TYPE을 반환해준다.
	// Shader들은 우리가 하드코딩해 사용하기 때문에 제외해 주었다.
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
