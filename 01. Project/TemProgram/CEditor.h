#pragma once

class CGameObjectEX;

class CEditor
	: public CSingleton<CEditor>
{
private:
	vector<CGameObjectEX*>	m_vecEditorObj;

	list<tDebugShapeInfo>	m_DebugDrawList; // 지속시간이 있는 임시 물체 관리
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

