#include "pch.h"
#include "CMeshRender.h"

#include "CGameObject.h"
#include "CTransform.h"

#include "CMesh.h"
#include "CMaterial.h"

#include "CAnimator2D.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::tick()
{
}

void CMeshRender::finaltick()
{
}

void CMeshRender::render()
{
	if (!IsActivate())
		return;

	if (nullptr == GetMesh() || nullptr == GetCurMaterial())
		return;

	// 위치정보 업데이트
	Transform()->UpdateData();

	// 사용할 재질 업데이트
	GetCurMaterial()->UpdateData();

	// Animator2D 컴포넌트가 있다면
	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	// 사용할 메쉬 업데이트 및 렌더링
	GetMesh()->render();

	// 사용한 텍스쳐 레지스터 비우기
	CMaterial::Clear();

	// Animator2D 컴포넌트가 있었으면, 관련 레지스터 클리어
	if (Animator2D())
	{
		Animator2D()->Clear();
	}
}