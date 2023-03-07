#include "pch.h"
#include "CCamera.h"

#include "CLevelMgr.h"
#include "CLevel.h"

#include "CDevice.h"
#include "CRenderMgr.h"
#include "CTransform.h"
#include "CRenderComponent.h"
#include "CMaterial.h"

#include "CMRT.h"

#include "CGameObject.h"

CCamera::CCamera()
	: CComponent(COMPONENT_TYPE::CAMERA)
	, m_Frustum(this)
	, m_bFrustumRender(false)
	, m_eProjType(PROJ_TYPE::ORTHOGRAPHICS)
	, m_fAspectRatio(1.f)
	, m_FOV(XM_PI / 2.f)
	, m_fFar(2000.f)
	, m_fScale(1.f)
	, m_iLayerMask(0)
	, m_iCamIdx(0)
{
	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();
	m_fAspectRatio = vRenderResolution.x / vRenderResolution.y;
}

CCamera::~CCamera()
{
}

void CCamera::finaltick()
{
	
	CalcViewMat();

	CalcProjMat();

	// Frustum ����
	m_Frustum.finaltick();

	// Frustum DebugRender
	if (m_bFrustumRender)
	{
		DebugDrawFrustum(Vec4(0.2f, 0.8f, 0.2f, 1.f), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f));
		Ptr<CMaterial> pCamMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"FrustumDebugDrawMtrl");
		pCamMtrl->SetScalarParam(SCALAR_PARAM::MAT_0, &GetFrustum().GetMatInv());
	}

	// ī�޶� ���
	CRenderMgr::GetInst()->RegisterCamera(this);
}

void CCamera::CalcViewMat()
{
	// =============
	// View ��� ���
	// =============
	// View �̵� ��� ( ī�޶� �������� �̵��ϴ� ��ŭ)
	Vec3 vPos = Transform()->GetRelativePos();
	Matrix matViewTrans = XMMatrixTranslation(-vPos.x, -vPos.y, -vPos.z);

	// View ȸ�� ��� ( ī�޶� ���� ���� ������ z ���� ������ ������ ��ŭ)
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
	// ���� ��� ���
	// =============

	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();

	if (PROJ_TYPE::PERSPECTIVE == m_eProjType)
	{
		// ���� ����
		m_matProj = XMMatrixPerspectiveFovLH(m_FOV, m_fAspectRatio, 1.f, m_fFar);
	}
	else
	{
		// ���� ����
		m_matProj = XMMatrixOrthographicLH(vRenderResolution.x * m_fScale, vRenderResolution.y * m_fScale, 1.f, m_fFar);
	}

	m_matProjInv = XMMatrixInverse(nullptr, m_matProj);
}

void CCamera::render()
{
	// ��� ����
	g_transform.matView = m_matView;
	g_transform.matViewInv = m_matViewInv;
	g_transform.matProj = m_matProj;

	// Shader Domain�� ���� ��ü �з�
	SortObject();

	// Domain �з��� ���� ���� ȣ��
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::DEFERRED)->OMSet();
	render_deferred();

	// Deferred Decal ó��
	render_deferreddecal();

	// Deferred ���� ó��
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::LIGHT)->OMSet();
	const vector<CLight3D*>& vecLight3D = CRenderMgr::GetInst()->GetLight3D();
	
	for (size_t i = 0; i < vecLight3D.size(); ++i)
	{
		vecLight3D[i]->render();
	}

	// SwapChainMRT�� ����
	CRenderMgr::GetInst()->GetMRT(MRT_TYPE::SWAPCHAIN)->OMSet();

	// DeferredMrt -> SwapChainMRT�� ����
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

void CCamera::SortObject()
{
	m_vecDeferred.clear();
	m_vecDeferredDecal.clear();
	m_vecQpaque.clear();
	m_vecMask.clear();
	m_vecDecal.clear();
	m_vecTransparent.clear();
	m_vecPostProcess.clear();

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// LayerȮ��
		if (m_iLayerMask & (1 << i))

		{
			// �ش� ���̾ ���� ���� ������Ʈ�� �����´�.
			CLayer* pLayer = pCurLevel->GetLayer(i);

			// ������Ʈ���� ���̴� �����ο� ���� �з��Ѵ�.
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

				// Frustum Culling ���� ��
				if (!vecObj[j]->IS_FrustumCul())
				{
					//if (!m_Frustum.CheckFrustum(vecObj[j]->Transform()->GetWorldPos()))
					if (!m_Frustum.CheckFrustumRadius(vecObj[j]->Transform()->GetWorldPos(), vecObj[j]->Transform()->GetWorldScale().x * 0.5f + 200.f))
						continue;
				}

				Ptr<CGrapicsShader> pShader = pRenderCom->GetCurMaterial()->GetShader();

				SHADER_DOMAIN eDomain = pShader->GetDomain();

				switch (eDomain)
				{
				case SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQIE:
				case SHADER_DOMAIN::DOMAIN_DEFERRED_MASK:
					m_vecDeferred.push_back(vecObj[j]);
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

void CCamera::render_deferred()
{
	for (size_t i = 0; i < m_vecDeferred.size(); ++i)
	{

		m_vecDeferred[i]->render();
	}
}

void CCamera::render_deferreddecal()
{
	for (size_t i = 0; i < m_vecDeferredDecal.size(); ++i)
	{
		m_vecDeferredDecal[i]->render();
	}
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