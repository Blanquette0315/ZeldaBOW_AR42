#include "pch.h"
#include "CLandScape.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_iXFaceCount(1)
	, m_iZFaceCount(1)
{
	//SetFaceCount(m_iXFaceCount, m_iZFaceCount);

	// 실험용으로 우선 Tess 대신 Std3DDeferredMtrl을 사용
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

	// 면 갯수에 맞는 매시 생성
	CreateMesh();
}