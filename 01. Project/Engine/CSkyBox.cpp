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
	// 실제 SkyBox 객체를 만들어서 카메라가 이동할때, 해당 SkyBox도 함께 이동하도록 구현하는 방법
	// 이렇게 실시간으로 위치를 갱신해 하는 방식과, 우리가 수업중에 만든 shader코드 쪽에서 처리해주는 것 간의 비용차이는 크지 않을 것 같다고 하신다.
	// MainCam의 위치를 알아낸다.
	// SkyBox의 위치값을 갱신한다.

	// MainCom의 Far 값을 가져온다.
	// Transform Scale로 적용한다.
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