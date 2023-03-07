#include "pch.h"
#include "CTransform.h"

#include "CDevice.h"
#include "CConstBuffer.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_vRelativeScale(Vec3(1.f,1.f,1.f))
{
	m_vRelativeDir[(UINT)DIR::RIGHT]	= Vec3(1.f, 0.f, 0.f);
	m_vRelativeDir[(UINT)DIR::UP]		= Vec3(0.f, 1.f, 0.f);
	m_vRelativeDir[(UINT)DIR::FRONT]	= Vec3(0.f, 0.f, 1.f);
}

CTransform::~CTransform()
{
}

void CTransform::tick()
{
}

void CTransform::finaltick()
{
	// m_vRelativePos, m_vRelativeScale, m_vRelativeRotation
	// ㄴ=> 월드 행렬을 만들어 냄
	m_matWorld = XMMatrixIdentity();

	Matrix matScale = XMMatrixScaling(m_vRelativeScale.x, m_vRelativeScale.y, m_vRelativeScale.z);

	Matrix matTrans = XMMatrixTranslation(m_vRelativePos.x, m_vRelativePos.y, m_vRelativePos.z);
	
	Matrix matRot = XMMatrixRotationX(m_vRelativeRotation.x);
	matRot *= XMMatrixRotationY(m_vRelativeRotation.y);
	matRot *= XMMatrixRotationZ(m_vRelativeRotation.z);
	m_matWorldRot = m_matWorld * matRot;

	// 회전 행렬을 이용해서 현재 물체의 우, 상, 전 방향을 구해놓는다.
	m_vRelativeDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), matRot);
	m_vRelativeDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), matRot);
	m_vRelativeDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, 1.f), matRot);

	m_matWorld = matScale * matRot * matTrans;

	// 만약 부모 오브젝트가 있다면
	if (GetOwner()->GetParent())
	{
		// 부모 오브젝트의 크기를 무시하기로 한 경우
		if (m_bIgnParentScale)
		{
			// 위쪽으로 모든 부모의 크기 값을 누적해서 역행렬을 만들어 둔다.
			CGameObject* pParent = GetOwner()->GetParent();
			Vec3 vParentScale = Vec3(1.f, 1.f, 1.f);

			while (pParent)
			{
				vParentScale *= pParent->Transform()->GetRelativeScale();

				// 부모 오브젝트도, 그 위로 크기를 무시하기로 한 경우 크기를 더 이상 누적할 필요가 없다.
				if (pParent->Transform()->m_bIgnParentScale)
				{
					pParent = nullptr;
				}
				else
				{
					pParent = pParent->GetParent();
				}

				// 부모 행렬의 크기 부분의 역행렬을 구해서 상쇄 시킨다.
				// 역행렬 예외 처리 (크기중 하나가 0일 경우, 역행렬이 만들어지지 않기 떄문)
				if (0.f == vParentScale.x)
					vParentScale.x = 1.f;
				if (0.f == vParentScale.y)
					vParentScale.y = 1.f;
				if (0.f == vParentScale.z)
					vParentScale.z = 1.f;

				Matrix matParentScaleInv = XMMatrixScaling(vParentScale.x, vParentScale.y, vParentScale.z);
				matParentScaleInv = XMMatrixInverse(nullptr, matParentScaleInv);

				// 구한 역행렬을 미리 곱해두고 부모의 월드 행렬을 곱한다.
				m_matWorld = m_matWorld * matParentScaleInv * GetOwner()->GetParent()->Transform()->GetWorldMat();
			}
		}

		// 부모 오브젝트의 크기를 무시하지 않기로 한 경우
		else
		{
			m_matWorld *= GetOwner()->GetParent()->Transform()->GetWorldMat();
		}

		//// WorldDir 구하기
		//m_vWorldDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), m_matWorld);
		//m_vWorldDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matWorld);
		//m_vWorldDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, 1.f), m_matWorld);
		//
		//// 회전, 크기 변환이 이루어졌기 때문에 변경된 크기를 초기화 하기 위해 Normalize 해준다.ㄴㄴ
		//m_vWorldDir[(UINT)DIR::RIGHT].Normalize();
		//m_vWorldDir[(UINT)DIR::UP].Normalize();
		//m_vWorldDir[(UINT)DIR::FRONT].Normalize();
	}
	// WorldDir 구하기
	m_vWorldDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), m_matWorld);
	m_vWorldDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matWorld);
	m_vWorldDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, 1.f), m_matWorld);

	// 회전, 크기 변환이 이루어졌기 때문에 변경된 크기를 초기화 하기 위해 Normalize 해준다.ㄴㄴ
	m_vWorldDir[(UINT)DIR::RIGHT].Normalize();
	m_vWorldDir[(UINT)DIR::UP].Normalize();
	m_vWorldDir[(UINT)DIR::FRONT].Normalize();

	m_matWorldInv = XMMatrixInverse(nullptr, m_matWorld);
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

Vec3 CTransform::GetWorldScale()
{
	// 위쪽으로 모든 부모의 크기값을 누적해서 역행렬을 만들어 준다.
	Vec3 vWorldScale = m_vRelativeScale;

	if (m_bIgnParentScale)
		return vWorldScale;

	CGameObject* pParent = GetOwner()->GetParent();
	while (pParent)
	{
		vWorldScale *= pParent->Transform()->GetRelativeScale();

		// 부모 오브젝트도 그 위로 크기를 무시하기로 한 경우 크기를 더 이상 누적할 필요 없다.
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
	Temp.left = vWorldPos.x - (m_vRelativeScale.x * 0.5f);
	Temp.top = vWorldPos.y + (m_vRelativeScale.y * 0.5f);
	Temp.right = vWorldPos.x + (m_vRelativeScale.x * 0.5f);
	Temp.bottom = vWorldPos.y - (m_vRelativeScale.y * 0.5f);

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