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
	// ť�� ���� �Ž� Ȯ���� ���� ����� �����
	DebugDrawCube(Vec4(0.2f, 0.8f, 0.2f, 1.f), Transform()->GetWorldPos(), Transform()->GetWorldScale(), Transform()->GetRelativeRotation());

	// ��ġ ��� ������Ʈ
	Transform()->UpdateData();

	// ���׸��� ������Ʈ
	CRenderMgr::GetInst()->CopyPositionTarget();
	GetCurMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PTCopyTex"));
	GetCurMaterial()->UpdateData();

	// ������
	GetMesh()->render();

	CMaterial::Clear();
}