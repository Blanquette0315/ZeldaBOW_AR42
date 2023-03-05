#include "pch.h"
#include "ParticleSystemUI.h"

#include <Engine/CResMgr.h>
#include <Engine/CParticleSystem.h>

#include "CImGuiMgr.h"
#include "ListUI.h"

ParticleSystemUI::ParticleSystemUI()
	: ComponentUI("Particle", COMPONENT_TYPE::PARTICLESYSTEM)
{
}

ParticleSystemUI::~ParticleSystemUI()
{
}

void ParticleSystemUI::update()
{
	if (nullptr != GetTarget())
	{
        m_iMaxCount = GetTarget()->ParticleSystem()->GetMaxCount();
        m_iAliveCount = GetTarget()->ParticleSystem()->GetAliveCount();

        m_vStartScale = GetTarget()->ParticleSystem()->GetStartScale();
        m_vEndScale = GetTarget()->ParticleSystem()->GetEndScale();

        m_vStartColor = GetTarget()->ParticleSystem()->GetStartColor();
        m_vEndColor = GetTarget()->ParticleSystem()->GetEndColor();

        m_vMinMaxSpeed = GetTarget()->ParticleSystem()->GetMinMaxSpeed();
        m_vMinMaxLifeTime = GetTarget()->ParticleSystem()->GetMinMaxLifeTime();

        m_fSpawnRange = GetTarget()->ParticleSystem()->GetSpawnRange();

        m_Frequency = GetTarget()->ParticleSystem()->GetFrequency();
        m_WorldSpawn = GetTarget()->ParticleSystem()->GetWorldSpawn();

        m_UpdateCS = GetTarget()->ParticleSystem()->GetCS();

        m_Material = GetTarget()->ParticleSystem()->GetCurMaterial();
	}

    ComponentUI::update();
}

void ParticleSystemUI::render_update()
{
    ComponentUI::render_update();

    // 키 값 가져오기.
    string MtrlName;
    if (nullptr != m_Material)
    {
        MtrlName = string(m_Material->GetKey().begin(), m_Material->GetKey().end());
    }

    // 파티클 재질 정하기
    ImGui::Text("Material    ");	ImGui::SameLine();
    ImGui::PushItemWidth(200.f);
    ImGui::InputText("##Mtrl", (char*)MtrlName.data(), MtrlName.length(), ImGuiInputTextFlags_ReadOnly);

    ImGui::SameLine();
    if (ImGui::Button("##PartcleMtrlBtn", Vec2(15.f, 15.f)))
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
        pListUI->AddDynamicDBClicked(this, (FUNC_1)&ParticleSystemUI::SetMaterial);

        pListUI->Open();
    }

    // 컴퓨트 셰이더 키 값 가져오기.
    string ComShaderKey;
    if (nullptr != m_UpdateCS)
    {
        ComShaderKey = string(m_UpdateCS->GetKey().begin(), m_UpdateCS->GetKey().end());
    }

    // 컴퓨트 셰이더 정하기
    ImGui::Text("ComShader   ");	ImGui::SameLine();
    ImGui::PushItemWidth(200.f);
    ImGui::InputText("##ComShader", (char*)ComShaderKey.data(), ComShaderKey.length(), ImGuiInputTextFlags_ReadOnly);

    ImGui::SameLine();
    if (ImGui::Button("##PartcleComShaderBtn", Vec2(15.f, 15.f)))
    {
        ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
        assert(pListUI);

        // 컴퓨트 셰이더 목록을 받아온다.
        const map<wstring, Ptr<CRes>> MapRes = CResMgr::GetInst()->GetResource(RES_TYPE::COMPUTE_SHADER);
        static vector<wstring> vecRes;
        vecRes.clear();

        map<wstring, Ptr<CRes>>::const_iterator iter = MapRes.begin();
        for (; iter != MapRes.end(); ++iter)
        {
            vecRes.push_back(iter->first);
        }
        pListUI->SetItemList(vecRes);
        pListUI->AddDynamicDBClicked(this, (FUNC_1)&ParticleSystemUI::SetComputeShader);

        pListUI->Open();
    }

    // 텍스쳐 정하기
    ImGui::Text("Select Texture"); ImGui::SameLine();
    if (ImGui::Button("##PTC_TexListBox", Vec2(18.f, 18.f)))
    {
        ListUI* pListUI = dynamic_cast<ListUI*>(CImGuiMgr::GetInst()->FindUI("ListUI"));
        assert(pListUI);

        const map<wstring, Ptr<CRes>> MapRes = CResMgr::GetInst()->GetResource(RES_TYPE::TEXTURE);
        static vector<wstring> vecRes;
        vecRes.clear();

        map<wstring, Ptr<CRes>>::const_iterator iter = MapRes.begin();
        for (; iter != MapRes.end(); ++iter)
        {
            vecRes.push_back(iter->first);
        }
        pListUI->SetItemList(vecRes);
        pListUI->AddDynamicDBClicked(this, (FUNC_1)&ParticleSystemUI::SetTexture);

        pListUI->Open();
    }

    ImGui::Text("MaxCount    "); ImGui::SameLine(); ImGui::InputInt("##PTC_MaxCount", &m_iMaxCount, 1, 10);
    ImGui::Text("AliveCount  "); ImGui::SameLine(); ImGui::InputInt("##PTC_AliveCount", &m_iAliveCount, 1, 10);
    ImGui::Text("Start Scale "); ImGui::SameLine(); ImGui::InputFloat3("##PTC_StartScale", m_vStartScale);
    ImGui::Text("End Scale   "); ImGui::SameLine(); ImGui::InputFloat3("##PTC_EndScale", m_vEndScale);

    ImGui::Text("Start Color "); ImGui::SameLine(); ImGui::InputFloat3("##PTC_StartColor", m_vStartColor);
    ImGui::SameLine();
    ImGui::ColorEdit4("##PTC_StartScale2", m_vStartColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_PickerHueWheel);

    ImGui::Text("End Color   "); ImGui::SameLine(); ImGui::InputFloat3("##PTC_EndColor", m_vEndColor);
    ImGui::SameLine();
    ImGui::ColorEdit4("##PTC_EndColor2", m_vEndColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_PickerHueWheel);

    ImGui::Text("Min Speed   "); ImGui::SameLine(); ImGui::InputFloat("##PTC_MinSpeed", &m_vMinMaxSpeed.x, 1.f, 10.f);
    ImGui::Text("Max Speed   "); ImGui::SameLine(); ImGui::InputFloat("##PTC_MaxSpeed", &m_vMinMaxSpeed.y, 1.f, 10.f);
    ImGui::Text("Min LifeTime"); ImGui::SameLine(); ImGui::InputFloat("##PTC_MinLifeTime", &m_vMinMaxLifeTime.x, 0.1f, 1.f);
    ImGui::Text("Max LifeTime"); ImGui::SameLine(); ImGui::InputFloat("##PTC_MaxLifeTime", &m_vMinMaxLifeTime.y, 0.1f, 1.f);
    ImGui::Text("Spawn Range "); ImGui::SameLine(); ImGui::InputFloat("##PTC_SpawnRange", &m_fSpawnRange, 1.0f, 10.f);
    ImGui::Text("Frequency   "); ImGui::SameLine(); ImGui::InputFloat("##PTC_Frequency", &m_Frequency, 1.0f, 10.f);
    ImGui::Text("World Spawn "); ImGui::SameLine(); ImGui::Checkbox("##PTC_WorldSpawn", &m_WorldSpawn);

    if (GetTarget())
    {
        GetTarget()->ParticleSystem()->SetMaxCount(m_iMaxCount);
        GetTarget()->ParticleSystem()->Set_SE_Color(m_vStartColor, m_vEndColor);
        GetTarget()->ParticleSystem()->Set_SE_Scale(m_vStartScale, m_vEndScale);
        GetTarget()->ParticleSystem()->SetMinMaxSpeed(m_vMinMaxSpeed);
        GetTarget()->ParticleSystem()->SetMinMaxLifeTime(m_vMinMaxLifeTime);
        GetTarget()->ParticleSystem()->SetSpawnRange(m_fSpawnRange);
        GetTarget()->ParticleSystem()->SetFrequency(m_Frequency);
        GetTarget()->ParticleSystem()->SetWorldSpawn(m_WorldSpawn);
    }
}

void ParticleSystemUI::SetTexture(DWORD_PTR _strTextureKey)
{
    string strKey = (char*)_strTextureKey;
    wstring wstrKey = wstring(strKey.begin(), strKey.end());

    Ptr<CTexture> pTexture = CResMgr::GetInst()->FindRes<CTexture>(wstrKey);
    assert(nullptr != pTexture);

    GetTarget()->ParticleSystem()->GetSharedMaterial()->SetTexParam(TEX_0, pTexture);
}

void ParticleSystemUI::SetMaterial(DWORD_PTR _strMaterialKey)
{
    string strKey = (char*)_strMaterialKey;
    wstring wstrKey = wstring(strKey.begin(), strKey.end());

    Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(wstrKey);
    assert(nullptr != pMtrl);

    GetTarget()->ParticleSystem()->SetSharedMaterial(pMtrl);
}

void ParticleSystemUI::SetComputeShader(DWORD_PTR _strMaterialKey)
{
    string strKey = (char*)_strMaterialKey;
    wstring wstrKey = wstring(strKey.begin(), strKey.end());

    Ptr<CComputeShader> pComShader = CResMgr::GetInst()->FindRes<CComputeShader>(wstrKey);
    assert(nullptr != pComShader);

    GetTarget()->ParticleSystem()->SetCS((CParticleUpdateShader*)pComShader.Get());
}
