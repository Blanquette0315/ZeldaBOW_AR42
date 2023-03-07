#include "pch.h"
#include "CLandScape.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_iXFaceCount(1)
	, m_iZFaceCount(1)
{
	//SetFaceCount(m_iXFaceCount, m_iZFaceCount);

	// ��������� �켱 Tess ��� Std3DDeferredMtrl�� ���
	SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DDeferredMtrl"));
}

CLandScape::~CLandScape()
{
}

void CLandScape::finaltick()
{
}

void CLandScape::render()
{
}

void CLandScape::SetFaceCount(UINT _X, UINT _Z)
{
	m_iXFaceCount = _X;
	m_iZFaceCount = _Z;

	// �� ������ �´� �Ž� ����
	CreateMesh();
}