#include "pch.h"
#include "CTransform.h"

#include "CDevice.h"
#include "CConstBuffer.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_vRelativeScale(Vec3(1.f,1.f,1.f))
	, m_bIgnParentScale(false)
	, m_bTurnY180(false)
	, m_bBase270Rot(false)
{
	m_vRelativeDir[(UINT)DIR::RIGHT]	= Vec3(1.f, 0.f, 0.f);
	m_vRelativeDir[(UINT)DIR::UP]		= Vec3(0.f, 1.f, 0.f);
	m_vRelativeDir[(UINT)DIR::FRONT]	= Vec3(0.f, 0.f, 1.f);
	m_BoundingBox.SetOwner(GetOwner());
}

CTransform::~CTransform()
{
}

CGameObject* CTransform::CheckRay(tRay _ray)
{
	m_BoundingBox.SetOwner(GetOwner());
	return m_BoundingBox.CheckRay(_ray);
}

void CTransform::begin()
{
	finaltick();
}

void CTransform::tick()
{
}

void CTransform::finaltick()
{
	m_matWorld = XMMatrixIdentity();

	m_matScale = XMMatrixScaling(m_vRelativeScale.x, m_vRelativeScale.y, m_vRelativeScale.z);

	m_matTrans = XMMatrixTranslation(m_vRelativePos.x, m_vRelativePos.y, m_vRelativePos.z);
	
	if (m_bBase270Rot)
	{
		m_matRot = XMMatrixRotationX(m_vRelativeRotation.x + 4.71239f);
	}
	else
	{
		m_matRot = XMMatrixRotationX(m_vRelativeRotation.x);
	}
	//m_matRot = XMMatrixRotationX(m_vRelativeRotation.x);
	m_matRot *= XMMatrixRotationY(m_vRelativeRotation.y);
	m_matRot *= XMMatrixRotationZ(m_vRelativeRotation.z);

	if (m_bTurnY180)
	{
		m_vRelativeDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(-1.f, 0.f, 0.f), m_matRot);
		m_vRelativeDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matRot);
		m_vRelativeDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, -1.f), m_matRot);
	}
	else
	{
		m_vRelativeDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), m_matRot);
		m_vRelativeDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matRot);
		m_vRelativeDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, 1.f), m_matRot);
	}

	m_matWorld = m_matScale * m_matRot * m_matTrans;
	m_matLocal = m_matWorld;

	if (GetOwner()->GetParent())
	{
		if (m_bIgnParentScale)
		{
			CGameObject* pParent = GetOwner()->GetParent();

			while (pParent)
			{
				Vec3 vParentScale = pParent->Transform()->m_vRelativeScale;
				Vec3 vParentRot = pParent->Transform()->m_vRelativeRotation;
				Vec3 vParentPos = pParent->Transform()->m_vRelativePos;

				if (0.f == vParentScale.x)
					vParentScale.x = 1.f;
				if (0.f == vParentScale.y)
					vParentScale.y = 1.f;
				if (0.f == vParentScale.z)
					vParentScale.z = 1.f;
				
				if(pParent->Transform()->m_bIgnParentScale)
				{
					m_matWorld = m_matWorld * pParent->Transform()->m_matScaleInv * pParent->Transform()->m_matWorld;
					pParent = nullptr;
				}
				else
				{
					m_matWorld = m_matWorld * pParent->Transform()->m_matRot * pParent->Transform()->m_matTrans;
					pParent = pParent->GetParent();
				}
			}
		}

		else
		{
			m_matWorld *= GetOwner()->GetParent()->Transform()->m_matWorld;
		}
	}
	if (m_bTurnY180)
	{
		m_vWorldDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(-1.f, 0.f, 0.f), m_matWorld);
		m_vWorldDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matWorld);
		m_vWorldDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, -1.f), m_matWorld);
	}
	else
	{
		m_vWorldDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), m_matWorld);
		m_vWorldDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matWorld);
		m_vWorldDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, 1.f), m_matWorld);
	}

	m_vWorldDir[(UINT)DIR::RIGHT].Normalize();
	m_vWorldDir[(UINT)DIR::UP].Normalize();
	m_vWorldDir[(UINT)DIR::FRONT].Normalize();

	m_matWorldInv = XMMatrixInverse(nullptr, m_matWorld);

	m_matScaleInv = XMMatrixInverse(nullptr, m_matScale);
	m_matRotInv = XMMatrixInverse(nullptr, m_matRot);
	m_matTransInv = XMMatrixInverse(nullptr, m_matTrans);
}

void CTransform::UpdateWorldTrans()
{
	m_matWorld = XMMatrixIdentity();

	m_matScale = XMMatrixScaling(m_vRelativeScale.x, m_vRelativeScale.y, m_vRelativeScale.z);

	m_matTrans = XMMatrixTranslation(m_vRelativePos.x, m_vRelativePos.y, m_vRelativePos.z);

	m_matRot = XMMatrixRotationX(m_vRelativeRotation.x);
	m_matRot *= XMMatrixRotationY(m_vRelativeRotation.y);
	m_matRot *= XMMatrixRotationZ(m_vRelativeRotation.z);

	if (m_bTurnY180)
	{
		m_vRelativeDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(-1.f, 0.f, 0.f), m_matRot);
		m_vRelativeDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matRot);
		m_vRelativeDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, -1.f), m_matRot);
	}
	else
	{
		m_vRelativeDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), m_matRot);
		m_vRelativeDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matRot);
		m_vRelativeDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, 1.f), m_matRot);
	}

	m_matWorld = m_matScale * m_matRot * m_matTrans;
	m_matLocal = m_matWorld;

	if (GetOwner()->GetParent())
	{
		if (m_bIgnParentScale)
		{
			CGameObject* pParent = GetOwner()->GetParent();

			while (pParent)
			{
				Vec3 vParentScale = pParent->Transform()->m_vRelativeScale;
				Vec3 vParentRot = pParent->Transform()->m_vRelativeRotation;
				Vec3 vParentPos = pParent->Transform()->m_vRelativePos;

				if (0.f == vParentScale.x)
					vParentScale.x = 1.f;
				if (0.f == vParentScale.y)
					vParentScale.y = 1.f;
				if (0.f == vParentScale.z)
					vParentScale.z = 1.f;

				if (pParent->Transform()->m_bIgnParentScale)
				{
					m_matWorld = m_matWorld * pParent->Transform()->m_matScaleInv * pParent->Transform()->m_matWorld;
					pParent = nullptr;
				}
				else
				{
					m_matWorld = m_matWorld * pParent->Transform()->m_matRot * pParent->Transform()->m_matTrans;
					pParent = pParent->GetParent();
				}
			}
		}

		else
		{
			m_matWorld *= GetOwner()->GetParent()->Transform()->m_matWorld;
		}
	}
	if (m_bTurnY180)
	{
		m_vWorldDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(-1.f, 0.f, 0.f), m_matWorld);
		m_vWorldDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matWorld);
		m_vWorldDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, -1.f), m_matWorld);
	}
	else
	{
		m_vWorldDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), m_matWorld);
		m_vWorldDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matWorld);
		m_vWorldDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, 1.f), m_matWorld);
	}

	m_vWorldDir[(UINT)DIR::RIGHT].Normalize();
	m_vWorldDir[(UINT)DIR::UP].Normalize();
	m_vWorldDir[(UINT)DIR::FRONT].Normalize();

	m_matWorldInv = XMMatrixInverse(nullptr, m_matWorld);

	m_matScaleInv = XMMatrixInverse(nullptr, m_matScale);
	m_matRotInv = XMMatrixInverse(nullptr, m_matRot);
	m_matTransInv = XMMatrixInverse(nullptr, m_matTrans);
}

void CTransform::UpdateData()
{
	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);

	g_transform.matWorld = m_matWorld;
	g_transform.matWorldInv = m_matWorldInv;
	g_transform.matWV = g_transform.matWorld * g_transform.matView;
	g_transform.matWVP = g_transform.matWV * g_transform.matProj;

	pCB->SetData(&g_transform);
	pCB->UpdateData(PIPELINE_STAGE::ALL_STAGE);
}

void CTransform::SetWorldPos(Vec3 _vPos)
{
	Matrix matWorldCopy = m_matWorld;
	matWorldCopy._41 = _vPos.x;
	matWorldCopy._42 = _vPos.y;
	matWorldCopy._43 = _vPos.z;

	Matrix MatParentSum = XMMatrixIdentity();

	CGameObject* pOwner = GetOwner();
	CGameObject* pParent = pOwner->GetParent();

	if (m_bIgnParentScale)
	{
		while (pParent)
		{
			const Matrix& vParentTransMatInv = pParent->Transform()->m_matTransInv;
			const Matrix& vParentRotMatInv = pParent->Transform()->m_matRotInv;

			Matrix matInv = vParentTransMatInv * vParentRotMatInv;
			matWorldCopy *= matInv;
			pParent = pParent->GetParent();
		}
	}
	else
	{
		if (pParent)
		{
			matWorldCopy *= pParent->Transform()->m_matWorldInv;
		}
	}

	Vec3 vRelativPos;
	vRelativPos = matWorldCopy.Translation();
	SetRelativePos(vRelativPos);
}

void CTransform::SetWorldScale(Vec3 _vScale)
{
	CGameObject* pOwner = GetOwner();
	CGameObject* pParent = pOwner->GetParent();

	if (!m_bIgnParentScale && pParent)
	{
		_vScale = XMVector3TransformNormal(_vScale, pParent->Transform()->GetWorldMatInv());
		SetRelativeScale(_vScale);
	}
	else
	{
		SetRelativeScale(_vScale);
	}
}

void CTransform::SetWorldRotation(Vec3 _vRot)
{
	CGameObject* pOwner = GetOwner();
	CGameObject* pParent = pOwner->GetParent();

	if (!m_bIgnParentScale && pParent)
	{
		_vRot = XMVector3TransformNormal(_vRot, pParent->Transform()->GetWorldMatInv());
		SetRelativeRotation(_vRot);
	}
	else
	{
		SetRelativeRotation(_vRot);
	}
}

Vec3 CTransform::GetWorldScale()
{
	XMVECTOR vScale;
	XMVECTOR vDummy;
	XMVECTOR vDummy2;
	XMMatrixDecompose(&vScale, &vDummy, &vDummy2, m_matWorld);
	return Vec3(vScale);
}

RECT CTransform::GetRectCoord()
{
	RECT Temp = {};

	Vec3 vWorldPos = GetWorldPos();
	Temp.left = (LONG)(vWorldPos.x - (m_vRelativeScale.x * 0.5f));
	Temp.top = (LONG)(vWorldPos.y + (m_vRelativeScale.y * 0.5f));
	Temp.right = (LONG)(vWorldPos.x + (m_vRelativeScale.x * 0.5f));
	Temp.bottom = (LONG)(vWorldPos.y - (m_vRelativeScale.y * 0.5f));

	return Temp;
}

Vec3 CTransform::GetWorldRotation()
{
	XMVECTOR vQuat;
	XMVECTOR vDummy;
	XMVECTOR vDummy2;
	Vec3 vEuler;
	XMMatrixDecompose(&vDummy, &vQuat, &vDummy2, m_matWorld);
	QuaternionToEuler(vQuat, vEuler);
	return vEuler;
}

const Matrix& CTransform::GetWorldRotMat()
{
	Matrix matWorldRot = m_matRot;

	CGameObject* pParent = GetOwner()->GetParent();

	while (pParent)
	{
		matWorldRot *= pParent->Transform()->m_matRot;
		pParent = pParent->GetParent();
	}

	return matWorldRot;
}

void CTransform::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "TRANSFORM";
	_emitter << YAML::Value << YAML::BeginMap;

	_emitter << YAML::Key << "RelativePos";
	_emitter << YAML::Value << m_vRelativePos;
	_emitter << YAML::Key << "RelativeScale";
	_emitter << YAML::Value << m_vRelativeScale;
	_emitter << YAML::Key << "RelativeRotation";
	_emitter << YAML::Value << m_vRelativeRotation;
	_emitter << YAML::Key << "IgnParentScale";
	_emitter << YAML::Value << m_bIgnParentScale;
	_emitter << YAML::Key << "TurnY180";
	_emitter << YAML::Value << m_bTurnY180;
	_emitter << YAML::Key << "Base270Rot";
	_emitter << YAML::Value << m_bBase270Rot;

	_emitter << YAML::EndMap;
}

void CTransform::LoadFromYAML(YAML::Node& _node)
{
	m_vRelativePos = _node["TRANSFORM"]["RelativePos"].as<Vec3>();
	m_vRelativeScale = _node["TRANSFORM"]["RelativeScale"].as<Vec3>();
	m_vRelativeRotation = _node["TRANSFORM"]["RelativeRotation"].as<Vec3>();
	m_bIgnParentScale = _node["TRANSFORM"]["IgnParentScale"].as<bool>();
	SAFE_LOAD_FROM_YAML(bool, m_bTurnY180, _node["TRANSFORM"]["TurnY180"]);
	SAFE_LOAD_FROM_YAML(bool, m_bBase270Rot, _node["TRANSFORM"]["Base270Rot"]);
}