#pragma once

#include <vector>
#include <string>

enum SCRIPT_TYPE
{
	BOKOBLINSCRIPT,
	BONESOCKETSCRIPT,
	CAMSCRIPT,
	EQUIPABLESCRIPT,
	GROUNDCHECKSCRIPT,
	LINKANIMSCRIPT,
	LINKARROWSCRIPT,
	LINKBOWSCRIPT,
	LINKCAMSCRIPT,
	LINKHITSCRIPT,
	LINKSCRIPT,
	LINKSWORDSCRIPT,
	LOCKONSCRIPT,
	MISSILESCRIPT,
	MONHEADSCRIPT,
	MONSTERAISCRIPT,
	MONSTERSCRIPT,
	MONWEAPONSCRIPT,
	NAVTESTSCR,
	PLAYERSCRIPT,
	RAYCASTSCRIPT,
	TESTSCR,
	END,
};

using namespace std;

class CScript;

class CScriptMgr
{
public:
	static void GetScriptInfo(vector<wstring>& _vec);
	static CScript * GetScript(const wstring& _strScriptName);
	static CScript * GetScript(UINT _iScriptType);
	static const wchar_t * GetScriptName(CScript * _pScript);
};
