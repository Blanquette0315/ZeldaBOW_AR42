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
	SetAnimTran(pAnimNode, LAT_WALK, LAC_KEY_WSAD);

	// Walk
	SetAnimNode(pAnimNode, LAT_WALK, LAP_REPEAT);
	SetAnimTran(pAnimNode, LAT_WAIT, 0, LAC_KEY_WSAD);
}