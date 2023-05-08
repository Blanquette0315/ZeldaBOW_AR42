#include "pch.h"
#include "CCamera.h"

#include "CKeyMgr.h"

#include "CLevelMgr.h"
#include "CLevel.h"

#include "CDevice.h"
#include "CRenderMgr.h"
#include "CTransform.h"
#include "CRenderComponent.h"
#include "CMaterial.h"

#include "CMRT.h"

#include "CMeshRender.h"
#include "CGameObject.h"

#include "CKeyMgr.h"

CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_Frustum(this)
	, m_eProjType(PROJ_TYPE::ORTHOGRAPHICS)
	, m_fAspectRatio(1.f)
	, m_fWidth(0.f)
	, m_FOV(XM_PI / 2.f)
	, m_fNear(1.f)
	, m_fFar(2000.f)
	, m_fScale(1.f)
	, m_iLayerMask(0)
	, m_iCamIdx(0)
	, m_ray{}
{
	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();
	m_fAspectRatio = vRenderResolution.x / vRenderResolution.y;
	m_fWidth = vRenderResolution.x;
}

CCamera::~CCamera()
{
}

void CCamera::finaltick()
{
	
	CalcViewMat();

	CalcProjMat();

	// Frustum 구성
	m_Frustum.finaltick();

	CalRay();

	// Frustum DebugRender
	if (Is_ShowDebugDraw())
	{
		DebugDrawFrustum(Vec4(0.2f, 0.8f, 0.2f, 1.f), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f));
		Ptr<CMaterial> pCamMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"FrustumDebugDrawMtrl");
		pCamMtrl->SetScalarParam(SCALAR_PARAM::MAT_0, &GetFrustum().GetMatInv());
	}
}

void CCamera::finaltick_module()
{
	CalcViewMat();

	CalcProjMat();

	m_Frustum.finaltick();

	CalRay();

	// Frustum DebugRender
	if (Is_ShowDebugDraw())
	{
		DebugDrawFrustum(Vec4(0.2f, 0.8f, 0.2f, 1.f), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f));
		Ptr<CMaterial> pCamMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"FrustumDebugDrawMtrl");
		pCamMtrl->SetScalarParam(SCALAR_PARAM::MAT_0, &GetFrustum().GetMatInv());
	}
}

void CCamera::CalcViewMat()
{
	// =============
	// View 행렬 계산
	// =============
	// View 이동 행렬 ( 카메라를 원점으로 이동하는 만큼)
	Vec3 vPos = Transform()->GetRelativePos();
	Matrix matViewTrans = XMMatrixTranslation(-vPos.x, -vPos.y, -vPos.z);

	// View 회전 행렬 ( 카메라가 보는 전방 방향을 z 축을 보도록 돌리는 만큼)
	Vec3 vRight = Transform()->GetRelativeDir(DIR::RIGHT);
	Vec3 vUP = Transform()->GetRelativeDir(DIR::UP);
	Vec3 vFront = Transform()->GetRelativeDir(DIR::FRONT);

	Matrix matViewRot = XMMatrixIdentity();
	matViewRot._11 = vRight.x; matViewRot._12 = vUP.x; matViewRot._13 = vFront.x;
	matViewRot._21 = vRight.y; matViewRot._22 = vUP.y; matViewRot._23 = vFront.y;
	matViewRot._31 = vRight.z; matViewRot._32 = vUP.z; matViewRot._33 = vFront.z;

	m_matView = matViewTrans * matViewRot;
	m_matViewInv = XMMatrixInverse(nullptr, m_matView);
}

void CCamera::CalcProjMat()
{
	// =============
	// 투영 행렬 계산
	// =============

	if (PROJ_TYPE::PERSPECTIVE == m_eProjType)
	{
		// 원근 투영
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_fAspectRatio, m_fNear, m_fFar);
	}
	else
	{
		// 직교 투영
		m_matProj = XMMatrixOrthographicLH(m_fWidth * m_fScale, m_fWidth / m_fAspectRatio * m_fScale, 1.f, m_fFar);
	}

	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);
}

void CCamera::CalRay()
{
	// 마우스 방향을 향하는 Ray 구하기
	// SwapChain 타겟의 ViewPort 정보
	CMRT* pMRT = CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN);
	D3D11_VIEWPORT tVP = pMRT->GetViewPort();

	//  현재 마우스 좌표
	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

	// 직선은 카메라의 좌표를 반드시 지난다.
	m_ray.vStart = Transform()->GetWorldPos();

	// view space 에서의 방향
	m_ray.vDir.x = ((((vMousePos.x - tVP.TopLeftX) * 2.f / tVP.Width) - 1.f) - m_matProj._31) / m_matProj._11;
	m_ray.vDir.y = (-(((vMousePos.y - tVP.TopLeftY) * 2.f / tVP.Height) - 1.f) - m_matProj._32) / m_matProj._22;
	m_ray.vDir.z = 1.f;

	// world space 에서의 방향
	m_ray.vDir = XMVector3TransformNormal(m_ray.vDir, m_matViewInv);
	m_ray.vDir.Normalize();
}

void CCamera::render()
{
	// 행렬 셋팅
	g_transform.matView = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj = m_matProj;

	// Shader Domain에 따른 물체 분류
	SortObject();

	// Domain 분류에 따른 렌더 호출
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->OMSet();
	render_deferred();

	render_deferred_transparent();

	// Deferred Decal 처리
	render_deferreddecal();

	// Deferred 광원 처리
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet();
	const vector<CLight3D*>& vecLight3D = CRenderMgr::GetInst()->GetLight3D();
	
	for (size_t i = 0; i < vecLight3D.size(); ++i)
	{
		vecLight3D[i]->render();
	}

	// Bloom Effect
	// render_Bloom();

	// SwapChainMRT로 변경
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

	// DeferredMrt -> SwapChainMRT로 병합
	static Ptr<CMaterial> pMergeMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Deferred_MergeMtrl");
	static Ptr<CMesh> pMergeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	pMergeMtrl->UpdateData();
	pMergeMesh->render();

	// Forward Rendering
	render_opaque();
	render_mask();

	render_decal();

	render_transparent();
	render_postprocess();
}

void CCamera::SetLayerMask(const wstring& _strLayerName)
{
	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
	CLayer* pLayer = pCurLevel->GetLayer(_strLayerName);
	assert(pLayer);

	SetLayerMask(pLayer->GetLayerIdx());
}

void CCamera::SetLayerMask(int _iLayerIdx)
{
	if (m_iLayerMask & (1 << _iLayerIdx))
	{
		m_iLayerMask &= ~(1 << _iLayerIdx);
	}
	else
	{
		m_iLayerMask |= (1 << _iLayerIdx);
	}
}

void CCamera::SetLayerVisible(int _iLayerIdx)
{
	m_iLayerMask |= (1 << _iLayerIdx);
}

void CCamera::SetLayerInvisible(int _iLayerIdx)
{
	m_iLayerMask &= ~(1 << _iLayerIdx);
}

void CCamera::SortObject()
{
	m_vecDeferred.clear();
	m_vecDeferredTransparent.clear();
	m_vecDeferredDecal.clear();
	m_vecQpaque.clear();
	m_vecMask.clear();
	m_vecDecal.clear();
	m_vecTransparent.clear();
	m_vecPostProcess.clear();

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// Layer확인
		if (m_iLayerMask & (1 << i))

		{
			// 해당 레이어에 속한 게임 오브젝트를 가져온다.
			CLayer* pLayer = pCurLevel->GetLayer(i);

			// 오브젝트들을 셰이더 도메인에 따라 분류한다.
			const vector<CGameObject*>& vecObj = pLayer->GetObjects();
			for (size_t j = 0; j < vecObj.size(); ++j)
			{
				CRenderComponent* pRenderCom = vecObj[j]->GetRenderComponent();

				if (nullptr == pRenderCom
					|| nullptr == pRenderCom->GetMesh()
					|| nullptr == pRenderCom->GetCurMaterial()
					|| nullptr == pRenderCom->GetCurMaterial()->GetShader())
				{
					continue;
				}

				// Frustum Culling 실패 시
				if (vecObj[j]->IS_FrustumCul())
				{
					//if (!m_Frustum.CheckFrustum(vecObj[j]->Transform()->GetWorldPos()))
					if (!m_Frustum.CheckFrustumRadius(vecObj[j]->Transform()->GetWorldPos(), vecObj[j]->Transform()->GetWorldScale().x * 0.5f + 200.f))
					{
						continue;
					}
				}
				// pushback for picking
				CFrustum::PushBackInFrustumObjs(vecObj[j]);

				Ptr<CGraphicsShader> pShader = pRenderCom->GetCurMaterial()->GetShader();

				SHADER_DOMAIN eDomain = pShader->GetDomain();

				switch (eDomain)
				{
				case SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQUE:
				case SHADER_DOMAIN::DOMAIN_DEFERRED_MASK:
					m_vecDeferred.push_back(vecObj[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_DEFERRED_TRANSPARENT:
					m_vecDeferredTransparent.push_back(vecObj[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL:
					m_vecDeferredDecal.push_back(vecObj[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_OPAQUE:
					m_vecQpaque.push_back(vecObj[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_MASK:
					m_vecMask.push_back(vecObj[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_DECAL:
					m_vecDecal.push_back(vecObj[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
					m_vecTransparent.push_back(vecObj[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_POST_PROCESS:
					m_vecPostProcess.push_back(vecObj[j]);
					break;
				}
			}
		}
	}
}

void CCamera::SortShadowObject()
{
	m_vecDynamicShadow.clear();

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurLevel->GetLayer(i);
		const vector<CGameObject*>& vecObj = pLayer->GetObjects();

		for (size_t j = 0; j < vecObj.size(); ++j)
		{
			CRenderComponent* pRenderCom = vecObj[j]->GetRenderComponent();

			if (vecObj[j]->IS_FrustumCul())
			{
				//if (!m_Frustum.CheckFrustum(vecObj[j]->Transform()->GetWorldPos()))
				if (vecObj[j]->Transform() != nullptr)
				{
					if (!m_Frustum.CheckFrustumRadius(vecObj[j]->Transform()->GetWorldPos(), vecObj[j]->Transform()->GetWorldScale().x * 0.5f + 200.f))
					{
						continue;
					}
				}
			}

			if (pRenderCom && pRenderCom->IsDynamicShadow())
			{
				m_vecDynamicShadow.push_back(vecObj[j]);
			}
		}
	}
}

void CCamera::render_deferred()
{
	for (size_t i = 0; i < m_vecDeferred.size(); ++i)
	{
		m_vecDeferred[i]->render();
	}
}

void CCamera::render_deferred_transparent()
{
	for (size_t i = 0; i < m_vecDeferredTransparent.size(); ++i)
	{
		m_vecDeferredTransparent[i]->render();
	}
}

void CCamera::render_deferreddecal()
{
	for (size_t i = 0; i < m_vecDeferredDecal.size(); ++i)
	{
		m_vecDeferredDecal[i]->render();
	}
}

void CCamera::render_Bloom()
{
	// Bright
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::BRIGHT)->OMSet();
	static Ptr<CMaterial> pBrightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"BrightMtrl");
	static Ptr<CMesh> pBrightMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	pBrightMtrl->UpdateData();
	pBrightMesh->render();

	// Bloom UpScale
	CONTEXT->GenerateMips(CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseBrightTargetTex")->GetSRV().Get());
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::BLOOM_UPSCALING)->OMSet();
	static Ptr<CMaterial> pBloomBlurMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"BloomUpScaleMtrl");
	pBloomBlurMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseBrightTargetTex"));
	static Ptr<CMesh> pBloomBlurMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	pBloomBlurMtrl->UpdateData();
	pBloomBlurMesh->render();

	// AlphaBloomMtrl
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::BLOOM_MARGE)->OMSet();
	static Ptr<CMaterial> pBloomMargeMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"AlphaBloomMtrl");
	static Ptr<CMesh> pBloomMargeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	pBloomMargeMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"BloomUpScaleTargetTex0"));
	pBloomMargeMtrl->UpdateData();
	pBloomMargeMesh->render();

	pBloomMargeMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"BloomUpScaleTargetTex1"));
	pBloomMargeMtrl->UpdateData();
	pBloomMargeMesh->render();

	pBloomMargeMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"BloomUpScaleTargetTex2"));
	pBloomMargeMtrl->UpdateData();
	pBloomMargeMesh->render();

	pBloomMargeMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"BloomUpScaleTargetTex3"));
	pBloomMargeMtrl->UpdateData();
	pBloomMargeMesh->render();

	pBloomMargeMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"BloomUpScaleTargetTex4"));
	pBloomMargeMtrl->UpdateData();
	pBloomMargeMesh->render();

	pBloomMargeMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"BloomUpScaleTargetTex5"));
	pBloomMargeMtrl->UpdateData();
	pBloomMargeMesh->render();

	// Bloom
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::BLOOM)->OMSet();
	static Ptr<CMaterial> pBloomMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"BloomMtrl");
	static Ptr<CMesh> pBloomMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	pBloomMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex"));
	pBloomMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"BloomMargeTargetTex"));
	pBloomMtrl->UpdateData();
	pBloomMesh->render();
}

void CCamera::render_opaque()
{
	for (size_t i = 0; i < m_vecQpaque.size(); ++i)
	{
		m_vecQpaque[i]->render();
	}
}

void CCamera::render_mask()
{
	for (size_t i = 0; i < m_vecMask.size(); ++i)
	{
		m_vecMask[i]->render();
	}
}

void CCamera::render_decal()
{
	for (size_t i = 0; i < m_vecDecal.size(); ++i)
	{
		m_vecDecal[i]->render();
	}
}

void CCamera::render_transparent()
{
	for (size_t i = 0; i < m_vecTransparent.size(); ++i)
	{
		m_vecTransparent[i]->render();
	}
}

void CCamera::render_postprocess()
{
	for (size_t i = 0; i < m_vecPostProcess.size(); ++i)
	{
		CRenderMgr::GetInst()->CopyRenderTarget();
		m_vecPostProcess[i]->render();
	}
}

void CCamera::render_depthmap()
{
	// 광원 카메라의 View, Proj 세팅
	g_transform.matView = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj = m_matProj;

	for (size_t i = 0; i < m_vecDynamicShadow.size(); ++i)
	{
		m_vecDynamicShadow[i]->GetRenderComponent()->render_depthmap();
	}
}

void CCamera::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "CAMERA";
	_emitter << YAML::Value << YAML::BeginMap;

	_emitter << YAML::Key << "ProjType";
	_emitter << YAML::Value << (UINT)m_eProjType;
	_emitter << YAML::Key << "Far";
	_emitter << YAML::Value << m_fFar;
	_emitter << YAML::Key << "FieldOfView";
	_emitter << YAML::Value << m_FOV;
	_emitter << YAML::Key << "Scale";
	_emitter << YAML::Value << m_fScale;
	_emitter << YAML::Key << "LayerMask";
	_emitter << YAML::Value << m_iLayerMask;
	_emitter << YAML::Key << "CamIdx";
	_emitter << YAML::Value << m_iCamIdx;

	_emitter << YAML::EndMap;
}

void CCamera::LoadFromYAML(YAML::Node& _node)
{
	m_eProjType = (PROJ_TYPE)(_node["CAMERA"]["ProjType"].as<UINT>());
	m_fFar = _node["CAMERA"]["Far"].as<float>();
	m_FOV = _node["CAMERA"]["FieldOfView"].as<float>();
	m_fScale = _node["CAMERA"]["Scale"].as<float>();
	m_iLayerMask = _node["CAMERA"]["LayerMask"].as<UINT>();
	m_iCamIdx = _node["CAMERA"]["CamIdx"].as<int>();
}