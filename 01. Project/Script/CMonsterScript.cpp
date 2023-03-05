#include "pch.h"
#include "CMonsterScript.h"

#include <Engine\CGameObject.h>


CMonsterScript::CMonsterScript()
	: CScript(MONSTERSCRIPT)
	, m_pTargetObject(nullptr)
{
}

CMonsterScript::~CMonsterScript()
{
}

void CMonsterScript::tick()
{
	if (!IsValid(m_pTargetObject))
		return;


}