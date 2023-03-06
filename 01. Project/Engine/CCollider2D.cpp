#include "pch.h"
#include "CCollider2D.h"

#include "CTransform.h"
#include "CScript.h"

CCollider2D::CCollider2D()
	: CComponent(COMPONENT_TYPE::COLLIDER2D)
	, m_vScale(Vec2(1.f, 1.f))
	, m_bIgnorObjectScale(false)
	, m_eType(COLLIDER2D_TYPE::COLLIDER2D_RECT)
	, m_iOverlapCount(0)
{
}

CCollider2D::~CCollider2D()
{
}

void CCollider2D::finaltick()
{
	//FinalPos ���
	Vec3 vObjectPos = Transform()->GetWorldPos();
	m_vFinalPos = m_vOffsetPos + Vec2(vObjectPos.x, vObjectPos.y);

	// ��� ����
	// ũ�� * ȸ�� * �̵�
	m_vFinalScale = Vec2(m_vScale.x, m_vScale.y);
	if (!m_bIgnorObjectScale)
	{
		Vec3 vWorldScale = Transform()->GetWorldScale();
		m_vFinalScale *= Vec2(vWorldScale.x, vWorldScale.y);
	}

	Matrix matScale = XMMatrixScaling(m_vFinalScale.x, m_vFinalScale.y, 1.f);

	Matrix matRot = XMMatrixRotationX(m_vRot.x);
	matRot *= XMMatrixRotationY(m_vRot.y);
	matRot *= XMMatrixRotationZ(m_vRot.z);

	Matrix matTrans = XMMatrixTranslation(m_vFinalPos.x, m_vFinalPos.y, 0.f);

	// �浹ü ���� ���
	m_matWorld = matScale * matRot * matTrans;

	// DebugDraw ��û
#ifdef _DEBUG
	// �������� 1�� �̻��̸� ���������� �׸���.
	Vec4 vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	if(0 < m_iOverlapCount)
		vColor = Vec4(1.f, 0.f, 0.f, 1.f);

	if (COLLIDER2D_TYPE::COLLIDER2D_RECT == m_eType)
	{
		// ���� �ð��� 0���� �־��ִ� ������ Collider2D���� �� Finaltick ���� ȣ�����ֱ� ������ 0�� ���� ������ ��ø�� �߻��Ѵ�.
		DebugDrawRect(vColor, Vec3(m_vFinalPos.x, m_vFinalPos.y, 0.f), Vec3(m_vFinalScale.x, m_vFinalScale.y, 1.f), m_vRot);
	}
	if (COLLIDER2D_TYPE::COLLIDER2D_CIRCLE == m_eType)
	{
		DebugDrawCircle(vColor, Vec3(m_vFinalPos.x, m_vFinalPos.y, 0.f), m_vFinalScale.x);
	}
#endif
}

void CCollider2D::BeginOverlap(CCollider2D* _pOther)
{
	++m_iOverlapCount;

	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->BeginOverlap(_pOther);
	}
}

void CCollider2D::Overlap(CCollider2D* _pOther)
{
	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->Overlap(_pOther);
	}
}

void CCollider2D::EndOverlap(CCollider2D* _pOther)
{
	--m_iOverlapCount;

	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScripts.size(); ++i)
	{
		vecScripts[i]->EndOverlap(_pOther);
	}
}




void CCollider2D::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE type = GetType();
	fwrite(&type, sizeof(UINT), 1, _File);

	fwrite(&m_vOffsetPos, sizeof(Vec2), 1, _File);
	fwrite(&m_vScale, sizeof(Vec2), 1, _File);
	fwrite(&m_vRot, sizeof(Vec3), 1, _File);
	fwrite(&m_eType, sizeof(UINT), 1, _File);
	fwrite(&m_bIgnorObjectScale, sizeof(bool), 1, _File);
}

void CCollider2D::LoadFromFile(FILE* _File)
{
	fread(&m_vOffsetPos, sizeof(Vec2), 1, _File);
	fread(&m_vScale, sizeof(Vec2), 1, _File);
	fread(&m_vRot, sizeof(Vec3), 1, _File);
	fread(&m_eType, sizeof(UINT), 1, _File);
	fread(&m_bIgnorObjectScale, sizeof(bool), 1, _File);
}