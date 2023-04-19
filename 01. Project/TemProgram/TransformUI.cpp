#include "pch.h"
#include "TransformUI.h"

#include <Engine\CTransform.h>
#include "imgui_internal.h"

#include <Engine/CKeyMgr.h>

TransformUI::TransformUI()
	: ComponentUI("Transform", COMPONENT_TYPE::TRANSFORM)
	, m_bIgnorScale(false)
	, m_eOperation(ImGuizmo::OPERATION::TRANSLATE)
	, m_eMode(ImGuizmo::MODE::LOCAL)
	, m_bUseGizmo(true)
{
}

TransformUI::~TransformUI()
{
}

void TransformUI::update()
{
	if (nullptr != GetTarget())
	{
		CGameObject* Temp = GetTarget();
		m_vPos = GetTarget()->Transform()->GetRelativePos();
		m_vScale = GetTarget()->Transform()->GetRelativeScale();
		m_vRot = GetTarget()->Transform()->GetRelativeRotation();
		m_bIgnorScale = GetTarget()->Transform()->GetIgnoreParentScale();
		m_bTurnY180 = GetTarget()->Transform()->GetTurnY180();
	}
}


#include <Engine/CCamera.h>
#include <Engine/CRenderMgr.h>
void TransformUI::render_update()
{
	// Ÿ��Ʋ ��ư�� ������ֱ� ���� ȣ�����־�� �Ѵ�.
	ComponentUI::render_update();

	// ���ڷ� Vec3�� ���޴µ�, Vec3���ο� float 3���� �ٲ�� ������ ����� ����� �ξ ����� �����ϴ�.
	ImGui::Text("Position");	ImGui::SameLine();	ImGui::DragFloat3("##Position", m_vPos);
	ImGui::Text("Scale   ");	ImGui::SameLine();	ImGui::DragFloat3("##Scale", m_vScale);

	m_vRot.ToDegree(); // ���� ������ �츮�� Rotation������ ȣ�������� ��ȯ�� �����ش�.
	ImGui::Text("Rotation");	ImGui::SameLine();	ImGui::DragFloat3("##Rotation", m_vRot);

	ImGui::Text("Ignore Parent Scale");	ImGui::SameLine();	ImGui::Checkbox("##IgnorParentScale", &m_bIgnorScale);
	ImGui::Text("Turn Y-Axis 180");	ImGui::SameLine();	ImGui::Checkbox("##TurnY-Axis180", &m_bTurnY180);

	if (GetTarget())
	{
		// �츮�� ���� ������� �츮�� UI�� ���� �Է��� ��ȯ�� ���� ���Ƿ� �ش� ���� �־��ִ� ������ ������ ��ġ���� �������.
		GetTarget()->Transform()->SetRelativePos(m_vPos);
		GetTarget()->Transform()->SetRelativeScale(m_vScale);

		m_vRot.ToRadian(); // ȣ�������� �Էµ� ������ �������� �ٲپ� �Է����ش�.
		GetTarget()->Transform()->SetRelativeRotation(m_vRot);

		GetTarget()->Transform()->SetIgnoreParentScale(m_bIgnorScale);
		GetTarget()->Transform()->SetTurnY180(m_bTurnY180);
	}

	ImGui::Checkbox("Use-Gizmo", &m_bUseGizmo);

	if(m_bUseGizmo)
		OperateGizmo3D();
}

void TransformUI::OperateGizmo3D()
{
	ImGui::Text("==Operation Setting==");
	if (ImGui::RadioButton("Pos", m_eOperation == ImGuizmo::OPERATION::TRANSLATE))
	{
		m_eOperation = ImGuizmo::OPERATION::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", m_eOperation == ImGuizmo::OPERATION::SCALE))
	{
		m_eOperation = ImGuizmo::OPERATION::SCALE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Rot", m_eOperation == ImGuizmo::OPERATION::ROTATE))
	{
		m_eOperation = ImGuizmo::OPERATION::ROTATE;
	}

	ImGui::Text("==Mode Setting==");
	if (ImGui::RadioButton("Local", m_eMode == ImGuizmo::MODE::LOCAL))
	{
		m_eMode = ImGuizmo::MODE::LOCAL;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("World", m_eMode == ImGuizmo::MODE::WORLD))
	{
		m_eMode = ImGuizmo::MODE::WORLD;
	}

	Matrix MatWorld = GetTarget()->Transform()->GetWorldMat();
	const Matrix& MatView = CRenderMgr::GetInst()->GetMainCam()->GetViewMat();
	const Matrix& MatProj = CRenderMgr::GetInst()->GetMainCam()->GetProjMat();
	static float fSnap = 0.1f;

	Vec3 vCamWorldPos = CRenderMgr::GetInst()->GetMainCam()->Transform()->GetWorldPos();
	Vec3 vObjWorldPos = GetTarget()->Transform()->GetWorldPos();
	float fDistCamObj = (vCamWorldPos - vObjWorldPos).Length();

	ImGuizmo::BeginFrame();
	ImGuizmo::SetDrawlist();
	ImGuizmo::AllowAxisFlip(false);
	ImGuizmo::SetGizmoSizeClipSpace(10.f / fDistCamObj);
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 vRectStart = ImGui::GetMainViewport()->Pos;
	ImGuizmo::SetRect(vRectStart.x, vRectStart.y, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(&MatView.m[0][0], &MatProj.m[0][0], m_eOperation, m_eMode, &MatWorld.m[0][0]
	, nullptr, &fSnap);

	XMVECTOR matTrans, matRot, matScale;

	CGameObject* pParent = GetTarget()->GetParent();

	if (GetTarget()->Transform()->GetIgnoreParentScale())
	{
		while (pParent)
		{
			const Matrix& vParentTransMatInv = pParent->Transform()->GetTransMatInv();
			const Matrix& vParentRotMatInv = pParent->Transform()->GetRotMatInv();

			Matrix matInv = vParentTransMatInv * vParentRotMatInv;
			MatWorld *= matInv;
			pParent = pParent->GetParent();
		}
	}
	else
	{
		if (pParent)
		{
			MatWorld *= pParent->Transform()->GetWorldMatInv();
		}
	}

	// matRot is Quaternion
	XMMatrixDecompose(&matScale, &matRot, &matTrans, MatWorld);

	Vec3 vMatScale = Vec3(matScale);
	Vec3 vMatPos = Vec3(matTrans);
	Vec3 vMatRot;
	QuaternionToEuler(matRot, vMatRot);

	if (GetTarget() && ImGuizmo::IsUsing())
	{
		// Matrix calc error exception handling
		Vec3 vPrevScale = GetTarget()->Transform()->GetRelativeScale();
		Vec3 vPrevPos = GetTarget()->Transform()->GetRelativePos();
		Vec3 vPrevRot = GetTarget()->Transform()->GetRelativeRotation();

		IgnoreMinuteDiff(vPrevScale, vMatScale);
		IgnoreMinuteDiff(vPrevPos, vMatPos);
		IgnoreMinuteDiff(vPrevRot, vMatRot);

		GetTarget()->Transform()->SetRelativePos(vMatPos);
		GetTarget()->Transform()->SetRelativeScale(vMatScale);
		GetTarget()->Transform()->SetRelativeRotation(vMatRot);
	}
	ImGuizmo::EndFrame();
}

void TransformUI::IgnoreMinuteDiff(Vec3 _vPrev, Vec3& _vOut, float _fIgnoreDiff)
{
	Vec3 vDifference = _vPrev - _vOut;
	if (fabs(vDifference.x) < _fIgnoreDiff)
	{
		_vOut.x = _vPrev.x;
	}
	if (fabs(vDifference.y) < _fIgnoreDiff)
	{
		_vOut.y = _vPrev.y;
	}
	if (fabs(vDifference.z) < _fIgnoreDiff)
	{
		_vOut.z = _vPrev.z;
	}
}
// Grid
// ImGuizmo::DrawGrid(&view.m[0][0], &proj.m[0][0], &identity.m[0][0], 20.f);



