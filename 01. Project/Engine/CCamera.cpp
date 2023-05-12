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
#include "CInstancingBuffer.h"
#include "CAnimator3D.h"

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

	render_deferreddecal();

	// Deferred 광원 처리
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet();
	const vector<CLight3D*>& vecLight3D = CRenderMgr::GetInst()->GetLight3D();
	
	for (size_t i = 0; i < vecLight3D.size(); ++i)
	{
		vecLight3D[i]->render();
	}

	// Bloom Effect
	//render_Bloom();

	// SwapChainMRT로 변경
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

	// DeferredMrt -> SwapChainMRT로 병합
	static Ptr<CMaterial> pMergeMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Deferred_MergeMtrl");
	static Ptr<CMesh> pMergeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
	pMergeMtrl->UpdateData();
	pMergeMesh->render();

	// Forward Rendering
	render_Forward();

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
	for (auto& pair : m_mapInstGroup_D)
		pair.second.clear();
	for (auto& pair : m_mapInstGroup_F)
		pair.second.clear();

	m_vecDeferredDecal.clear();
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
					if (!m_Frustum.CheckFrustumRadius(vecObj[j]->Transform()->GetWorldPos(), vecObj[j]->Transform()->GetWorldScale().x * 0.5f + 20.f))
					{
						continue;
					}
				}
				// pushback for picking
				CFrustum::PushBackInFrustumObjs(vecObj[j]);


				// 메테리얼 개수만큼 반복
				UINT iMtrlCount = pRenderCom->GetMtrlCount();

				for (UINT iMtrl = 0; iMtrl < iMtrlCount; ++iMtrl)
				{
					if (nullptr != pRenderCom->GetCurMaterial(iMtrl) || nullptr != pRenderCom->GetCurMaterial(iMtrl)->GetShader())
					{
						Ptr<CGraphicsShader> pShader = pRenderCom->GetCurMaterial(iMtrl)->GetShader();

						SHADER_DOMAIN eDomain = pShader->GetDomain();

						switch (eDomain)
						{
						case SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQUE:
						case SHADER_DOMAIN::DOMAIN_DEFERRED_MASK:
						case SHADER_DOMAIN::DOMAIN_DEFERRED_TRANSPARENT:
						case SHADER_DOMAIN::DOMAIN_OPAQUE:
						case SHADER_DOMAIN::DOMAIN_MASK:
						{
							// Shader 의 POV 에 따라서 인스턴싱 그룹을 분류한다.
							map<ULONG64, vector<tInstObj>>* pMap = NULL;
							Ptr<CMaterial> pMtrl = pRenderCom->GetCurMaterial(iMtrl);

							if (pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQUE
								|| pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED_MASK
								|| pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_DEFERRED_TRANSPARENT)
							{
								pMap = &m_mapInstGroup_D;
							}
							else if (pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_OPAQUE
								|| pShader->GetDomain() == SHADER_DOMAIN::DOMAIN_MASK)
							{
								pMap = &m_mapInstGroup_F;
							}
							else
							{
								assert(nullptr);
								continue;
							}

							uInstID uID = {};
							uID.llID = pRenderCom->GetInstID(iMtrl);

							// ID 가 0 다 ==> Mesh 나 Material 이 셋팅되지 않았다.
							if (0 == uID.llID)
								continue;

							map<ULONG64, vector<tInstObj>>::iterator iter = pMap->find(uID.llID);
							if (iter == pMap->end())
							{
								pMap->insert(make_pair(uID.llID, vector<tInstObj>{tInstObj{ vecObj[j], iMtrl }}));
							}
							else
							{
								iter->second.push_back(tInstObj{ vecObj[j], iMtrl });
							}
						}
						break;
						case SHADER_DOMAIN::DOMAIN_DEFERRED_DECAL:
							m_vecDeferredDecal.push_back(vecObj[j]);
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
					if (!m_Frustum.CheckFrustumRadius(vecObj[j]->Transform()->GetWorldPos(), vecObj[j]->Transform()->GetWorldScale().x * 0.5f + 20.f))
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
	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	// Deferred object render
	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_D)
	{
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second.empty())
			continue;

		// instancing 개수 조건 미만이거나
		// Animation2D 오브젝트거나(스프라이트 애니메이션 오브젝트)
		// Shader 가 Instancing 을 지원하지 않는경우
		if (pair.second.size() <= 1
			|| pair.second[0].pObj->Animator2D()
			|| pair.second[0].pObj->GetRenderComponent()->GetCurMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetVSInst() == nullptr)
		{
			// 해당 물체들은 단일 랜더링으로 전환
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		Ptr<CMesh> pMesh = pObj->GetRenderComponent()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->GetRenderComponent()->GetCurMaterial(pair.second[0].iMtrlIdx);

		// Instancing 버퍼 클리어
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second[i].pObj->Animator3D())
			{
				pair.second[i].pObj->Animator3D()->UpdateData();
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
			}
			else
				tInstData.iRowIdx = -1;

			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// 인스턴싱에 필요한 데이터를 세팅(SysMem -> GPU Mem)
		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(true); // Animation Mesh 알리기
			pMtrl->SetBoneCount(pMesh->GetBoneCount());
		}

		pMtrl->UpdateData_Inst();
		pMesh->render_instancing(pair.second[0].iMtrlIdx);

		// 정리
		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(false); // Animation Mesh 알리기
			pMtrl->SetBoneCount(0);
		}
	}

	// 개별 랜더링
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		pair.second[0].pObj->Transform()->UpdateData();

		for (auto& instObj : pair.second)
		{
			instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx);
		}

		if (pair.second[0].pObj->Animator3D())
		{
			pair.second[0].pObj->Animator3D()->ClearData();
		}
	}
}

void CCamera::render_deferred_transparent()
{
	/*for (size_t i = 0; i < m_vecDeferredTransparent.size(); ++i)
	{
		m_vecDeferredTransparent[i]->render();
	}*/
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

void CCamera::render_Forward()
{
	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	// Deferred object render
	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_F)
	{
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second.empty())
			continue;

		// instancing 개수 조건 미만이거나
		// Animation2D 오브젝트거나(스프라이트 애니메이션 오브젝트)
		// Shader 가 Instancing 을 지원하지 않는경우
		if (pair.second.size() <= 1
			|| pair.second[0].pObj->Animator2D()
			|| pair.second[0].pObj->GetRenderComponent()->GetCurMaterial(pair.second[0].iMtrlIdx)->GetShader()->GetVSInst() == nullptr)
		{
			// 해당 물체들은 단일 랜더링으로 전환
			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second[i].pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second[i]);
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second[i].pObj, vector<tInstObj>{pair.second[i]}));
				}
			}
			continue;
		}

		CGameObject* pObj = pair.second[0].pObj;
		Ptr<CMesh> pMesh = pObj->GetRenderComponent()->GetMesh();
		Ptr<CMaterial> pMtrl = pObj->GetRenderComponent()->GetCurMaterial(pair.second[0].iMtrlIdx);

		// Instancing 버퍼 클리어
		CInstancingBuffer::GetInst()->Clear();

		int iRowIdx = 0;
		bool bHasAnim3D = false;
		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			tInstData.matWorld = pair.second[i].pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second[i].pObj->Animator3D())
			{
				pair.second[i].pObj->Animator3D()->UpdateData();
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second[i].pObj->Animator3D()->GetFinalBoneMat());
				bHasAnim3D = true;
			}
			else
				tInstData.iRowIdx = -1;

			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		// 인스턴싱에 필요한 데이터를 세팅(SysMem -> GPU Mem)
		CInstancingBuffer::GetInst()->SetData();

		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(true); // Animation Mesh 알리기
			pMtrl->SetBoneCount(pMesh->GetBoneCount());
		}

		pMtrl->UpdateData_Inst();
		pMesh->render_instancing(pair.second[0].iMtrlIdx);

		// 정리
		if (bHasAnim3D)
		{
			pMtrl->SetAnim3D(false); // Animation Mesh 알리기
			pMtrl->SetBoneCount(0);
		}
	}

	// 개별 랜더링
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		pair.second[0].pObj->Transform()->UpdateData();

		for (auto& instObj : pair.second)
		{
			instObj.pObj->GetRenderComponent()->render(instObj.iMtrlIdx);
		}

		if (pair.second[0].pObj->Animator3D())
		{
			pair.second[0].pObj->Animator3D()->ClearData();
		}
	}
}

void CCamera::render_opaque()
{
	/*for (size_t i = 0; i < m_vecQpaque.size(); ++i)
	{
		m_vecQpaque[i]->render();
	}*/
}

void CCamera::render_mask()
{
	/*for (size_t i = 0; i < m_vecMask.size(); ++i)
	{
		m_vecMask[i]->render();
	}*/
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