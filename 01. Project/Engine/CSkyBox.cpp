#include "pch.h"
#include "CSkyBox.h"

#include "CTransform.h"

CSkyBox::CSkyBox()
	: CRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_eSkyBoxType(SKYBOX_TYPE::SPHERE)
{
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyBoxMtrl"));
	SetType(m_eSkyBoxType);
	SetSkyMesh();
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::SetTexture(Ptr<CTexture> _pTex)
{
	m_pSkyBoxTex = _pTex;
	if (m_eSkyBoxType == SKYBOX_TYPE::SPHERE)
	{
		GetCurMaterial()->SetTexParam(TEX_0, m_pSkyBoxTex);
	}
	else
	{
		GetCurMaterial()->SetTexParam(TEX_CUBE_0, m_pSkyBoxTex);
	}
}

void CSkyBox::finaltick()
{

}

void CSkyBox::render()
{
	Transform()->UpdateData();

	GetCurMaterial()->SetScalarParam(INT_0, (void*)&m_eSkyBoxType);
	GetCurMaterial()->SetScalarParam(MAT_0, (void*)&Transform()->GetWorldRotMat());

	if (m_eSkyBoxType == SKYBOX_TYPE::SPHERE)
	{
		GetCurMaterial()->SetTexParam(TEX_CUBE_0, nullptr);
		GetCurMaterial()->SetTexParam(TEX_0, m_pSkyBoxTex);
	}

	else if (m_eSkyBoxType == SKYBOX_TYPE::CUBE)
	{
		GetCurMaterial()->SetTexParam(TEX_0, nullptr);
		GetCurMaterial()->SetTexParam(TEX_CUBE_0, m_pSkyBoxTex);
	}

	GetCurMaterial()->UpdateData();

	GetMesh()->render();

	CMaterial::Clear();
}

void CSkyBox::SetSkyMesh()
{
	if (m_eSkyBoxType == SKYBOX_TYPE::SPHERE)
	{
		SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	}
	else
	{
		SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	}
}

void CSkyBox::SetType(SKYBOX_TYPE _type)
{
	m_eSkyBoxType = _type;
}

void CSkyBox::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "SKYBOX";
	_emitter << YAML::Value << YAML::BeginMap;

	_emitter << YAML::Key << "SkyBoxTexture";
	_emitter << YAML::Value << YAML::BeginMap;
	SaveResourceRef<CTexture>(m_pSkyBoxTex, _emitter);
	_emitter << YAML::EndMap;
	_emitter << YAML::Key << "SkyBoxType";
	_emitter << YAML::Value << (UINT)m_eSkyBoxType;

	_emitter << YAML::EndMap;
}

void CSkyBox::LoadFromYAML(YAML::Node& _node)
{
	SetType((SKYBOX_TYPE)(_node["SKYBOX"]["SkyBoxType"].as<UINT>()));
    SetSkyMesh();
	YAML::Node node = _node["SKYBOX"]["SkyBoxTexture"];
	LoadResourceRef<CTexture>(m_pSkyBoxTex, node);
}