#pragma once
#include "singleton.h"

class CSoundMgr
	: public CSingleton<CSoundMgr>
{
private:
	map<FMOD::Channel*, CGameObject*> m_mSoundlist;

public:
	void pushToSoundlist(FMOD::Channel* _channel, CGameObject* _Obj);
	void eraseFromSoundlist(FMOD::Channel* _channel);
public:
	void tick();

public:
	CSoundMgr();
	~CSoundMgr();
};

