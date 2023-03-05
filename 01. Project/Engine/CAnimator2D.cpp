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
	// �ִϸ��̼��� �ִٸ� �����ϰ� ���ٸ� �������� �ʴ´�.
	if (!IsValid(m_pCurAnim))
	{
		return;
	}

	// �ִϸ��̼��� Finish �����̰�, �ݺ� ����� ���
	if (m_pCurAnim->m_bFinish && m_bRepeat)
	{
		// �ٽ� 0���������� ���½��� �����ϰ� �Ѵ�.
		m_pCurAnim->Reset();
	}

	// �ִϸ��̼� ������Ʈ
	m_pCurAnim->finaltick();
}

void CAnimator2D::CreateAnimation(const wstring& _strKey, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS)
{
	// ��Ʋ�� �ؽ�ó�� �ִ��� Ȯ��
	assert(_AtlasTex.Get());

	// ���ڷ� ���� Ű������ �̹� �ִϸ��̼��� �����ϴ��� Ȯ��
	CAnimation2D* pAnim = FindAnimation(_strKey);
	assert(!pAnim);

	// ���ο� �ִϸ��̼� �߰�
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
	// ���� Ű������ ����� �ִϸ��̼��� ����� �ȴ�.
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
	// ��Ʋ�� �ؽ�ó�� �ִ��� Ȯ��
	assert(_AtlasTex.Get());

	// ���ڷ� ���� Ű������ �̹� �ִϸ��̼��� �����ϴ��� Ȯ��
	CAnimation2D* pAnim = FindAnimation(_strKey);
	assert(!pAnim);
	pAnim = new CAnimation2D;
	pAnim->Create(_strKey, _AtlasTex, _vecFrm);

	pAnim->m_pOwner = this;
	m_mapAnim.insert(make_pair(_strKey, pAnim));
}

void CAnimator2D::CreatebeginAnimation()
{
	// �ӽ÷� ������ �ε��ϴ� NoiseTexture�� �־��ش�.
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
	// ������ �ִϸ��̼��� ã��
	CAnimation2D* pCurAnim = FindAnimation(_strKey);

	if (IsValid(pCurAnim))
	{
		m_pCurAnim = pCurAnim;
	}

	// �ݺ� ���� ����
	m_bRepeat = _bRepeat;

	// ������ �ִϸ��̼� ����
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

	// Animation ����
	// ������� Level�� Play���¸� �����ϰ�� �ִϸ��̼��� �÷��� �Ǹ� �ȵȴ�.
	// ������ Stop���� �϶�, �ִϸ��̼��� ������� ������, ������ �� �����ϴ�.
	// ���� ������ ���� �ִϸ��̼� �ʱⰪ�� ����ϰ�, �ش� �ִϸ����͸� ����ϴ� Script�ʿ��� � ��Ȳ�� �� �
	// �ִϸ��̼��� �����ϰ� �ݺ����� ������ ���ϵ��� ������ ��Ҵ�.
	size_t iAnimCount = m_mapAnim.size();
	fwrite(&iAnimCount, sizeof(size_t), 1, _File);

	for (const auto& pair : m_mapAnim)
	{
		pair.second->SaveToFile(_File);
	}
}

void CAnimator2D::LoadFromFile(FILE* _File)
{
	// Animation �ε�
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
