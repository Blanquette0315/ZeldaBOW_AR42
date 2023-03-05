#pragma once
#include "CScript.h"

class CPlayerScript :
    public CScript
{
private:
	float m_fAccTime;

public:
	virtual void begin() override;
	virtual void tick() override;

	virtual void BeginOverlap(CCollider2D* _pOther) override;
	virtual void Overlap(CCollider2D* _pOther) override;
	virtual void EndOverlap(CCollider2D* _pOther) override;

	CLONE(CPlayerScript);
public:
	CPlayerScript();
	~CPlayerScript();
};