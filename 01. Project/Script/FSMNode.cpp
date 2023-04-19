#include "pch.h"
#include "FSMNode.h"

#include <Engine/CAnimation3D.h>

tTransition::tTransition(CAnimation3D* _pAnim)
{
	pTargetAnim = _pAnim;
	pTargetAnimKey = pTargetAnim->GetAnimName();
}

tTransition::~tTransition()
{

}