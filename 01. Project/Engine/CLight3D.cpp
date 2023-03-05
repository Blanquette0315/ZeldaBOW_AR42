#include "pch.h"
#include "CLight3D.h"

#include "CRenderMgr.h"
#include "CTransform.h"

CLight3D::CLight3D()
	: CComponent(COMPONENT_TYPE::LIGHT3D)
	, m_iLightIdx(-1)
{
}

CLight3D::~CLight3D()
{
}


void CLight3D::CalcuRelativeScale()
{
	float length = tan(m_Info.fOutAngle / 180.f * XM_PI) * m_Info.fRadius;
	Transform()->SetRelativeScale(Vec3(length, length, m_Info.fRadius));
}

void CLight3D::finaltick()
{
	if (LIGHT_TYPE::POINT == m_Info.iLightType)
	{
		Transform()->SetRelativeScale(Vec3(m_Info.fRadius * 2.f, m_Info.fRadius * 2.f, m_Info.fRadius * 2.f));
	}
	if (LIGHT_TYPE::SPOT == m_Info.iLightType)
	{
		CalcuRelativeScale();
	}

	m_Info.vWorldPos = Transform()->GetWorldPos();
	m_Info.vWorldDir = Transform()->GetWorldDir(DIR::FRONT);

	// RenderMgr에 등록시킨 후, 인덱스를 리턴 받는다.
	m_iLightIdx = CRenderMgr::GetInst()->RegisterLight3D(this);
}

void CLight3D::render()
{
	if (LIGHT_TYPE::POINT == m_Info.iLightType)
		DebugDrawSphere(Vec4(0.2f, 0.8f, 0.2f, 1.f), Transform()->GetWorldPos(), Transform()->GetWorldScale().x * 0.5f);

	if (LIGHT_TYPE::SPOT == m_Info.iLightType)
	{
		DebugDrawCone(Vec4(0.2f, 0.8f, 0.2f, 1.f), Transform()->GetWorldPos(), Transform()->GetRelativeRotation(), m_Info.fRadius, m_Info.fOutAngle);
	}

	if (nullptr == m_pLightMtrl)
		return;

	// Transform Update
	Transform()->UpdateData();

	// 재질을 통해서 본인의 라이트 인덱스를 전달
	m_pLightMtrl->SetScalarParam(INT_0, &m_iLightIdx);

	// 재질 바인딩 (셰이더, 상수, 텍스쳐(Target) 등등)
	m_pLightMtrl->UpdateData();

	// 랜더링
	m_pVolumeMesh->render();
}

void CLight3D::SetLightType(LIGHT_TYPE _type)
{
	m_Info.iLightType = _type;

	if (LIGHT_TYPE::DIRECTIONAL == m_Info.iLightType)
	{
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DirLightMtrl");
	}

	else if (LIGHT_TYPE::POINT == m_Info.iLightType)
	{
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	}

	else
	{
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"ConeMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"SpotLightMtrl");
	}
}

void CLight3D::SaveToFile(FILE* _File)
{
}

void CLight3D::LoadFromFile(FILE* _File)
{
}