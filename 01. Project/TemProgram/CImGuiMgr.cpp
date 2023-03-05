#include "pch.h"
#include "CImGuiMgr.h"

#include <Engine/CDevice.h>

#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "UI.h"
#include "ParamUI.h"


#include "InspectorUI.h"
#include "ContentUI.h"
#include "OutlinerUI.h"
#include "MenuUI.h"

#include "ListUI.h"
#include "TextureUI.h"
#include "TileMapTableUI.h"
#include "CurAnimEditor.h"
#include "LayerCollisionUI.h"


CImGuiMgr::CImGuiMgr()
    : m_NotifyHandle(nullptr)
{
}

CImGuiMgr::~CImGuiMgr()
{
    Safe_Del_Map(m_mapUI);

    clear();

    // 이밴트 핸들 제거
    FindCloseChangeNotification(m_NotifyHandle);
}

void CImGuiMgr::init(HWND _hWnd)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(_hWnd);
    ImGui_ImplDX11_Init(DEVICE, CONTEXT);

    // UI 생성
    CreateUI();

    // 파일 변경감지 핸들 등록
    m_NotifyHandle = FindFirstChangeNotification(CPathMgr::GetInst()->GetContentPath(), true, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);

}

void CImGuiMgr::progress()
{
    // 알림 확인
    ObserveContent();
#ifdef _DEBUG

    // ImGui가 그려지기 위해 필요한 3가지 함수들
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 매번 0으로 초기화해서 무한히 증가하는 것을 방지.
    // 뿐만아니라 계속 아이디가 바뀌면 버튼이 동작하지 않는다.
    ParamUI::ParamCount = 0;

    // 이부분에 우리가 만들 창들을 입력하면 된다.
    // 이제 구조가 바뀌어서 Create할 때 만들어두고 update, render쪽에서 해준다.
    {
        // 이것은 데모 윈도우를 띄우기 위한 코드다.
        bool bTrue = true;
        ImGui::ShowDemoWindow(&bTrue);

        // 모든 UI의 업데이트를 돌려주고, render를 돌려준다.
        map<string, UI*>::iterator iter = m_mapUI.begin();
        for (; iter != m_mapUI.end(); ++iter)
        {
            iter->second->update();
        }
        
        iter = m_mapUI.begin();
        for (; iter != m_mapUI.end(); ++iter)
        {
            iter->second->render();
        }
    }


    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
#else
#endif
}

void CImGuiMgr::ObserveContent()
{
    // 시간을 0초로 넣어주었기 때문에 틱마다 수행하고 바로나온다.
    // 대기 시간이 길어질 수록 Window의 Message처럼 계속 함수내에서 반복 수행을 해서 프로그램이 진행하지 않는다.
    DWORD dwWateState = WaitForSingleObject(m_NotifyHandle, 0);

    if (dwWateState == WAIT_OBJECT_0)
    {
        ContentUI* pContentUI = (ContentUI*)FindUI("Content");
        pContentUI->ReloadContent();

        FindNextChangeNotification(m_NotifyHandle);
    }
}

void CImGuiMgr::clear()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}




void CImGuiMgr::CreateUI()
{
    UI* pUI = nullptr;

    pUI = new InspectorUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new OutlinerUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new ContentUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));
    ((ContentUI*)pUI)->ReloadContent();
    ((ContentUI*)pUI)->ResetContent();

    pUI = new MenuUI;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    pUI = new CurAnimEditor;
    m_mapUI.insert(make_pair(pUI->GetName(), pUI));

    ListUI* pList = new ListUI;
    pList->SetModal(true);
    m_mapUI.insert(make_pair(pList->GetName(), pList));

    TextureUI* pTextureUI = new TextureUI;
    m_mapUI.insert(make_pair(pTextureUI->GetName(), pTextureUI));

    TileMapTableUI* pTileMapTableUI = new TileMapTableUI("TempTileTable");
    m_mapUI.insert(make_pair(pTileMapTableUI->GetName(), pTileMapTableUI));

    LayerCollisionUI* pLayerCollisionUI = new LayerCollisionUI("LayerCollisionUI");
    m_mapUI.insert(make_pair(pLayerCollisionUI->GetName(), pLayerCollisionUI));
}


void CImGuiMgr::RegisterUI(UI* _UI)
{
    UI* tempUI = FindUI(_UI->GetName());
    assert(!tempUI);

    m_mapUI.insert(make_pair(_UI->GetName(), _UI));
}

UI* CImGuiMgr::FindUI(const string& _name)
{
    map<string, UI*>::iterator iter = m_mapUI.find(_name);

    if (iter == m_mapUI.end())
    {
        return nullptr;
    }

    return iter->second;
}