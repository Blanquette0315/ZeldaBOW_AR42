#include "pch.h"
#include "CLinkAnimScript.h"

#include "FSMNode.h"
#include <Engine/CAnimator3D.h>
#include <Engine/CAnimation3D.h>

void CLinkAnimScript::CreateAnimNode()
{
	map<wstring, CAnimation3D*>::iterator iter = m_mapAnim.begin();

	tAnimNode* pAnimNode = nullptr;

	// Create
	for (; iter != m_mapAnim.end(); ++iter)
	{
		pAnimNode = new tAnimNode;
		pAnimNode->pAnimKey = iter->second->GetAnimName();
		pAnimNode->pAnim = iter->second;
		m_mapAnimNode.insert({ pAnimNode->pAnimKey, pAnimNode });
	}
}

void CLinkAnimScript::SetAnimNode(tAnimNode*& _pAnimNode, LINK_ANIM_TYPE _eAnim)
{
	_pAnimNode = m_mapAnimNode.find(LINK_ANIM_WCHAR[_eAnim])->second;
}

void CLinkAnimScript::SetAnimNode(tAnimNode*& _pAnimNode, LINK_ANIM_TYPE _eAnim, UINT _ePref)
{
	_pAnimNode = m_mapAnimNode.find(LINK_ANIM_WCHAR[_eAnim])->second;
	AddBit(_pAnimNode->iPreferences, _ePref);
}

void CLinkAnimScript::SetAnimTran(tAnimNode* _pAnimNode, LINK_ANIM_TYPE _eAnim, UINT _eIncludeAs1Bit, UINT _eIncludeAs0Bit)
{
	tTransition* pAnimTran = new tTransition(m_mapAnimNode.find(LINK_ANIM_WCHAR[_eAnim])->second->pAnim);
	pAnimTran->AddInclude(_eIncludeAs1Bit, _eIncludeAs0Bit);
	_pAnimNode->vecTranAnim.push_back(pAnimTran);
}

void CLinkAnimScript::MakeFSM()
{
	tAnimNode* pAnimNode = nullptr;

	// Wait
	SetAnimNode(pAnimNode, LAT_WAIT, LAP_REPEAT);
	SetAnimTran(pAnimNode, LAT_WALK, LAC_KEY_WSAD | LAC_MODE_WALK, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_RUN, LAC_KEY_WSAD | LAC_MODE_RUN, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_KEY_WSAD | LAC_KEY_SHIFT);

	// Walk
	SetAnimNode(pAnimNode, LAT_WALK, LAP_REPEAT);
	SetAnimTran(pAnimNode, LAT_RUN, LAC_KEY_WSAD | LAC_MODE_RUN);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_KEY_WSAD | LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_WALK_ED_L, LAC_TOE_L_FRONT, LAC_KEY_WSAD);
	SetAnimTran(pAnimNode, LAT_WALK_ED_R, 0, LAC_KEY_WSAD | LAC_TOE_L_FRONT);

	SetAnimNode(pAnimNode, LAT_WALK_ED_L);
	SetAnimTran(pAnimNode, LAT_WAIT, LAC_ANIM_FINISHED);
	SetAnimTran(pAnimNode, LAT_WALK, LAC_KEY_WSAD | LAC_MODE_WALK, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_RUN, LAC_KEY_WSAD | LAC_MODE_RUN, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_KEY_WSAD | LAC_KEY_SHIFT);

	SetAnimNode(pAnimNode, LAT_WALK_ED_R);
	SetAnimTran(pAnimNode, LAT_WAIT, LAC_ANIM_FINISHED);
	SetAnimTran(pAnimNode, LAT_WALK, LAC_KEY_WSAD | LAC_MODE_WALK, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_RUN, LAC_KEY_WSAD | LAC_MODE_RUN, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_KEY_WSAD | LAC_KEY_SHIFT);
	
	// Run
	SetAnimNode(pAnimNode, LAT_RUN, LAP_REPEAT);
	SetAnimTran(pAnimNode, LAT_WAIT, 0, LAC_KEY_WSAD);
	SetAnimTran(pAnimNode, LAT_WALK, LAC_KEY_WSAD | LAC_MODE_WALK);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_KEY_WSAD | LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_RUN_BRAKE_L, LAC_TURN_BACK | LAC_TOE_L_FRONT );
	SetAnimTran(pAnimNode, LAT_RUN_BRAKE_R, LAC_TURN_BACK, LAC_TOE_L_FRONT);
	SetAnimTran(pAnimNode, LAT_RUN_ED_L, LAC_TOE_L_FRONT, LAC_KEY_WSAD);
	SetAnimTran(pAnimNode, LAT_RUN_ED_R, 0, LAC_TOE_L_FRONT | LAC_KEY_WSAD);

	SetAnimNode(pAnimNode, LAT_RUN_BRAKE_L);
	SetAnimTran(pAnimNode, LAT_RUN, LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_RUN_BRAKE_R);
	SetAnimTran(pAnimNode, LAT_RUN, LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_RUN_ED_L);
	SetAnimTran(pAnimNode, LAT_WAIT, LAC_ANIM_FINISHED);
	SetAnimTran(pAnimNode, LAT_WALK, LAC_KEY_WSAD | LAC_MODE_WALK, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_RUN, LAC_KEY_WSAD | LAC_MODE_RUN, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_KEY_WSAD | LAC_KEY_SHIFT);

	SetAnimNode(pAnimNode, LAT_RUN_ED_R);
	SetAnimTran(pAnimNode, LAT_WAIT, LAC_ANIM_FINISHED);
	SetAnimTran(pAnimNode, LAT_WALK, LAC_KEY_WSAD | LAC_MODE_WALK, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_RUN, LAC_KEY_WSAD | LAC_MODE_RUN, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_KEY_WSAD | LAC_KEY_SHIFT);

	// Dash
	SetAnimNode(pAnimNode, LAT_DASH, LAP_REPEAT);
	SetAnimTran(pAnimNode, LAT_WAIT, 0, LAC_KEY_WSAD);
	SetAnimTran(pAnimNode, LAT_WALK, LAC_KEY_WSAD | LAC_MODE_WALK, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_RUN, LAC_KEY_WSAD | LAC_MODE_RUN, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_DASH_BRAKE_L, LAC_TURN_BACK | LAC_TOE_L_FRONT);
	SetAnimTran(pAnimNode, LAT_DASH_BRAKE_R, LAC_TURN_BACK, LAC_TOE_L_FRONT);

	SetAnimNode(pAnimNode, LAT_DASH_BRAKE_L);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_DASH_BRAKE_R);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_ANIM_FINISHED);
}