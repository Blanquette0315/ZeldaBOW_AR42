#include "pch.h"
#include "CScriptMgr.h"

#include "CBokoblinScript.h"
#include "CMissileScript.h"
#include "CMonsterAIScript.h"
#include "CMonsterScript.h"
#include "CNavTestSCR.h"
#include "CPlayerScript.h"
#include "CTestSCR.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CBokoblinScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CMonsterAIScript");
	_vec.push_back(L"CMonsterScript");
	_vec.push_back(L"CNavTestSCR");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CTestSCR");
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CBokoblinScript" == _strScriptName)
		return new CBokoblinScript;
	if (L"CMissileScript" == _strScriptName)
		return new CMissileScript;
	if (L"CMonsterAIScript" == _strScriptName)
		return new CMonsterAIScript;
	if (L"CMonsterScript" == _strScriptName)
		return new CMonsterScript;
	if (L"CNavTestSCR" == _strScriptName)
		return new CNavTestSCR;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CTestSCR" == _strScriptName)
		return new CTestSCR;
	return nullptr;
}

CScript * CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::BOKOBLINSCRIPT:
		return new CBokoblinScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT:
		return new CMissileScript;
		break;
	case (UINT)SCRIPT_TYPE::MONSTERAISCRIPT:
		return new CMonsterAIScript;
		break;
	case (UINT)SCRIPT_TYPE::MONSTERSCRIPT:
		return new CMonsterScript;
		break;
	case (UINT)SCRIPT_TYPE::NAVTESTSCR:
		return new CNavTestSCR;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::TESTSCR:
		return new CTestSCR;
		break;
	}
	return nullptr;
}

const wchar_t * CScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::BOKOBLINSCRIPT:
		return L"CBokoblinScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";
		break;

	case SCRIPT_TYPE::MONSTERAISCRIPT:
		return L"CMonsterAIScript";
		break;

	case SCRIPT_TYPE::MONSTERSCRIPT:
		return L"CMonsterScript";
		break;

	case SCRIPT_TYPE::NAVTESTSCR:
		return L"CNavTestSCR";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::TESTSCR:
		return L"CTestSCR";
		break;

	}
	return nullptr;
}