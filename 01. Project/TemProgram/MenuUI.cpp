#include "pch.h"
#include "MenuUI.h"

#include <Engine/CLevelMgr.h>
#include <Engine/CEventMgr.h>
#include <Engine/CGameObject.h>
#include <Engine/CScript.h>
#include <Script/CScriptMgr.h>

#include "CImGuiMgr.h"
#include "InspectorUI.h"
#include "ContentUI.h"
#include "CSaveLoadMgr.h"



MenuUI::MenuUI()
	: UI("##MenuUI")
    , m_ePrevState(LEVEL_STATE::STOP)
{
}

MenuUI::~MenuUI()
{
}

void MenuUI::render()
{
    if (ImGui::BeginMainMenuBar())
    {
        // �̰��� �������� �ϴ� ���� �־��ָ� ���� ���̴�.
        if (ImGui::BeginMenu("File"))
        {

            if (ImGui::MenuItem("Level Save"))
            {
                // Level ������
                CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

                CSaveLoadMgr::GetInst()->SaveLevel(pCurLevel);
            }

            ImGui::EndMenu();
        }

        // ���� ����Ű�� �����صδ� �۾��� ���� �ʾƼ� ������ �ƹ��� ���۵� ���� ���� �Ӵ���,
        // ����Ű�� ������ �ƹ��� ������ ���� �ʴ´�.
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z"))
            {

            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }

        // ������Ʈ �߰������ ����� ���̴�.
        if (ImGui::BeginMenu("Component"))
        {
            if (ImGui::BeginMenu("AddScript"))
            {
                vector<wstring> vecScriptName;
                CScriptMgr::GetScriptInfo(vecScriptName);

                for (size_t i = 0; i < vecScriptName.size(); ++i)
                {
                    string strScriptName = WStringToString(vecScriptName[i]);
                    if (ImGui::MenuItem(strScriptName.c_str()))
                    {
                        InspectorUI* pInspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
                        CGameObject* pTargetObject = pInspector->GetTargetObject();
                        if (nullptr != pTargetObject)
                        {
                            pTargetObject->AddComponent(CScriptMgr::GetScript(vecScriptName[i]));
                            pInspector->SetTargetObject(pTargetObject);
                        }
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Level"))
        {
            LEVEL_STATE State = CLevelMgr::GetInst()->GetLevelState();

            bool PlayEnable = true;
            bool PauseEnable = true;
            bool StopEnable = true;

            if (LEVEL_STATE::PLAY != State) PlayEnable = true; else PlayEnable = false;
            if (LEVEL_STATE::PLAY == State) PauseEnable = true; else  PauseEnable = false;
            if (LEVEL_STATE::STOP != State) StopEnable = true; else StopEnable = false;

            if (ImGui::MenuItem("Play", nullptr, nullptr, PlayEnable))
            {
                tEvent evn = {};
                evn.eType = EVENT_TYPE::CHANGE_LEVEL_STATE;
                evn.wParam = (DWORD_PTR)LEVEL_STATE::PLAY;
                CEventMgr::GetInst()->AddEvent(evn);

                if (m_ePrevState == LEVEL_STATE::STOP)
                {
                    CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
                    m_strOriCurLevelPath = pCurLevel->GetRelativePath();
                    CSaveLoadMgr::SaveLevel(pCurLevel, L"tmpSave");
                }

                m_ePrevState = LEVEL_STATE::PLAY;
            }

            if (ImGui::MenuItem("Pause", nullptr, nullptr, PauseEnable))
            {
                tEvent evn = {};
                evn.eType = EVENT_TYPE::CHANGE_LEVEL_STATE;
                evn.wParam = (DWORD_PTR)LEVEL_STATE::PAUSE;
                CEventMgr::GetInst()->AddEvent(evn);
            }

            if (ImGui::MenuItem("Stop", nullptr, nullptr, StopEnable))
            {
                CLevel* pChangeLevel = CSaveLoadMgr::GetInst()->LoadLevel(L"level\\tmpSave.lv");
                pChangeLevel->SetRelativePath(m_strOriCurLevelPath.c_str());
                ChangeLevel(pChangeLevel);
                m_ePrevState = LEVEL_STATE::STOP;
                InspectorUI* Inspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
                Inspector->SetTargetLevel(nullptr);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("DebugDraw"))
        {
            // Editor���� ���ο� ���ҽ��� ����� �ش�.
            if (ImGui::MenuItem("All DebugDraw OFF"))
            {
                vector<CGameObject*> pvecObj = CLevelMgr::GetInst()->GetCurLevel()->GetGameObjects();

                for (size_t i = 0; i < pvecObj.size(); ++i)
                {
                    for (UINT j = 0; j < (UINT)COMPONENT_TYPE::END; ++j)
                    {
                        CComponent* pTargetCompo = pvecObj[i]->GetComponent((COMPONENT_TYPE)j);
                        if (nullptr != pTargetCompo)
                            pTargetCompo->ShowDebugDraw(false);
                    }
                }
            }

            // Editor���� ���ο� ���ҽ��� ����� �ش�.
            if (ImGui::MenuItem("All DebugDraw ON"))
            {
                vector<CGameObject*> pvecObj = CLevelMgr::GetInst()->GetCurLevel()->GetGameObjects();

                for (size_t i = 0; i < pvecObj.size(); ++i)
                {
                    for (UINT j = 0; j < (UINT)COMPONENT_TYPE::END; ++j)
                    {
                        CComponent* pTargetCompo = pvecObj[i]->GetComponent((COMPONENT_TYPE)j);
                        if (nullptr != pTargetCompo)
                            pTargetCompo->ShowDebugDraw(true);
                    }
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tool"))
        {
            if (ImGui::MenuItem("NavMesh-Export"))
            {
                CImGuiMgr::GetInst()->FindUI("ExportTool")->Open();
            }

            if (ImGui::MenuItem("AddAnimation"))
            {
                CImGuiMgr::GetInst()->FindUI("AddAnimationTool")->Open();
            }

            if (ImGui::MenuItem("InjectAnimation"))
            {
                CImGuiMgr::GetInst()->FindUI("AnimInjectionTool")->Open();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void MenuUI::render_update()
{
}

