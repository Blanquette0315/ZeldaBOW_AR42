#include "pch.h"
#include "CDecal.h"

#include "CRenderMgr.h"
#include "CTransform.h"

CDecal::CDecal()
	: CRenderComponent(COMPONENT_TYPE::DECAL)
	, m_bLighting(false)
{
	// ��Į���� ����� �Žô� �Ϲ������� ť�� �����̱� ������ �����ڿ��� �������ش�.
	// Shader�� ��� Deferred��, forward�Ŀ� ���� �޶����� ������ �Լ��� �з��� ���̴�.
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	SetRenderType(m_bLighting);
}

CDecal::~CDecal()
{
}

void CDecal::SetRenderType(bool _bSet)
{
	m_bLighting = _bSet;

	// true�� ��� Deferred�� �������ϰڴٴ� �ǹ��̴�.
	if (m_bLighting)
	{
		SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Deferred_DecalMtrl"));
	}

	// false�� ��� Forward�� �������ϰڴٴ� �ǹ��̴�.
	else
	{
		SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DecalMtrl"));
	}
}

void CDecal::finaltick()
{
	
}

void CDecal::render()
{
	// Decal Debug Draw
	if (Is_ShowDebugDraw())
	{
		DebugDrawCube(Vec4(0.2f, 0.8f, 0.2f, 1.f), Transform()->GetWorldPos(), Transform()->GetWorldScale(), Transform()->GetRelativeRotation());
	}

	// ��ġ ��� ������Ʈ
	Transform()->UpdateData();

	// ���׸��� ������Ʈ
	CRenderMgr::GetInst()->CopyPositionTarget();
	GetCurMaterial()->SetTexParam(TEX_1, m_pDecalTex);
	GetCurMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PTCopyTex"));
	GetCurMaterial()->UpdateData();

	// ������
	GetMesh()->render();

	CMaterial::Clear();
}

void CDecal::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "DECAL";
	_emitter << YAML::Value << YAML::BeginMap;

	_emitter << YAML::Key << "DecalTexture";
	_emitter << YAML::Value << YAML::BeginMap;
	SaveResourceRef<CTexture>(m_pDecalTex, _emitter);
	_emitter << YAML::EndMap;
	_emitter << YAML::Key << "LightingMethod";
	_emitter << YAML::Value << m_bLighting;

	_emitter << YAML::EndMap;
}

void CDecal::LoadFromYAML(YAML::Node& _node)
{
	SetRenderType(_node["DECAL"]["LightingMethod"].as<bool>());

	YAML::Node node = _node["DECAL"]["DecalTexture"];
	LoadResourceRef<CTexture>(m_pDecalTex, node);
}
