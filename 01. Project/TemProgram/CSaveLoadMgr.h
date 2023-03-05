#pragma once

class CLevel;
class CLayer;
class CGameObject;

class CSaveLoadMgr
	: public CSingleton<CSaveLoadMgr>
{
private:

public:
	void init();

public:
	void SaveLevel(CLevel* _Level);
	CLevel* LoadLevel(wstring _strRelativePath);

public:
	// 정적 멤버 함수로 선언한 이유는 Client 밖에 있는 Engine의 Prefab저장시 사용하기 위해서 정적 멤버 함수로 선언해주었다.
	static void SaveGameObject(CGameObject* _Object, FILE* _File);
	static CGameObject* LoadGameObject(FILE* _File);

private:
	CSaveLoadMgr();
	~CSaveLoadMgr();
	friend class CSingleton<CSaveLoadMgr>;
};

