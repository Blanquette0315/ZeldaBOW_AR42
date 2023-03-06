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
	// ���� ��� �Լ��� ������ ������ Client �ۿ� �ִ� Engine�� Prefab����� ����ϱ� ���ؼ� ���� ��� �Լ��� �������־���.
	static void SaveGameObject(CGameObject* _Object, FILE* _File);
	static CGameObject* LoadGameObject(FILE* _File);

private:
	CSaveLoadMgr();
	~CSaveLoadMgr();
	friend class CSingleton<CSaveLoadMgr>;
};
