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

void CLinkAnimScript::ReplaceNodeAnim()
{
	map<wstring, tAnimNode*>::iterator iter = m_mapAnimNode.begin();
	for (; iter != m_mapAnimNode.end(); ++iter)
	{
		if(iter->second->pAnim)
			iter->second->pAnim = m_mapAnim.find(iter->second->pAnimKey)->second;
	}
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
	SetAnimTran(pAnimNode, LAT_JUMP_L, LAC_KEY_SPACE | LAC_TOE_L_FRONT);
	SetAnimTran(pAnimNode, LAT_JUMP_R, LAC_KEY_SPACE, LAC_TOE_L_FRONT);
	SetAnimTran(pAnimNode, LAT_SWORD_EQUIP_ON, LAC_KEY_N1);
	SetAnimTran(pAnimNode, LAT_BOW_EQUIP_ON, LAC_KEY_N2);
	
	// Walk
	SetAnimNode(pAnimNode, LAT_WALK, LAP_REPEAT); pAnimNode->AddFuncSteady(&CLinkAnimScript::Func_WalkRunDash);
	SetAnimTran(pAnimNode, LAT_RUN, LAC_KEY_WSAD | LAC_MODE_RUN);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_KEY_WSAD | LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_WALK_ED_L, LAC_TOE_L_FRONT, LAC_KEY_WSAD);
	SetAnimTran(pAnimNode, LAT_WALK_ED_R, 0, LAC_KEY_WSAD | LAC_TOE_L_FRONT);
	SetAnimTran(pAnimNode, LAT_JUMP_L, LAC_KEY_SPACE | LAC_TOE_L_FRONT);
	SetAnimTran(pAnimNode, LAT_JUMP_R, LAC_KEY_SPACE, LAC_TOE_L_FRONT);

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
	SetAnimNode(pAnimNode, LAT_RUN, LAP_REPEAT); pAnimNode->AddFuncSteady(&CLinkAnimScript::Func_WalkRunDash);
	SetAnimTran(pAnimNode, LAT_WAIT, 0, LAC_KEY_WSAD);
	SetAnimTran(pAnimNode, LAT_WALK, LAC_KEY_WSAD | LAC_MODE_WALK);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_KEY_WSAD | LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_RUN_BRAKE_L, LAC_TURN_BACK | LAC_TOE_L_FRONT );
	SetAnimTran(pAnimNode, LAT_RUN_BRAKE_R, LAC_TURN_BACK, LAC_TOE_L_FRONT);
	SetAnimTran(pAnimNode, LAT_RUN_ED_L, LAC_TOE_L_FRONT, LAC_KEY_WSAD);
	SetAnimTran(pAnimNode, LAT_RUN_ED_R, 0, LAC_TOE_L_FRONT | LAC_KEY_WSAD);
	SetAnimTran(pAnimNode, LAT_JUMP_L, LAC_KEY_SPACE | LAC_TOE_L_FRONT);
	SetAnimTran(pAnimNode, LAT_JUMP_R, LAC_KEY_SPACE, LAC_TOE_L_FRONT);

	SetAnimNode(pAnimNode, LAT_RUN_BRAKE_L); pAnimNode->AddFuncEnd(&CLinkAnimScript::Func_TurnBack);
	SetAnimTran(pAnimNode, LAT_RUN, LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_RUN_BRAKE_R); pAnimNode->AddFuncEnd(&CLinkAnimScript::Func_TurnBack);
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
	SetAnimNode(pAnimNode, LAT_DASH, LAP_REPEAT); pAnimNode->AddFuncSteady(&CLinkAnimScript::Func_WalkRunDash);
	SetAnimTran(pAnimNode, LAT_WAIT, 0, LAC_KEY_WSAD);
	SetAnimTran(pAnimNode, LAT_WALK, LAC_KEY_WSAD | LAC_MODE_WALK, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_RUN, LAC_KEY_WSAD | LAC_MODE_RUN, LAC_KEY_SHIFT);
	SetAnimTran(pAnimNode, LAT_DASH_BRAKE_L, LAC_TURN_BACK | LAC_TOE_L_FRONT);
	SetAnimTran(pAnimNode, LAT_DASH_BRAKE_R, LAC_TURN_BACK, LAC_TOE_L_FRONT);
	SetAnimTran(pAnimNode, LAT_JUMP_L, LAC_KEY_SPACE | LAC_TOE_L_FRONT);
	SetAnimTran(pAnimNode, LAT_JUMP_R, LAC_KEY_SPACE, LAC_TOE_L_FRONT);

	SetAnimNode(pAnimNode, LAT_DASH_BRAKE_L); pAnimNode->AddFuncEnd(&CLinkAnimScript::Func_TurnBack);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_DASH_BRAKE_R); pAnimNode->AddFuncEnd(&CLinkAnimScript::Func_TurnBack);
	SetAnimTran(pAnimNode, LAT_DASH, LAC_ANIM_FINISHED);

	// Jump
	SetAnimNode(pAnimNode, LAT_JUMP_L); pAnimNode->AddFuncStart(&CLinkAnimScript::Func_Jump);
	SetAnimTran(pAnimNode, LAT_LAND_L, LAC_ANIM_FINISHED | LAC_GROUNDED);

	SetAnimNode(pAnimNode, LAT_LAND_L);
	SetAnimTran(pAnimNode, LAT_WAIT, LAC_GROUNDED | LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_JUMP_R); pAnimNode->AddFuncStart(&CLinkAnimScript::Func_Jump);
	SetAnimTran(pAnimNode, LAT_LAND_R, LAC_ANIM_FINISHED | LAC_GROUNDED);

	SetAnimNode(pAnimNode, LAT_LAND_R);
	SetAnimTran(pAnimNode, LAT_WAIT, LAC_GROUNDED | LAC_ANIM_FINISHED);

	// Sword
	SetAnimNode(pAnimNode, LAT_SWORD_EQUIP_ON, LAP_BLEND | LAP_EQUIP_SWORD); pAnimNode->AddFuncStart(&CLinkAnimScript::Func_SwordEquipOn);
	SetAnimTran(pAnimNode, LAT_SWORD_LOCKON_WAIT, LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_SWORD_EQUIP_OFF, LAP_BLEND); pAnimNode->AddFuncStart(&CLinkAnimScript::Func_SwordEquipOff);
	SetAnimTran(pAnimNode, LAT_WAIT, LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_SWORD_LOCKON_WAIT, LAP_REPEAT | LAP_BLEND | LAP_EQUIP_SWORD);
	SetAnimTran(pAnimNode, LAT_SWORD_EQUIP_OFF, LAC_KEY_N1);
	SetAnimTran(pAnimNode, LAT_SWORD_MOVE_RUN, LAC_KEY_WSAD, LAC_MODE_LOCKON);
	SetAnimTran(pAnimNode, LAT_SWORD_ATTACK_S1, LAC_KEY_LBTN);

	SetAnimNode(pAnimNode, LAT_SWORD_MOVE_RUN, LAP_REPEAT | LAP_EQUIP_SWORD); pAnimNode->AddFuncSteady(&CLinkAnimScript::Func_SwordRun);
	SetAnimTran(pAnimNode, LAT_SWORD_LOCKON_WAIT, 0, LAC_KEY_LBTN | LAC_KEY_WSAD);
	SetAnimTran(pAnimNode, LAT_SWORD_ATTACK_S1, LAC_KEY_LBTN);

	SetAnimNode(pAnimNode, LAT_SWORD_ATTACK_S1, LAP_COMBO | LAP_ATTACK | LAP_EQUIP_SWORD); pAnimNode->AddFuncStart(&CLinkAnimScript::Func_SwordAttackMove);
	SetAnimTran(pAnimNode, LAT_SWORD_LOCKON_WAIT, LAC_ANIM_FINISHED, LAC_KEY_LBTN_COMBO);
	SetAnimTran(pAnimNode, LAT_SWORD_ATTACK_S2, LAC_KEY_LBTN_COMBO | LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_SWORD_ATTACK_S2, LAP_COMBO | LAP_ATTACK | LAP_EQUIP_SWORD); pAnimNode->AddFuncStart(&CLinkAnimScript::Func_SwordAttackMove);
	SetAnimTran(pAnimNode, LAT_SWORD_LOCKON_WAIT, LAC_ANIM_FINISHED, LAC_KEY_LBTN_COMBO);
	SetAnimTran(pAnimNode, LAT_SWORD_ATTACK_S3, LAC_KEY_LBTN_COMBO | LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_SWORD_ATTACK_S3, LAP_COMBO | LAP_ATTACK | LAP_EQUIP_SWORD); pAnimNode->AddFuncStart(&CLinkAnimScript::Func_SwordAttackMove);
	SetAnimTran(pAnimNode, LAT_SWORD_LOCKON_WAIT, LAC_ANIM_FINISHED, LAC_KEY_LBTN_COMBO);
	SetAnimTran(pAnimNode, LAT_SWORD_ATTACK_SF, LAC_KEY_LBTN_COMBO | LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_SWORD_ATTACK_SF, LAP_ATTACK | LAP_EQUIP_SWORD); pAnimNode->AddFuncStart(&CLinkAnimScript::Func_SwordAttackMove);
	SetAnimTran(pAnimNode, LAT_SWORD_LOCKON_WAIT, LAC_ANIM_FINISHED);

	// Bow
	SetAnimNode(pAnimNode, LAT_BOW_EQUIP_ON, LAP_BLEND | LAP_EQUIP_BOW); pAnimNode->AddFuncStart(&CLinkAnimScript::Func_BowEquipOn);
	SetAnimTran(pAnimNode, LAT_BOW_LOCKON_WAIT, LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_BOW_LOCKON_WAIT, LAP_REPEAT | LAP_EQUIP_BOW | LAP_BLEND);
	SetAnimTran(pAnimNode, LAT_BOW_EQUIP_OFF, LAC_KEY_N2);
	SetAnimTran(pAnimNode, LAT_BOW_MOVE_RUN_UPPER, LAC_KEY_WSAD, LAC_MODE_LOCKON);
	SetAnimTran(pAnimNode, LAT_BOW_ATTACK_CHARGE, LAC_KEY_LBTN);

	SetAnimNode(pAnimNode, LAT_BOW_EQUIP_OFF, LAP_BLEND); pAnimNode->AddFuncStart(&CLinkAnimScript::Func_BowEquipOff);
	SetAnimTran(pAnimNode, LAT_WAIT, LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_BOW_MOVE_RUN_UPPER, LAP_REPEAT | LAP_EQUIP_BOW); pAnimNode->AddFuncSteady(&CLinkAnimScript::Func_BowRun);
	SetAnimTran(pAnimNode, LAT_BOW_LOCKON_WAIT, 0, LAC_KEY_LBTN | LAC_KEY_WSAD);
	SetAnimTran(pAnimNode, LAT_BOW_ATTACK_CHARGE, LAC_KEY_LBTN);

	//SetAnimNode(pAnimNode, LAT_BOW_ATTACK_LOAD, LAP_BLEND | LAP_EQUIP_BOW);
	//SetAnimTran(pAnimNode, LAT_BOW_ATTACK_CHARGE, LAC_ANIM_FINISHED);

	SetAnimNode(pAnimNode, LAT_BOW_ATTACK_CHARGE, LAP_BLEND | LAP_EQUIP_BOW);
	SetAnimTran(pAnimNode, LAT_BOW_ATTACK_SHOOT, 0, LAC_KEY_LBTN);

	SetAnimNode(pAnimNode, LAT_BOW_ATTACK_SHOOT, LAP_EQUIP_BOW);
	SetAnimTran(pAnimNode, LAT_BOW_LOCKON_WAIT, LAC_ANIM_FINISHED);
		
	SetAnimNode(pAnimNode, LAT_BOW_EQUIP_ON_RUN);

	SetAnimNode(pAnimNode, LAT_BOW_EQUIP_OFF_RUN);
	


	
	// Damaged
	SetAnimNode(pAnimNode, LAT_DAMAGE_S_B);
	SetAnimTran(pAnimNode, LAT_SWORD_LOCKON_WAIT, LAC_EQUIP_SWORD | LAC_ANIM_FINISHED);
	SetAnimTran(pAnimNode, LAT_BOW_LOCKON_WAIT, LAC_EQUIP_BOW | LAC_ANIM_FINISHED);
	SetAnimTran(pAnimNode, LAT_WAIT, LAC_ANIM_FINISHED, LAC_EQUIP_SWORD | LAC_EQUIP_BOW);

	SetAnimNode(pAnimNode, LAT_DAMAGE_M_B);
	SetAnimTran(pAnimNode, LAT_SWORD_LOCKON_WAIT, LAC_EQUIP_SWORD | LAC_ANIM_FINISHED);
	SetAnimTran(pAnimNode, LAT_BOW_LOCKON_WAIT, LAC_EQUIP_BOW | LAC_ANIM_FINISHED);
	SetAnimTran(pAnimNode, LAT_WAIT, LAC_ANIM_FINISHED, LAC_EQUIP_SWORD | LAC_EQUIP_BOW);

	// Any State Node
	tAnimNode* pAnyStateNode = new tAnimNode();
	pAnyStateNode->pAnimKey = LINK_STRING_WCHAR[LINK_STRING_ANYSTATE_NODE_KEY];
	SetAnimTran(pAnyStateNode, LAT_DAMAGE_M_B, LAC_DAMAGED_BACK | LAC_DAMAGED_MEDIUM, LAC_DEAD);
	SetAnimTran(pAnyStateNode, LAT_DAMAGE_S_B, LAC_DAMAGED_BACK | LAC_DAMAGED_SMALL, LAC_DEAD);
	m_mapAnimNode.insert({ pAnyStateNode->pAnimKey, pAnyStateNode });
}