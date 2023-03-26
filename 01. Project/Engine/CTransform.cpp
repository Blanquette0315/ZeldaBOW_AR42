#include "pch.h"
#include "CTransform.h"

#include "CDevice.h"
#include "CConstBuffer.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_vRelativeScale(Vec3(1.f,1.f,1.f))
	, m_bIgnParentScale(false)
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
	// m_vRelativePos, m_vRelativeScale, m_vRelativeRotation
	// ��=> ���� ����� ����� ��
	m_matWorld = XMMatrixIdentity();

	Matrix matScale = XMMatrixScaling(m_vRelativeScale.x, m_vRelativeScale.y, m_vRelativeScale.z);

	Matrix matTrans = XMMatrixTranslation(m_vRelativePos.x, m_vRelativePos.y, m_vRelativePos.z);
	
	m_matRot = XMMatrixRotationX(m_vRelativeRotation.x);
	m_matRot *= XMMatrixRotationY(m_vRelativeRotation.y);
	m_matRot *= XMMatrixRotationZ(m_vRelativeRotation.z);
	m_matWorldRot = m_matWorld * m_matRot;

	// ȸ�� ����� �̿��ؼ� ���� ��ü�� ��, ��, �� ������ ���س��´�.
	m_vRelativeDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), m_matRot);
	m_vRelativeDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matRot);
	m_vRelativeDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, 1.f), m_matRot);

	m_matWorld = matScale * m_matRot * matTrans;

	// ���� �θ� ������Ʈ�� �ִٸ�
	if (GetOwner()->GetParent())
	{
		// �θ� ������Ʈ�� ũ�⸦ �����ϱ�� �� ���
		if (m_bIgnParentScale)
		{
			// �������� ��� �θ��� ũ�� ���� �����ؼ� ������� ����� �д�.
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
					Matrix matParentScaleInv = XMMatrixScaling(vParentScale.x, vParentScale.y, vParentScale.z);
					matParentScaleInv = XMMatrixInverse(nullptr, matParentScaleInv);
					m_matWorld = m_matWorld * matParentScaleInv * pParent->Transform()->GetWorldMat();
					pParent = nullptr;
				}
				else
				{
					Matrix matParentRot = XMMatrixRotationX(vParentRot.x);
					matParentRot *= XMMatrixRotationY(vParentRot.y);
					matParentRot *= XMMatrixRotationZ(vParentRot.z);
					Matrix matParentTrans = XMMatrixTranslation(vParentPos.x, vParentPos.y, vParentPos.z);

					m_matWorld = m_matWorld * matParentRot * matParentTrans;
					pParent = pParent->GetParent();
				}
			}
		}

		// �θ� ������Ʈ�� ũ�⸦ �������� �ʱ�� �� ���
		else
		{
			m_matWorld *= GetOwner()->GetParent()->Transform()->GetWorldMat();
		}
	}
	// WorldDir ���ϱ�
	m_vWorldDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), m_matWorld);
	m_vWorldDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matWorld);
	m_vWorldDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, 1.f), m_matWorld);

	// ȸ��, ũ�� ��ȯ�� �̷������ ������ ����� ũ�⸦ �ʱ�ȭ �ϱ� ���� Normalize ���ش�.����
	m_vWorldDir[(UINT)DIR::RIGHT].Normalize();
	m_vWorldDir[(UINT)DIR::UP].Normalize();
	m_vWorldDir[(UINT)DIR::FRONT].Normalize();

	m_matWorldInv = XMMatrixInverse(nullptr, m_matWorld);
	m_matRotInv = XMMatrixInverse(nullptr, m_matRot);
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
			Vec3 vParentRot = pParent->Transform()->GetRelativeRotation();
			Vec3 vParentPos = pParent->Transform()->GetRelativePos();

			Matrix matParentRot = XMMatrixRotationX(vParentRot.x);
			matParentRot *= XMMatrixRotationY(vParentRot.y);
			matParentRot *= XMMatrixRotationZ(vParentRot.z);
			Matrix matParentTrans = XMMatrixTranslation(vParentPos.x, vParentPos.y, vParentPos.z);

			Matrix matInv = XMMatrixInverse(nullptr, matParentRot * matParentTrans);
			matWorldCopy *= matInv;
		}
	}
	else
	{
		if (pParent)
		{
			Matrix matInv = XMMatrixInverse(nullptr, pParent->Transform()->GetWorldMat());
			matWorldCopy *= matInv;
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

	if (!m_bIgnParentScale)
	{
		Vec3 vSumScale = Vec3(1.f, 1.f, 1.f);
		while (pParent)
		{
			vSumScale *= pParent->Transform()->GetRelativeScale();

			if (pParent->Transform()->GetIgnoreParentScale())
				break;

			pParent = pParent->GetParent();
		}
		SetRelativeScale(_vScale / vSumScale);
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

	Vec3 vSumRot = Vec3(0.f, 0.f, 0.f);
	while (pParent)
	{
		vSumRot += pParent->Transform()->GetRelativeRotation();
		pParent = pParent->GetParent();
	}
	SetRelativeRotation(_vRot - vSumRot);
}

Vec3 CTransform::GetWorldScale()
{
	// �������� ��� �θ��� ũ�Ⱚ�� �����ؼ� ������� ����� �ش�.
	Vec3 vWorldScale = m_vRelativeScale;

	if (m_bIgnParentScale)
		return vWorldScale;

	CGameObject* pParent = GetOwner()->GetParent();
	while (pParent)
	{
		vWorldScale *= pParent->Transform()->GetRelativeScale();

		// �θ� ������Ʈ�� �� ���� ũ�⸦ �����ϱ�� �� ��� ũ�⸦ �� �̻� ������ �ʿ� ����.
		if (pParent->Transform()->m_bIgnParentScale)
			pParent = nullptr;
		else
			pParent = pParent->GetParent();
	}

	return vWorldScale;
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

	_emitter << YAML::EndMap;
}

void CTransform::LoadFromYAML(YAML::Node& _node)
{
	m_vRelativePos = _node["TRANSFORM"]["RelativePos"].as<Vec3>();
	m_vRelativeScale = _node["TRANSFORM"]["RelativeScale"].as<Vec3>();
	m_vRelativeRotation = _node["TRANSFORM"]["RelativeRotation"].as<Vec3>();
	m_bIgnParentScale = _node["TRANSFORM"]["IgnParentScale"].as<bool>();
}