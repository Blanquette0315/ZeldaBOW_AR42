#include "pch.h"
#include "CLinkScript.h"

#include <Engine/CAnimator3D.h>
#include <Engine/CAnimation3D.h>
#include <Engine/CMeshRender.h>
#include <Engine/CMesh.h>
#include "CCameraMgr.h"



CLinkScript::CLinkScript()
	: CScript(SCRIPT_TYPE::LINKSCRIPT)
{
}

CLinkScript::~CLinkScript()
{
}

void CLinkScript::CalcMoveDirection()
{
	// camera and link tick point problem can occur.
	// maybe... rigidbody tick determine where the link is. -> so doesn't matter.
	Vec3 vCamFrontDir = m_pLinkCamObj->Transform()->GetWorldDir(DIR::FRONT);

	m_vDir[(UINT)LINK_DIRECTION::FRONT] = Vec3(vCamFrontDir.x, 0.f, vCamFrontDir.z);
	m_vDir[(UINT)LINK_DIRECTION::FRONT].Normalize();
	m_vDir[(UINT)LINK_DIRECTION::BACK] = Vec3(-m_vDir[(UINT)LINK_DIRECTION::FRONT].x, 0.f, -m_vDir[(UINT)LINK_DIRECTION::FRONT].z);
	m_vDir[(UINT)LINK_DIRECTION::LEFT] = Vec3(-m_vDir[(UINT)LINK_DIRECTION::FRONT].z, 0.f, m_vDir[(UINT)LINK_DIRECTION::FRONT].x);
	m_vDir[(UINT)LINK_DIRECTION::RIGHT] = Vec3(m_vDir[(UINT)LINK_DIRECTION::FRONT].z, 0.f, -m_vDir[(UINT)LINK_DIRECTION::FRONT].x);
	m_vDir[(UINT)LINK_DIRECTION::NONE] = Vec3::Zero;

	// Test Code
	//Vec3 vPos = Transform()->GetRelativePos();
	//if (KEY_PRESSED(KEY::W))
	//{
	//	m_vDir[(UINT)LINK_DIRECTION::FRONT] *= 10.f;
	//	vPos += m_vDir[(UINT)LINK_DIRECTION::FRONT];
	//}
	//if (KEY_PRESSED(KEY::S))
	//{
	//	m_vDir[(UINT)LINK_DIRECTION::BACK] *= 10.f;
	//	vPos += m_vDir[(UINT)LINK_DIRECTION::BACK];
	//}
	//if (KEY_PRESSED(KEY::A))
	//{
	//	m_vDir[(UINT)LINK_DIRECTION::LEFT] *= 10.f;
	//	vPos += m_vDir[(UINT)LINK_DIRECTION::LEFT];
	//}
	//if (KEY_PRESSED(KEY::D))
	//{
	//	m_vDir[(UINT)LINK_DIRECTION::RIGHT] *= 10.f;
	//	vPos += m_vDir[(UINT)LINK_DIRECTION::RIGHT];
	//}

	//Transform()->SetRelativePos(vPos);
}

void CLinkScript::begin()
{
	m_pLinkCamObj = CCameraMgr::GetInst()->GetCameraObj(CAMERA_SELECTION::LINK);
}

void CLinkScript::tick()
{
	CalcMoveDirection();
}

LINK_FRONT_TOE CLinkScript::GetFrontToe(CAnimation3D* _pCurAnim)
{
	CAnimator3D* pAnimator = Animator3D();
	CAnimation3D* pAnim = nullptr;

	if(pAnimator)
		pAnim = pAnimator->GetCurAnimation();

	if (pAnim)
	{
		int iCurFrame = pAnim->GetCurFrame();
		float fCurTime = iCurFrame / 60.f;
		const tMTBone& tBoneToe_L = pAnimator->GetBoneByName(L"Toe_L");
		const tMTBone& tBoneToe_R = pAnimator->GetBoneByName(L"Toe_R");
		const vector<tMTKeyFrame>& vecKeyFrame_Toe_L = tBoneToe_L.vecKeyFrame;
		const vector<tMTKeyFrame>& vecKeyFrame_Toe_R = tBoneToe_R.vecKeyFrame;

		int iIdxToe_L = vecKeyFrame_Toe_L.size() / 2;
		int iIdxToe_R = vecKeyFrame_Toe_R.size() / 2;
		iIdxToe_L = FindClosestIdx(vecKeyFrame_Toe_L, fCurTime, iIdxToe_L, 0, vecKeyFrame_Toe_L.size());
		iIdxToe_R = FindClosestIdx(vecKeyFrame_Toe_R, fCurTime, iIdxToe_R, 0, vecKeyFrame_Toe_R.size());

		// should consider direction of link
		if (vecKeyFrame_Toe_L[iIdxToe_L].vTranslate.z < vecKeyFrame_Toe_R[iIdxToe_R].vTranslate.z)
			return LINK_FRONT_TOE::LEFT;
		else
			return LINK_FRONT_TOE::RIGHT;
	}

	return LINK_FRONT_TOE::END;
}

int CLinkScript::FindClosestIdx(const vector<tMTKeyFrame>& _vec, float _fTime, int _iIdx ,int _iStart, int _iEnd)
{
	if (_iStart == _iIdx)
		return _iIdx;

	if (_vec[_iIdx].dTime > _fTime)
	{
		return FindClosestIdx(_vec, _fTime, (_iStart + _iIdx) / 2, _iStart, _iIdx);
	}
	else
	{
		return FindClosestIdx(_vec, _fTime, (_iEnd + _iIdx) / 2 , _iIdx, _iEnd);
	}
}

void CLinkScript::BeginOverlap(CGameObject* _pOther)
{
}

void CLinkScript::Overlap(CGameObject* _pOther)
{
}

void CLinkScript::EndOverlap(CGameObject* _pOther)
{
	
}

void CLinkScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
}

void CLinkScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
}


