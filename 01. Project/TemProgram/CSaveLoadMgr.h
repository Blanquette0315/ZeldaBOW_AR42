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
	static void SaveLevel(CLevel* _Level, const wstring& _customName = L"");
	static CLevel* LoadLevel(wstring _strRelativePath);

public:
	// ���� ��� �Լ��� ������ ������ Client �ۿ� �ִ� Engine�� Prefab����� ����ϱ� ���ؼ� ���� ��� �Լ��� �������־���.
	static void SaveGameObject(CGameObject* _Object, YAML::Emitter& _emitter);
	static CGameObject* LoadGameObject(YAML::Node& _node);

private:
	CSaveLoadMgr();
	~CSaveLoadMgr();
	friend class CSingleton<CSaveLoadMgr>;
};

