#include "pch.h"
#include "CLight3D.h"

#include "CRenderMgr.h"
#include "CTransform.h"
#include "CCamera.h"

CLight3D::CLight3D()
	: CComponent(COMPONENT_TYPE::LIGHT3D)
	, m_iLightIdx(-1)
	, m_pLightCam(nullptr)
{
	m_pLightCam = new CGameObject;
	m_pLightCam->AddComponent(new CTransform);
	m_pLightCam->AddComponent(new CCamera);
}

CLight3D::CLight3D(const CLight3D& _origin)
	: CComponent(_origin)
	, m_Info(_origin.m_Info)
	, m_iLightIdx(-1)
	, m_pLightCam(nullptr)
{
	m_pLightCam = _origin.m_pLightCam->Clone();
}

CLight3D::~CLight3D()
{
	if (nullptr != m_pLightCam)
		delete m_pLightCam;
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
	if (Is_ShowDebugDraw())
	{
		if (LIGHT_TYPE::POINT == m_Info.iLightType)
			DebugDrawSphere(Vec4(0.2f, 0.8f, 0.2f, 1.f), Transform()->GetWorldPos(), Transform()->GetWorldScale().x * 0.5f);

		if (LIGHT_TYPE::SPOT == m_Info.iLightType)
		{
			DebugDrawCone(Vec4(0.2f, 0.8f, 0.2f, 1.f), Transform()->GetWorldPos(), Transform()->GetRelativeRotation(), m_Info.fRadius, m_Info.fOutAngle);
		}
	}

	if (nullptr == m_pLightMtrl)
		return;

	if (LIGHT_TYPE::DIRECTIONAL == m_Info.iLightType)
	{
		m_pLightCam->Transform()->SetRelativePos(CRenderMgr::GetInst()->GetMainCam()->Transform()->GetRelativePos() - m_Info.vWorldDir * 5000.f);
		m_pLightCam->Transform()->SetRelativeRotation(DecomposeRotMat(Transform()->GetWorldRotMat()));
		m_pLightCam->finaltick_module();
	}

	// Transform Update
	Transform()->UpdateData();

	// 재질을 통해서 본인의 라이트 인덱스를 전달
	m_pLightMtrl->SetScalarParam(INT_0, &m_iLightIdx);

	// 재질 바인딩 (셰이더, 상수, 텍스쳐(Target) 등등)
	m_pLightMtrl->UpdateData();

	// 방향성 광원인 경우 그림자 처리를 위해서 광원카메라로 투영시킬 수 있게 View * Proj 행렬을 전달
	if (LIGHT_TYPE::DIRECTIONAL == (LIGHT_TYPE)m_Info.iLightType)
	{
		Matrix matLightVP = m_pLightCam->Camera()->GetViewMat() * m_pLightCam->Camera()->GetProjMat();
		m_pLightMtrl->SetScalarParam(SCALAR_PARAM::MAT_0, &matLightVP);
		m_pLightMtrl->SetTexParam(TEX_PARAM::TEX_3, CResMgr::GetInst()->FindRes<CTexture>(L"DepthMapTex"));
	}

	// 랜더링
	m_pVolumeMesh->render();
}

void CLight3D::render_depthmap()
{
	m_pLightCam->Camera()->SortShadowObject();
	m_pLightCam->Camera()->render_depthmap();
}

void CLight3D::SetLightDirection(Vec3 _vDir)
{
	m_Info.vWorldDir = _vDir;
	m_Info.vWorldDir.Normalize();

	Vec3 vFront = m_Info.vWorldDir;
	Vec3 vUp = Vec3(0.f, 1.f, 0.f);
	Vec3 vRight = XMVector3Cross(vUp, vFront);
	vRight.Normalize();
	vUp = XMVector3Cross(vFront, vRight);
	vUp.Normalize();

	Matrix matRot = XMMatrixIdentity();
	matRot._11 = vRight.x;	matRot._12 = vRight.y;	matRot._13 = vRight.z;
	matRot._21 = vUp.x;		matRot._22 = vUp.y;		matRot._23 = vUp.z;
	matRot._31 = vFront.x;	matRot._32 = vFront.y;	matRot._33 = vFront.z;

	Vec3 vRot = DecomposeRotMat(matRot);

	// 광원이 가리키는 방향이 Transform 의 Front 가 되도록 회전시켜준다.
	Transform()->SetRelativeRotation(vRot);
}

void CLight3D::SetLightType(LIGHT_TYPE _type)
{
	m_Info.iLightType = _type;

	if (LIGHT_TYPE::DIRECTIONAL == m_Info.iLightType)
	{
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DirLightMtrl");

		m_pLightCam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAPHICS);
		m_pLightCam->Camera()->SetWidth(4096.f);
		m_pLightCam->Camera()->SetAspectRatio(1.f);
		m_pLightCam->Camera()->SetFar(20000.f);
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

void CLight3D::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "LIGHT3D";
	_emitter << YAML::Value << YAML::BeginMap;

	_emitter << YAML::Key << "Diff";
	_emitter << YAML::Value << m_Info.vDiff;
	_emitter << YAML::Key << "Spec";
	_emitter << YAML::Value << m_Info.vSpec;
	_emitter << YAML::Key << "Emb";
	_emitter << YAML::Value << m_Info.vEmb;
	_emitter << YAML::Key << "LightType";
	_emitter << YAML::Value << (UINT)m_Info.iLightType;
	_emitter << YAML::Key << "Radius";
	_emitter << YAML::Value << m_Info.fRadius;
	_emitter << YAML::Key << "InAngle";
	_emitter << YAML::Value << m_Info.fInAngle;
	_emitter << YAML::Key << "OutAngle";
	_emitter << YAML::Value << m_Info.fOutAngle;	

	_emitter << YAML::EndMap;
}

void CLight3D::LoadFromYAML(YAML::Node& _node)
{
	m_Info.vDiff = _node["LIGHT3D"]["Diff"].as<Vec4>();
	m_Info.vSpec = _node["LIGHT3D"]["Spec"].as<Vec4>();
	m_Info.vEmb = _node["LIGHT3D"]["Emb"].as<Vec4>();
	m_Info.iLightType = (LIGHT_TYPE)(_node["LIGHT3D"]["LightType"].as<UINT>());
	SetLightType(m_Info.iLightType);
	m_Info.fRadius = _node["LIGHT3D"]["Radius"].as<float>();
	m_Info.fInAngle = _node["LIGHT3D"]["InAngle"].as<float>();
	m_Info.fOutAngle = _node["LIGHT3D"]["OutAngle"].as<float>();
}