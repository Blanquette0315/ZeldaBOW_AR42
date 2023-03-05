#include "pch.h"
#include "CAnimator2D.h"
#include "CAnimation2D.h"

CAnimator2D::CAnimator2D()
	: CComponent(COMPONENT_TYPE::ANIMATOR2D)
	, m_pCurAnim(nullptr)
	, m_bRepeat(false)
{
}

CAnimator2D::CAnimator2D(const CAnimator2D& _origin)
	: CComponent(COMPONENT_TYPE::ANIMATOR2D)
	, m_pCurAnim(nullptr)
	, m_bRepeat(false)
{
	map<wstring, CAnimation2D*>::const_iterator iter = _origin.m_mapAnim.begin();
	for (; iter != _origin.m_mapAnim.end(); ++iter)
	{
		CreateAnimation(iter->first, iter->second->GetAtlas(), iter->second->GetvecFrm());
	}
	
	//m_pCurAnim = m_mapAnim.find(_origin.m_pCurAnim->GetName())->second;
}

CAnimator2D::~CAnimator2D()
{
	Safe_Del_Map(m_mapAnim);
}

void CAnimator2D::begin()
{
	//m_pCurAnim = nullptr;
	m_bRepeat = false;
}

void CAnimator2D::finaltick()
{
	// 애니메이션이 있다면 동작하고 없다면 동작하지 않는다.
	if (!IsValid(m_pCurAnim))
	{
		return;
	}

	// 애니메이션이 Finish 상태이고, 반복 재생일 경우
	if (m_pCurAnim->m_bFinish && m_bRepeat)
	{
		// 다시 0프레임으로 리셋시켜 동작하게 한다.
		m_pCurAnim->Reset();
	}

	// 애니메이션 업데이트
	m_pCurAnim->finaltick();
}

void CAnimator2D::CreateAnimation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS)
{
	// 아틀라스 텍스처가 있는지 확인
	assert(_AtlasTex.Get());

	// 인자로 받은 키값으로 이미 애니메이션이 존재하는지 확인
	CAnimation2D* pAnim = FindAnimation(_strKey);
	assert(!pAnim);

	// 새로운 애니메이션 추가
	pAnim = new CAnimation2D;
	pAnim->Create(_strKey, _AtlasTex, _vLeftTop, _vSlice, _fStep, _iMaxFrm, _FPS);

	pAnim->m_pOwner = this;
	m_mapAnim.insert(make_pair(_strKey, pAnim));
}


int CAnimator2D::GetCurAnimIdx()
{
	return m_pCurAnim->GetCurIdx();
}

void CAnimator2D::EraseAnimation(const wstring& _strKey)
{
	CAnimation2D* pAnim = FindAnimation(_strKey);
	assert(pAnim);

	map<wstring, CAnimation2D*>::iterator Targetiter = m_mapAnim.find(_strKey.c_str());
	delete Targetiter->second;
	m_mapAnim.erase(Targetiter);
}

void CAnimator2D::ChangeAnimName(const wstring& _strOrignKey, const wstring& _strNewKey)
{
	CAnimation2D* pTargetAnim = FindAnimation(_strOrignKey);
	if (nullptr != pTargetAnim)
	{
		CAnimation2D* pNewAnim = pTargetAnim->Clone();
		pNewAnim->SetName(_strNewKey);
		RegisterAnimation(_strNewKey, pNewAnim);
		EraseAnimation(_strOrignKey);
		m_pCurAnim = FindAnimation(_strNewKey);
	}
}

void CAnimator2D::RegisterAnimation(const wstring& _strKey, CAnimation2D* _TargetAnim)
{
	// 들어온 키값으로 저장된 애니메이션이 없어야 된다.
	if (nullptr == FindAnimation(_strKey))
	{
		m_mapAnim.insert(make_pair(_strKey, _TargetAnim));
	}
}

void CAnimator2D::OverWriteAnimation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, const vector<tAnim2DFrm>& _vecFrm)
{
	EraseAnimation(_strKey);
	CreateAnimation(_strKey, _AtlasTex, _vecFrm);
	m_pCurAnim = FindAnimation(_strKey);
}

void CAnimator2D::CreateAnimation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, const vector<tAnim2DFrm>& _vecFrm)
{
	// 아틀라스 텍스처가 있는지 확인
	assert(_AtlasTex.Get());

	// 인자로 받은 키값으로 이미 애니메이션이 존재하는지 확인
	CAnimation2D* pAnim = FindAnimation(_strKey);
	assert(!pAnim);
	pAnim = new CAnimation2D;
	pAnim->Create(_strKey, _AtlasTex, _vecFrm);

	pAnim->m_pOwner = this;
	m_mapAnim.insert(make_pair(_strKey, pAnim));
}

void CAnimator2D::CreatebeginAnimation()
{
	// 임시로 무조건 로딩하는 NoiseTexture를 넣어준다.
	Ptr<CTexture> _AtlasTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\noise\\TestNoise.png");

	CreateAnimation(L"New Animation", _AtlasTex, Vec2(0.f,0.f), Vec2(10.f,10.f), 10.f, 1.f, 1.f);
	m_pCurAnim = FindAnimation(L"New Animation");
	m_pCurAnim->m_iCurIdx = 0;
}

CAnimation2D* CAnimator2D::FindAnimation(const wstring& _strKey)
{
	map<wstring, CAnimation2D*>::iterator iter = m_mapAnim.find(_strKey);

	if (iter == m_mapAnim.end())
	{
		return nullptr;
	}

	return iter->second;
}

void CAnimator2D::Play(const wstring& _strKey, bool _bRepeat)
{
	// 실행할 애니메이션을 찾기
	CAnimation2D* pCurAnim = FindAnimation(_strKey);

	if (IsValid(pCurAnim))
	{
		m_pCurAnim = pCurAnim;
	}

	// 반복 여부 저장
	m_bRepeat = _bRepeat;

	// 실행할 애니메이션 리셋
	m_pCurAnim->Reset();
}

void CAnimator2D::UpdateData()
{
	if (!IsValid(m_pCurAnim))
		return;

	m_pCurAnim->UpdateData();
}

void CAnimator2D::Clear()
{
	if (!IsValid(m_pCurAnim))
	{
		return;
	}

	m_pCurAnim->Clear();
}







void CAnimator2D::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE type = GetType();
	fwrite(&type, sizeof(UINT), 1, _File);

	// Animation 저장
	// 원래라면 Level이 Play상태를 제외하고는 애니메이션이 플레이 되면 안된다.
	// 하지만 Stop상태 일때, 애니메이션이 재생되지 않으면, 제작할 때 불편하다.
	// 따라서 저장할 때는 애니메이션 초기값만 사용하고, 해당 애니메이터를 사용하는 Script쪽에서 어떤 상황일 때 어떤
	// 애니메이션을 실행하고 반복할지 말지를 정하도록 구조를 잡았다.
	size_t iAnimCount = m_mapAnim.size();
	fwrite(&iAnimCount, sizeof(size_t), 1, _File);

	for (const auto& pair : m_mapAnim)
	{
		pair.second->SaveToFile(_File);
	}
}

void CAnimator2D::LoadFromFile(FILE* _File)
{
	// Animation 로드
	size_t iAnimCount = 0;
	fread(&iAnimCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < iAnimCount; ++i)
	{
		CAnimation2D* pAnim = new CAnimation2D;
		pAnim->LoadFromFile(_File);

		pAnim->m_pOwner = this;
		m_mapAnim.insert(make_pair(pAnim->GetName(), pAnim));
	}
}
