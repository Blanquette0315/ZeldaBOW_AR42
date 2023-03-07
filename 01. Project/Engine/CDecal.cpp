#include "pch.h"
#include "CDecal.h"

#include "CRenderMgr.h"
#include "CTransform.h"

CDecal::CDecal()
	: CRenderComponent(COMPONENT_TYPE::DECAL)
	, m_bLighting(false)
{
	// 데칼에서 사용할 매시는 일반적으로 큐브 고정이기 때문에 생성자에서 고정해준다.
	// Shader의 경우 Deferred냐, forward냐에 따라 달라지기 때문에 함수로 분류할 것이다.
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
}

CDecal::~CDecal()
{
}

void CDecal::SetRenderType(bool _bSet)
{
	m_bLighting = _bSet;

	// true일 경우 Deferred로 랜더링하겠다는 의미이다.
	if (m_bLighting)
	{
		SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Deferred_DecalMtrl"));
	}

	// false일 경우 Forward로 랜더링하겠다는 의미이다.
	else
	{
		SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DecalMtrl"));
	}
}

void CDecal::finaltick()
{
}

void CDecal::render()
{
	// 큐브 볼륨 매시 확인을 위한 디버그 드로잉
	DebugDrawCube(Vec4(0.2f, 0.8f, 0.2f, 1.f), Transform()->GetWorldPos(), Transform()->GetWorldScale(), Transform()->GetRelativeRotation());

	// 위치 행렬 업데이트
	Transform()->UpdateData();

	// 머테리얼 업데이트
	CRenderMgr::GetInst()->CopyPositionTarget();
	GetCurMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PTCopyTex"));
	GetCurMaterial()->UpdateData();

	// 렌더링
	GetMesh()->render();

	CMaterial::Clear();
}