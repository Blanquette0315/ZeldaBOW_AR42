#include "pch.h"
#include "CreateTestLevel.h"

#include <Engine/CEventMgr.h>
#include <Engine/CResMgr.h>
#include <Engine/CCollisionMgr.h>

#include <Engine/CLevelMgr.h>
#include <Engine/CLevel.h>
#include <Engine/CGameObject.h>
#include <Engine/GlobalComponent.h>

#include <Engine/CPaintShader.h>

#include <Script/CPlayerScript.h>
#include <Script/CMonsterAIScript.h>
#include <Script/CBokoblinScript.h>

#include "CCameraScript.h"

#include "CSaveLoadMgr.h"

void CreateTestLevel()
{
	//CLevel* Level = CSaveLoadMgr::GetInst()->LoadLevel(L"level\\Test.lv");
	//ChangeLevel(Level);
	//return;
	//#ifdef _DEBUG
	//	CLevel* Level = CSaveLoadMgr::GetInst()->LoadLevel(L"level\\Test Level2.lv");
	//	
	//	ChangeLevel(Level);
	//	return;
	//#else
	//	CLevel* Level = CSaveLoadMgr::GetInst()->LoadLevel(L"level\\MainLevel.lv");
	//	CLevelMgr::GetInst()->RegisterLevel(Level->GetRelativePath(), Level);
	//	ChangeLevelAndPlay(Level);
	//	return;
	//#endif

	CLevel* pLevel = new CLevel;

	ChangeLevel(pLevel);
}
