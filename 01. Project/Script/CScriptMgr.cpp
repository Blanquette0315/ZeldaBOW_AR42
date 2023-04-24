#include "pch.h"
#include "CScriptMgr.h"

#include "CCamScript.h"
#include "CGroundCheckScript.h"
#include "CLinkAnimScript.h"
#include "CLinkCamScript.h"
#include "CLinkScript.h"
#include "CMissileScript.h"
#include "CMonsterScript.h"
#include "CNavTestSCR.h"
#include "CPlayerScript.h"
#include "CRayCastScript.h"
#include "CTestSCR.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CCamScript");
	_vec.push_back(L"CGroundCheckScript");
	_vec.push_back(L"CLinkAnimScript");
	_vec.push_back(L"CLinkCamScript");
	_vec.push_back(L"CLinkScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CMonsterScript");
	_vec.push_back(L"CNavTestSCR");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CRayCastScript");
	_vec.push_back(L"CTestSCR");
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CCamScript" == _strScriptName)
		return new CCamScript;
	if (L"CGroundCheckScript" == _strScriptName)
		return new CGroundCheckScript;
	if (L"CLinkAnimScript" == _strScriptName)
		return new CLinkAnimScript;
	if (L"CLinkCamScript" == _strScriptName)
		return new CLinkCamScript;
	if (L"CLinkScript" == _strScriptName)
		return new CLinkScript;
	if (L"CMissileScript" == _strScriptName)
		return new CMissileScript;
	if (L"CMonsterScript" == _strScriptName)
		return new CMonsterScript;
	if (L"CNavTestSCR" == _strScriptName)
		return new CNavTestSCR;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CRayCastScript" == _strScriptName)
		return new CRayCastScript;
	if (L"CTestSCR" == _strScriptName)
		return new CTestSCR;
	return nullptr;
}

CScript * CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::CAMSCRIPT:
		return new CCamScript;
		break;
	case (UINT)SCRIPT_TYPE::GROUNDCHECKSCRIPT:
		return new CGroundCheckScript;
		break;
	case (UINT)SCRIPT_TYPE::LINKANIMSCRIPT:
		return new CLinkAnimScript;
		break;
	case (UINT)SCRIPT_TYPE::LINKCAMSCRIPT:
		return new CLinkCamScript;
		break;
	case (UINT)SCRIPT_TYPE::LINKSCRIPT:
		return new CLinkScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT:
		return new CMissileScript;
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
	case (UINT)SCRIPT_TYPE::RAYCASTSCRIPT:
		return new CRayCastScript;
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
	case SCRIPT_TYPE::CAMSCRIPT:
		return L"CCamScript";
		break;

	case SCRIPT_TYPE::GROUNDCHECKSCRIPT:
		return L"CGroundCheckScript";
		break;

	case SCRIPT_TYPE::LINKANIMSCRIPT:
		return L"CLinkAnimScript";
		break;

	case SCRIPT_TYPE::LINKCAMSCRIPT:
		return L"CLinkCamScript";
		break;

	case SCRIPT_TYPE::LINKSCRIPT:
		return L"CLinkScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";
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

	case SCRIPT_TYPE::RAYCASTSCRIPT:
		return L"CRayCastScript";
		break;

	case SCRIPT_TYPE::TESTSCR:
		return L"CTestSCR";
		break;

	}
	return nullptr;
}