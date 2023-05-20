#include "pch.h"
#include "CScriptMgr.h"

#include "CBokoblinScript.h"
#include "CBonesocketScript.h"
#include "CBossFireballScript.h"
#include "CBossScript.h"
#include "CCamScript.h"
#include "CEffectScaler.h"
#include "CEquipableScript.h"
#include "CGroundCheckScript.h"
#include "CLinkAnimScript.h"
#include "CLinkArrowScript.h"
#include "CLinkBombScript.h"
#include "CLinkBowScript.h"
#include "CLinkCamScript.h"
#include "CLinkHitScript.h"
#include "CLinkScript.h"
#include "CLinkSwordScript.h"
#include "CLockOnScript.h"
#include "CMissileScript.h"
#include "CMonHeadScript.h"
#include "CMonsterAIScript.h"
#include "CMonsterScript.h"
#include "CMonWeaponScript.h"
#include "CNavTestSCR.h"
#include "CPlayerScript.h"
#include "CRayCastScript.h"
#include "CTestSCR.h"
#include "CUIHeartScript.h"
#include "CUIScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CBokoblinScript");
	_vec.push_back(L"CBonesocketScript");
	_vec.push_back(L"CBossFireballScript");
	_vec.push_back(L"CBossScript");
	_vec.push_back(L"CCamScript");
	_vec.push_back(L"CEffectScaler");
	_vec.push_back(L"CEquipableScript");
	_vec.push_back(L"CGroundCheckScript");
	_vec.push_back(L"CLinkAnimScript");
	_vec.push_back(L"CLinkArrowScript");
	_vec.push_back(L"CLinkBombScript");
	_vec.push_back(L"CLinkBowScript");
	_vec.push_back(L"CLinkCamScript");
	_vec.push_back(L"CLinkHitScript");
	_vec.push_back(L"CLinkScript");
	_vec.push_back(L"CLinkSwordScript");
	_vec.push_back(L"CLockOnScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CMonHeadScript");
	_vec.push_back(L"CMonsterAIScript");
	_vec.push_back(L"CMonsterScript");
	_vec.push_back(L"CMonWeaponScript");
	_vec.push_back(L"CNavTestSCR");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CRayCastScript");
	_vec.push_back(L"CTestSCR");
	_vec.push_back(L"CUIHeartScript");
	_vec.push_back(L"CUIScript");
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CBokoblinScript" == _strScriptName)
		return new CBokoblinScript;
	if (L"CBonesocketScript" == _strScriptName)
		return new CBonesocketScript;
	if (L"CBossFireballScript" == _strScriptName)
		return new CBossFireballScript;
	if (L"CBossScript" == _strScriptName)
		return new CBossScript;
	if (L"CCamScript" == _strScriptName)
		return new CCamScript;
	if (L"CEffectScaler" == _strScriptName)
		return new CEffectScaler;
	if (L"CEquipableScript" == _strScriptName)
		return new CEquipableScript;
	if (L"CGroundCheckScript" == _strScriptName)
		return new CGroundCheckScript;
	if (L"CLinkAnimScript" == _strScriptName)
		return new CLinkAnimScript;
	if (L"CLinkArrowScript" == _strScriptName)
		return new CLinkArrowScript;
	if (L"CLinkBombScript" == _strScriptName)
		return new CLinkBombScript;
	if (L"CLinkBowScript" == _strScriptName)
		return new CLinkBowScript;
	if (L"CLinkCamScript" == _strScriptName)
		return new CLinkCamScript;
	if (L"CLinkHitScript" == _strScriptName)
		return new CLinkHitScript;
	if (L"CLinkScript" == _strScriptName)
		return new CLinkScript;
	if (L"CLinkSwordScript" == _strScriptName)
		return new CLinkSwordScript;
	if (L"CLockOnScript" == _strScriptName)
		return new CLockOnScript;
	if (L"CMissileScript" == _strScriptName)
		return new CMissileScript;
	if (L"CMonHeadScript" == _strScriptName)
		return new CMonHeadScript;
	if (L"CMonsterAIScript" == _strScriptName)
		return new CMonsterAIScript;
	if (L"CMonsterScript" == _strScriptName)
		return new CMonsterScript;
	if (L"CMonWeaponScript" == _strScriptName)
		return new CMonWeaponScript;
	if (L"CNavTestSCR" == _strScriptName)
		return new CNavTestSCR;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CRayCastScript" == _strScriptName)
		return new CRayCastScript;
	if (L"CTestSCR" == _strScriptName)
		return new CTestSCR;
	if (L"CUIHeartScript" == _strScriptName)
		return new CUIHeartScript;
	if (L"CUIScript" == _strScriptName)
		return new CUIScript;
	return nullptr;
}

CScript * CScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::BOKOBLINSCRIPT:
		return new CBokoblinScript;
		break;
	case (UINT)SCRIPT_TYPE::BONESOCKETSCRIPT:
		return new CBonesocketScript;
		break;
	case (UINT)SCRIPT_TYPE::BOSSFIREBALLSCRIPT:
		return new CBossFireballScript;
		break;
	case (UINT)SCRIPT_TYPE::BOSSSCRIPT:
		return new CBossScript;
		break;
	case (UINT)SCRIPT_TYPE::CAMSCRIPT:
		return new CCamScript;
		break;
	case (UINT)SCRIPT_TYPE::EFFECTSCALER:
		return new CEffectScaler;
		break;
	case (UINT)SCRIPT_TYPE::EQUIPABLESCRIPT:
		return new CEquipableScript;
		break;
	case (UINT)SCRIPT_TYPE::GROUNDCHECKSCRIPT:
		return new CGroundCheckScript;
		break;
	case (UINT)SCRIPT_TYPE::LINKANIMSCRIPT:
		return new CLinkAnimScript;
		break;
	case (UINT)SCRIPT_TYPE::LINKARROWSCRIPT:
		return new CLinkArrowScript;
		break;
	case (UINT)SCRIPT_TYPE::LINKBOMBSCRIPT:
		return new CLinkBombScript;
		break;
	case (UINT)SCRIPT_TYPE::LINKBOWSCRIPT:
		return new CLinkBowScript;
		break;
	case (UINT)SCRIPT_TYPE::LINKCAMSCRIPT:
		return new CLinkCamScript;
		break;
	case (UINT)SCRIPT_TYPE::LINKHITSCRIPT:
		return new CLinkHitScript;
		break;
	case (UINT)SCRIPT_TYPE::LINKSCRIPT:
		return new CLinkScript;
		break;
	case (UINT)SCRIPT_TYPE::LINKSWORDSCRIPT:
		return new CLinkSwordScript;
		break;
	case (UINT)SCRIPT_TYPE::LOCKONSCRIPT:
		return new CLockOnScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT:
		return new CMissileScript;
		break;
	case (UINT)SCRIPT_TYPE::MONHEADSCRIPT:
		return new CMonHeadScript;
		break;
	case (UINT)SCRIPT_TYPE::MONSTERAISCRIPT:
		return new CMonsterAIScript;
		break;
	case (UINT)SCRIPT_TYPE::MONSTERSCRIPT:
		return new CMonsterScript;
		break;
	case (UINT)SCRIPT_TYPE::MONWEAPONSCRIPT:
		return new CMonWeaponScript;
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
	case (UINT)SCRIPT_TYPE::UIHEARTSCRIPT:
		return new CUIHeartScript;
		break;
	case (UINT)SCRIPT_TYPE::UISCRIPT:
		return new CUIScript;
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

	case SCRIPT_TYPE::BONESOCKETSCRIPT:
		return L"CBonesocketScript";
		break;

	case SCRIPT_TYPE::BOSSFIREBALLSCRIPT:
		return L"CBossFireballScript";
		break;

	case SCRIPT_TYPE::BOSSSCRIPT:
		return L"CBossScript";
		break;

	case SCRIPT_TYPE::CAMSCRIPT:
		return L"CCamScript";
		break;

	case SCRIPT_TYPE::EFFECTSCALER:
		return L"CEffectScaler";
		break;

	case SCRIPT_TYPE::EQUIPABLESCRIPT:
		return L"CEquipableScript";
		break;

	case SCRIPT_TYPE::GROUNDCHECKSCRIPT:
		return L"CGroundCheckScript";
		break;

	case SCRIPT_TYPE::LINKANIMSCRIPT:
		return L"CLinkAnimScript";
		break;

	case SCRIPT_TYPE::LINKARROWSCRIPT:
		return L"CLinkArrowScript";
		break;

	case SCRIPT_TYPE::LINKBOMBSCRIPT:
		return L"CLinkBombScript";
		break;

	case SCRIPT_TYPE::LINKBOWSCRIPT:
		return L"CLinkBowScript";
		break;

	case SCRIPT_TYPE::LINKCAMSCRIPT:
		return L"CLinkCamScript";
		break;

	case SCRIPT_TYPE::LINKHITSCRIPT:
		return L"CLinkHitScript";
		break;

	case SCRIPT_TYPE::LINKSCRIPT:
		return L"CLinkScript";
		break;

	case SCRIPT_TYPE::LINKSWORDSCRIPT:
		return L"CLinkSwordScript";
		break;

	case SCRIPT_TYPE::LOCKONSCRIPT:
		return L"CLockOnScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";
		break;

	case SCRIPT_TYPE::MONHEADSCRIPT:
		return L"CMonHeadScript";
		break;

	case SCRIPT_TYPE::MONSTERAISCRIPT:
		return L"CMonsterAIScript";
		break;

	case SCRIPT_TYPE::MONSTERSCRIPT:
		return L"CMonsterScript";
		break;

	case SCRIPT_TYPE::MONWEAPONSCRIPT:
		return L"CMonWeaponScript";
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

	case SCRIPT_TYPE::UIHEARTSCRIPT:
		return L"CUIHeartScript";
		break;

	case SCRIPT_TYPE::UISCRIPT:
		return L"CUIScript";
		break;

	}
	return nullptr;
}