#include "pch.h"
#include "CSkyBox.h"

#include "CTransform.h"

CSkyBox::CSkyBox()
	: CRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_eSkyBoxType(SKYBOX_TYPE::SPHERE)
{
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"SkyBoxMtrl"));
	SetType(m_eSkyBoxType);
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
	// ���� SkyBox ��ü�� ���� ī�޶� �̵��Ҷ�, �ش� SkyBox�� �Բ� �̵��ϵ��� �����ϴ� ���
	// �̷��� �ǽð����� ��ġ�� ������ �ϴ� ��İ�, �츮�� �����߿� ���� shader�ڵ� �ʿ��� ó�����ִ� �� ���� ������̴� ũ�� ���� �� ���ٰ� �ϽŴ�.
	// MainCam�� ��ġ�� �˾Ƴ���.
	// SkyBox�� ��ġ���� �����Ѵ�.

	// MainCom�� Far ���� �����´�.
	// Transform Scale�� �����Ѵ�.
}

void CSkyBox::render()
{
	Transform()->UpdateData();

	GetCurMaterial()->SetScalarParam(INT_0, (void*)&m_eSkyBoxType);
	GetCurMaterial()->SetScalarParam(MAT_0, (void*)&Transform()->GetWorldRotMat());

	if (m_eSkyBoxType == SKYBOX_TYPE::SPHERE)
	{
		GetCurMaterial()->SetTexParam(TEX_0, m_pSkyBoxTex);
	}

	else if (m_eSkyBoxType == SKYBOX_TYPE::CUBE)
	{
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