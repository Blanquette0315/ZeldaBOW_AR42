#include "pch.h"
#include "CGrid2DScript.h"

#include <Engine/CRenderMgr.h>
#include <Engine/CDevice.h>

#include <Engine/CTransform.h>
#include <Engine/CCamera.h>
#include <Engine/CMeshRender.h>
#include <Engine/CMaterial.h>



CGrid2DScript::CGrid2DScript()
	: CScript(-1)
	, m_pMainCam(nullptr)
{
}

CGrid2DScript::~CGrid2DScript()
{
}

void CGrid2DScript::begin()
{
}

void CGrid2DScript::tick()
{
	m_pMainCam = CRenderMgr::GetInst()->GetMainCam();

	if (nullptr == m_pMainCam)
		return;

	if (PROJ_TYPE::PERSPECTIVE == m_pMainCam->GetProjType())
	{
		MeshRender()->Deactivate();
	}
	else
	{
		MeshRender()->Activate();
	}

	Vec3 vCamWorldPos = m_pMainCam->Transform()->GetRelativePos();
	float fScale = m_pMainCam->GetOrthographicScale();
	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();

	MeshRender()->GetCurMaterial()->SetScalarParam(VEC4_0, &vCamWorldPos);
	MeshRender()->GetCurMaterial()->SetScalarParam(FLOAT_0, &fScale);
	MeshRender()->GetCurMaterial()->SetScalarParam(VEC2_0, &vRenderResolution);
	MeshRender()->GetCurMaterial()->SetScalarParam(FLOAT_1, &m_fThickness);
	MeshRender()->GetCurMaterial()->SetScalarParam(FLOAT_2, &m_fGridInterval);
	MeshRender()->GetCurMaterial()->SetScalarParam(VEC4_1, &m_vColor);
}