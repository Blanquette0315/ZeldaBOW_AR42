#include "pch.h"
#include "CMapRender.h"

#include "CGameObject.h"
#include "CTransform.h"

#include "CMesh.h"
#include "CMaterial.h"

#include "CAnimator2D.h"
#include "CAnimator3D.h"

#include "CDevice.h"

CMapRender::CMapRender()
	: CRenderComponent(COMPONENT_TYPE::MAPRENDER)
{
}

CMapRender::~CMapRender()
{
}


void CMapRender::tick()
{
}

void CMapRender::finaltick()
{
}

void CMapRender::render()
{
	if (!IsActivate())
		return;

	if (nullptr == GetMesh())
		return;

	// 위치정보 업데이트
	Transform()->UpdateData();

	// Animator2D 컴포넌트가 있다면
	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	// Animator3D 업데이트
	if (Animator3D())
	{
		Animator3D()->UpdateData();

		for (size_t i = 0; i < GetMtrlCount(); ++i)
		{
			if (nullptr == GetCurMaterial(i))
				continue;
			GetCurMaterial(i)->SetAnim3D(true); // Animation Mesh 알리기
			GetCurMaterial(i)->SetBoneCount(Animator3D()->GetBoneCount());
		}
	}

	// 사용할 재질 업데이트
	UINT iSubsetCount = GetMesh()->GetSubsetCount();

	for (int i = 0; i < iSubsetCount; ++i)
	{
		if (nullptr != GetCurMaterial(i))
		{
			GetCurMaterial(i)->UpdateData();

			// 사용할 메쉬 업데이트 및 렌더링
			GetMesh()->render(i);
		}
	}

	// 사용한 텍스쳐 레지스터 비우기
	CMaterial::Clear();

	// Animator2D 컴포넌트가 있었으면, 관련 레지스터 클리어
	if (Animator2D())
	{
		Animator2D()->Clear();
	}

	if (Animator3D())
		Animator3D()->ClearData();
}

void CMapRender::render(UINT _iSubset)
{
	if (nullptr == GetMesh() || nullptr == GetCurMaterial(_iSubset))
		return;

	// ==========
	// UpdateData
	// ==========
	// Transform 업데이트
	Transform()->UpdateData();

	// Animation2D 업데이트
	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	// Animator3D 업데이트
	if (Animator3D())
	{
		Animator3D()->UpdateData();
		GetCurMaterial(_iSubset)->SetAnim3D(true); // Animation Mesh 알리기
		GetCurMaterial(_iSubset)->SetBoneCount(Animator3D()->GetBoneCount());
	}

	// ======
	// Render
	// ======
	GetCurMaterial(_iSubset)->UpdateData();
	GetMesh()->render(_iSubset);

	// =====
	// Clear
	// =====	
	if (Animator2D())
		Animator2D()->Clear();

	if (Animator3D())
		Animator3D()->ClearData();
}

void CMapRender::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "MAPRENDER";
	_emitter << YAML::Value << YAML::BeginMap;
	CRenderComponent::SaveToYAML(_emitter);
	_emitter << YAML::EndMap;
}

void CMapRender::LoadFromYAML(YAML::Node& _node)
{
	YAML::Node node = _node["MAPRENDER"];
	CRenderComponent::LoadFromYAML(node);

	if (Animator3D())
	{
		UINT i = GetMesh()->GetAnimClip()->at(0).iEndFrame;
		Animator3D()->SetFrameLimit(GetMesh()->GetAnimClip()->at(0).iEndFrame);
	}
}