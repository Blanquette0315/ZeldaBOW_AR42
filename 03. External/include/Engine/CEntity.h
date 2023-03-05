#pragma once

class CEntity
{
private:
	static UINT g_ID;

private:
	wstring m_strName;
	const UINT m_ID;

public:
	UINT GetID() { return m_ID; }
	void SetName(const wstring& _strName) { m_strName = _strName; }
	const wstring& GetName() { return m_strName; }

public:
	virtual void SaveToFile(FILE* _pFile);
	virtual void LoadFromFile(FILE* _pFile);

	virtual CEntity* Clone() = 0;

public:
	CEntity();
	CEntity(const CEntity& _other);
	virtual ~CEntity();
};

