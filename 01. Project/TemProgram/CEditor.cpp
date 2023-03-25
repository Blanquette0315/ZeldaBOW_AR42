#include "pch.h"
#include "CEditor.h"

#include "imgui.h"
#include "CGameObjectEX.h"
#include <Engine\CTransform.h>
#include <Engine\CMeshRender.h>
#include "CGrid2DScript.h"
#include <Engine\CCamera.h>

#include <Engine\CRenderMgr.h>

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>

#include "CCameraScript.h"
#include "CEditorCam.h"

CEditor::CEditor()
	: m_DebugDrawObject{}
{
}

CEditor::~CEditor()
{
	Safe_Del_Vec(m_vecEditorObj);
	Safe_Del_Array(m_DebugDrawObject);
}

void CEditor::init()
{
	CreateDebugDrawObject();

	// Editor �뵵 Grid Object �߰�
	/*CGameObjectEX* pGridObj = new CGameObjectEX;
	pGridObj->SetName(L"Grid Object");

	pGridObj->AddComponent(new CTransform);
	pGridObj->AddComponent(new CMeshRender);
	pGridObj->AddComponent(new CGrid2DScript);

	pGridObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	pGridObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"GridMtrl"));

	pGridObj->GetScript<CGrid2DScript>()->SetGridColor(Vec4(0.2f, 0.9f, 0.2f, 1.f));
	pGridObj->GetScript<CGrid2DScript>()->SetGridInterval(100.f);
	pGridObj->GetScript<CGrid2DScript>()->SetThickness(2.f);

	m_vecEditorObj.push_back(pGridObj);*/

	// Editor �뵵 Camera Object �߰�
	CGameObjectEX* pEditorCam = new CGameObjectEX;
	pEditorCam->SetName(L"Editor Camera");

	pEditorCam->AddComponent(new CTransform);
	pEditorCam->AddComponent(new CEditorCam);
	pEditorCam->AddComponent(new CCameraScript);

	pEditorCam->Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
	pEditorCam->Camera()->SetFar(10000.f);
	pEditorCam->Camera()->SetLayerMaskAll();
	pEditorCam->Camera()->SetLayerMask(31); // UI�� ����ŷ ó����.

	m_vecEditorObj.push_back(pEditorCam);
	CRenderMgr::GetInst()->RegisterEditorCamera(pEditorCam->Camera());
}

void CEditor::progress()
{
	// ������ �÷��� ������ ���� �������� tick�� ���� �ȵȴ�.
	if (LEVEL_STATE::PLAY != CLevelMgr::GetInst()->GetCurLevel()->GetState())
	{
		tick();
		render();
	}

	// Debug Shape
	render_debug();
}

void CEditor::tick()
{
	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->tick();
	}

	for (size_t i = 0; i < m_vecEditorObj.size(); i++)
	{
		m_vecEditorObj[i]->finaltick();
	}
}

void CEditor::render()
{
	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->render();
	}
}

void CEditor::render_debug()
{
	// DebugDrawRender
	// ������ list���ִ� ��ü�� �׸��� ���ε��� ���� �׷��ֵ��� ���� ������
	// �ݴ�� �Ұ�� ���ε��� ��ü�� �׸���, �����ð��� �ִٸ�, ���� �߰����� ��, �ٽ� �ѹ� �� �׸��� �����̴�.
	// ���� �ð����� ������ �Ǵ� Shape
	list<tDebugShapeInfo>::iterator iter = m_DebugDrawList.begin();
	for (; iter != m_DebugDrawList.end();)
	{
		iter->fCurtime += FDT;
		if (iter->fDuration < iter->fCurtime)
		{
			iter = m_DebugDrawList.erase(iter);
		}
		else
		{
			DebugDraw(*iter);
			++iter;
		}
	}

	// ���� �߰��� DebugShape Ȯ��
	vector<tDebugShapeInfo>& vecinfo = CRenderMgr::GetInst()->GetDebutgDrawInfo();

	for (size_t i = 0; i < vecinfo.size(); ++i)
	{
		DebugDraw(vecinfo[i]);

		if (0.f < vecinfo[i].fDuration)
		{
			m_DebugDrawList.push_back(vecinfo[i]);
		}
	}
	vecinfo.clear();
}

void CEditor::CreateDebugDrawObject()
{
	CGameObjectEX* pDebugObj = nullptr;

	// DEBUG_SHAPE::RECT
	pDebugObj = new CGameObjectEX;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh_Debug"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::RECT] = pDebugObj;

	// DEBUG_SHAPE::CIRCLE
	pDebugObj = new CGameObjectEX;
	
	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh_Debug"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::CIRCLE] = pDebugObj;


	// DEBUG_SHAPE::CUBE
	pDebugObj = new CGameObjectEX;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::CUBE] = pDebugObj;

	// DEBUG_SHAPE::SPHERE
	pDebugObj = new CGameObjectEX;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::SPHERE] = pDebugObj;

	// DEBUG_SHAPE::CONE
	pDebugObj = new CGameObjectEX;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"ConeMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::CONE] = pDebugObj;

	// DEBUG_SHAPE::FRUSTUM
	pDebugObj = new CGameObjectEX;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"FrustumDebugDrawMtrl"));

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::FRUSTUM] = pDebugObj;

	// DEBUG_SHAPE::CYLINDER
	pDebugObj = new CGameObjectEX;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CylinderMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::CYLINDER] = pDebugObj;
}

void CEditor::DebugDraw(tDebugShapeInfo& _info)
{
	CGameObjectEX* pDebugObj = m_DebugDrawObject[(UINT)_info.eShape];

	pDebugObj->Transform()->SetRelativePos(_info.vPosition);
	pDebugObj->Transform()->SetRelativeRotation(_info.vRot);

	if (DEBUG_SHAPE::CIRCLE == _info.eShape || DEBUG_SHAPE::SPHERE == _info.eShape)
	{
		pDebugObj->Transform()->SetRelativeScale(Vec3(_info.fRadius * 2.f, _info.fRadius * 2.f, _info.fRadius * 2.f));
	}
	else if (DEBUG_SHAPE::RECT == _info.eShape || DEBUG_SHAPE::CUBE == _info.eShape)
	{
		pDebugObj->Transform()->SetRelativeScale(_info.vScale);
	}
	else if (DEBUG_SHAPE::CONE == _info.eShape)
	{
		float length = tan(_info.fAngle / 180 * XM_PI) * _info.fRadius;
		pDebugObj->Transform()->SetRelativeScale(Vec3(length, length, _info.fRadius));
	}
	else if (DEBUG_SHAPE::FRUSTUM == _info.eShape)
	{
		//pDebugObj->MeshRender()->GetCurMaterial()->SetScalarParam(SCALAR_PARAM::MAT_0, &pDebugObj->Camera()->GetFrustum().GetMatInv());
	}
	else if (DEBUG_SHAPE::CYLINDER == _info.eShape)
	{
		pDebugObj->Transform()->SetRelativeScale(_info.vScale);
	}

	pDebugObj->MeshRender()->GetCurMaterial()->SetScalarParam(SCALAR_PARAM::VEC4_0, _info.vColor);

	CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCam();

	pDebugObj->Transform()->finaltick();

	g_transform.matWorld = pDebugObj->Transform()->GetWorldMat();
	g_transform.matView = pMainCam->GetViewMat();
	g_transform.matProj = pMainCam->GetProjMat();

	pDebugObj->render();
}


// Simple Meth�� �ش� �Լ����� ���� �Ǿ��ְ� ���ǰ� �Ǿ����� �ʾƼ� ���Ǹ� ���־���.
void Vector3::ToDegree()
{
	x = (x / XM_PI) * 180.f;
	y = (y / XM_PI) * 180.f;
	z = (z / XM_PI) * 180.f;
}

void Vector3::ToRadian()
{
	x = (x / 180.f) * XM_PI;
	y = (y / 180.f) * XM_PI;
	z = (z / 180.f) * XM_PI;
}

// ������ �Լ� ������ ���־���.
// ������ �Լ� ������ ���� �ٸ��� ������ ���� ��. (SimpleMeth ������ �����ϴ� �͵� �ٸ���.)
Vec2::operator ImVec2() const { return ImVec2(x, y); }