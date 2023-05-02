#include "pch.h"
#include "CBonesocketScript.h"

#include <Engine/CAnimation3D.h>

CBonesocketScript::CBonesocketScript()
	: CScript(BONESOCKETSCRIPT)
	, m_iBoneIdx(0)
{
	//m_vOffset = Vec3(-0.29, -0.06, -0.036);
	//m_vOffsetRot = Vec3(-XM_PI / 2.f, 0.f, 0.f);
	// m_iBoneIdx = 18; 
	AddScriptParam(SCRIPT_PARAM::VEC3, "OffsetPos       ", &m_vOffsetPos);
	AddScriptParam(SCRIPT_PARAM::VEC3, "OffsetRot       ", &m_vOffsetRot);
	AddScriptParam(SCRIPT_PARAM::INT,  "BoneIdx         ", &m_iBoneIdx);
}

CBonesocketScript::~CBonesocketScript()
{
}

void CBonesocketScript::tick()
{
	if (GetOwner()->GetParent() == nullptr)
		return;

	
	if (GetOwner()->GetParent()->Animator3D()->GetCurAnimation() != nullptr)
	{
		const vector<tMTBone>* vecBones = GetOwner()->GetParent()->MeshRender()->GetMesh()->GetBones();
		int iCurFrame = GetOwner()->GetParent()->Animator3D()->GetCurAnimation()->GetCurFrame();
		int iNextFrame = GetOwner()->GetParent()->Animator3D()->GetCurAnimation()->GetNextFrame();
		float fRatio = GetOwner()->GetParent()->Animator3D()->GetCurAnimation()->GetRatio();

		Quaternion a = vecBones->at(m_iBoneIdx).vecKeyFrame[iCurFrame].qRot;
		Quaternion b = vecBones->at(m_iBoneIdx).vecKeyFrame[iNextFrame].qRot;

		Quaternion c = Quaternion::Lerp(a, b, fRatio);
		Vec3 Rot;
		QuaternionToEuler(c, Rot);
		Transform()->SetRelativeRotation(Rot + (m_vOffsetRot * (XM_PI / 180.f)));

		// offsetpos should be effected by rot
		Matrix matRot = XMMatrixRotationX(Rot.x);
		matRot *= XMMatrixRotationY(Rot.y);
		matRot *= XMMatrixRotationZ(Rot.z);

		Vec3 vFinalOffset = XMVector3TransformCoord(m_vOffsetPos, matRot);

		// Transform()->SetRelativeScale(Vector3::Lerp(vecBones->at(m_iBoneIdx).vecKeyFrame[iCurFrame].vScale, vecBones->at(m_iBoneIdx).vecKeyFrame[iNextFrame].vScale, fRatio));
		Transform()->SetRelativePos(Vector3::Lerp(vecBones->at(m_iBoneIdx).vecKeyFrame[iCurFrame].vTranslate + vFinalOffset, vecBones->at(m_iBoneIdx).vecKeyFrame[iNextFrame].vTranslate + vFinalOffset, fRatio));
		//Vec3 CurRot;
		//Vec3 NextRot;
		//const vector<tMTKeyFrame>& vecKeyFrame = vecBones->at(m_iBoneIdx).vecKeyFrame;
	
		//QuaternionToEuler(vecBones->at(m_iBoneIdx).vecKeyFrame[iCurFrame].qRot, CurRot);
		//QuaternionToEuler(vecBones->at(m_iBoneIdx).vecKeyFrame[iNextFrame].qRot, NextRot);
		//Transform()->SetRelativeRotation(Vector3::Lerp(CurRot, NextRot, fRatio) + (m_vOffsetRot * (XM_PI / 180.f)));



		//Transform()->SetRelativePos(vecBones->at(18).vecKeyFrame[iCurFrame].vTranslate + m_vOffset);
		//Transform()->SetRelativeScale(Vector3::Lerp(vecBones->at(18).vecKeyFrame[iCurFrame].vScale, vecBones->at(18).vecKeyFrame[iNextFrame].vScale, fRatio));
		//Transform()->SetRelativeRotation(CurRot - Vec3(XM_PI / 2.f, 0.f, 0.f));	
	}
}

void CBonesocketScript::SaveToYAML(YAML::Emitter& _emitter)
{
	CScript::SaveToYAML(_emitter);
	_emitter << YAML::Key << "OffsetPos";
	_emitter << YAML::Value << m_vOffsetPos;
	_emitter << YAML::Key << "OffsetRot";
	_emitter << YAML::Value << m_vOffsetRot;
	_emitter << YAML::Key << "BoneIdx";
	_emitter << YAML::Value << m_iBoneIdx;
}

void CBonesocketScript::LoadFromYAML(YAML::Node& _node)
{
	CScript::LoadFromYAML(_node);
	SAFE_LOAD_FROM_YAML(Vec3, m_vOffsetPos, _node["OffsetPos"]);
	SAFE_LOAD_FROM_YAML(Vec3, m_vOffsetRot, _node["OffsetRot"]);
	SAFE_LOAD_FROM_YAML(UINT, m_iBoneIdx, _node["BoneIdx"]);
}
