#include "pch.h"
#include "CSound.h"

#include "CResMgr.h"
#include "CSoundMgr.h"
#include "CGameObject.h"
#include "CTransform.h"

FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
	, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
	, void* commanddata1, void* commanddata2);

FMOD::System* CSound::g_pFMOD = nullptr;

CSound::CSound()
	: CRes(RES_TYPE::SOUND)
	, m_pSound(nullptr)
{
}

CSound::~CSound()
{
	if (nullptr != m_pSound)
	{
		m_pSound->release();
		m_pSound = nullptr;
	}
}

int CSound::Play(int _iRoopCount, float _fVolume, bool _bOverlap, CGameObject* _pObj, Vec3 _vPos, Vec2 _v3DMinMaxDistance)
{
	if (_iRoopCount <= -1)
	{
		assert(nullptr);
	}

	// 재생되고 있는 채널이 있는데, 중복재생을 허용하지 않았다 -> 재생 안함
	if (!_bOverlap && !m_listChannel.empty())
	{
		return -1;
	}

	_iRoopCount -= 1;

	FMOD::Channel* pChannel = nullptr;
	g_pFMOD->playSound(m_pSound, nullptr, false, &pChannel);

	pChannel->setVolume(_fVolume);

	pChannel->setCallback(CHANNEL_CALLBACK);
	pChannel->setUserData(this);

	pChannel->setMode(FMOD_LOOP_NORMAL);
	pChannel->setLoopCount(_iRoopCount);

	pChannel->set3DMinMaxDistance(_v3DMinMaxDistance.x, _v3DMinMaxDistance.y);

	if (_pObj == nullptr)
	{
		if (_vPos != Vec3::Zero)
		{
			pChannel->set3DAttributes((FMOD_VECTOR*)(&_vPos), 0);
		}
	}
	else
	{
		Vec3 vObjPos = _pObj->Transform()->GetWorldPos();
		pChannel->set3DAttributes((FMOD_VECTOR*)(&vObjPos), 0);
		CSoundMgr::GetInst()->pushToSoundlist(pChannel, _pObj);
	}
	m_listChannel.push_back(pChannel);

	int iIdx = -1;
	pChannel->getIndex(&iIdx);

	return iIdx;
}

int CSound::PlayBGM(float _fVolume)
{
	int iRoopCount = -1;

	if (!m_listChannel.empty())
	{
		return -1;
	}

	FMOD::Channel* pChannel = nullptr;
	g_pFMOD->playSound(m_pSound, nullptr, false, &pChannel);

	pChannel->setVolume(_fVolume);

	pChannel->setCallback(CHANNEL_CALLBACK);
	pChannel->setUserData(this);

	pChannel->setMode(FMOD_LOOP_NORMAL);
	pChannel->setLoopCount(iRoopCount);
	pChannel->set3DMinMaxDistance(100000.f, 100000.f);
	m_listChannel.push_back(pChannel);

	int iIdx = -1;
	pChannel->getIndex(&iIdx);

	return iIdx;
}

void CSound::Stop()
{
	list<FMOD::Channel*>::iterator iter;

	while (!m_listChannel.empty())
	{
		iter = m_listChannel.begin();
		(*iter)->stop();
	}
}

void CSound::SetVolume(float _f, int _iChannelIdx)
{
	list<FMOD::Channel*>::iterator iter = m_listChannel.begin();

	int iIdx = -1;
	for (; iter != m_listChannel.end(); ++iter)
	{
		(*iter)->getIndex(&iIdx);
		if (_iChannelIdx == iIdx)
		{
			(*iter)->setVolume(_f);
			return;
		}
	}
}

void CSound::RemoveChannel(FMOD::Channel* _pTargetChannel)
{
	CSoundMgr::GetInst()->eraseFromSoundlist(_pTargetChannel);

	list<FMOD::Channel*>::iterator iter = m_listChannel.begin();
	for (; iter != m_listChannel.end(); ++iter)
	{
		if (*iter == _pTargetChannel)
		{
			m_listChannel.erase(iter);
			return;
		}
	}
}

int CSound::Load(const wstring& _strFilePath)
{
	string path(_strFilePath.begin(), _strFilePath.end());

	if (FMOD_OK != g_pFMOD->createSound(path.c_str(), FMOD_3D|FMOD_3D_LINEARSQUAREROLLOFF, nullptr, &m_pSound))
	{
		assert(nullptr);
	}

	return S_OK;
}



// =========
// Call Back
// =========
FMOD_RESULT CHANNEL_CALLBACK(FMOD_CHANNELCONTROL* channelcontrol, FMOD_CHANNELCONTROL_TYPE controltype
	, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
	, void* commanddata1, void* commanddata2)
{
	FMOD::Channel* cppchannel = (FMOD::Channel*)channelcontrol;
	CSound* pSound = nullptr;

	switch (controltype)
	{
	case FMOD_CHANNELCONTROL_CALLBACK_END:
	{
		cppchannel->getUserData((void**)&pSound);
		pSound->RemoveChannel(cppchannel);
	}
	break;
	}

	return FMOD_OK;
}