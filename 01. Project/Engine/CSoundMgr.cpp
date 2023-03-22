#include "pch.h"
#include "CSoundMgr.h"

#include "CGameObject.h"
#include "CTransform.h"

CSoundMgr::CSoundMgr()
{
}

CSoundMgr::~CSoundMgr()
{
}

void CSoundMgr::pushToSoundlist(FMOD::Channel* _channel, CGameObject* _Obj)
{
	m_mSoundlist.insert(make_pair(_channel, _Obj));
}

void CSoundMgr::eraseFromSoundlist(FMOD::Channel* _channel)
{
	map<FMOD::Channel*, CGameObject*>::iterator iter = m_mSoundlist.find(_channel);
	if (iter == m_mSoundlist.end())
	{
		return;
	}

	m_mSoundlist.erase(iter);
}

void CSoundMgr::tick()
{
	map<FMOD::Channel*, CGameObject*>::iterator iter = m_mSoundlist.begin();

	for (; iter != m_mSoundlist.end(); ++iter)
	{
		FMOD::Channel* pChannel = iter->first;
		CGameObject* pObj = iter->second;

		pChannel->set3DAttributes((FMOD_VECTOR*)(pObj->Transform()->GetRelativePosPointer()), 0);
	}
}