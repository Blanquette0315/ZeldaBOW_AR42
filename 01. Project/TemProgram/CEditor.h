#pragma once

class CGameObjectEX;

class CEditor
	: public CSingleton<CEditor>
{
private:
	vector<CGameObjectEX*>	m_vecEditorObj;

	list<tDebugShapeInfo>	m_DebugDrawList; // ���ӽð��� �ִ� �ӽ� ��ü ����
	CGameObjectEX*			m_DebugDrawObject[(UINT)DEBUG_SHAPE::END];

public:
	void init();
	void progress();

private:
	void tick();
	void render();
	void render_debug();

private:
	void CreateDebugDrawObject();
	void DebugDraw(tDebugShapeInfo& _info);

public:
	CEditor();
	~CEditor();
};

