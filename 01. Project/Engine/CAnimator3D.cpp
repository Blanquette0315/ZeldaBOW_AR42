#include "pch.h"

#include "CAnimator3D.h"
#include "CAnimation3D.h"

#include "CTimeMgr.h"
#include "CMeshRender.h"
#include "CStructuredBuffer.h"
#include "CResMgr.h"

#include "CAnimation3DShader.h"

#include "CKeyMgr.h"


CAnimator3D::CAnimator3D()
	: m_pVecBones(nullptr)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_iFrmlimit(0)
	, m_pCurAnim(nullptr)
	, m_pCurAnimLower(nullptr)
	, m_bRepeat(false)
	, m_bRepeatLower(false)
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer;
}

CAnimator3D::CAnimator3D(const CAnimator3D& _origin)
	: m_pVecBones(_origin.m_pVecBones)
	, m_pBoneFinalMatBuffer(nullptr)
	, m_bFinalMatUpdate(false)
	, m_iFrmlimit(_origin.m_iFrmlimit)
	, m_pCurAnim(nullptr)
	, m_pCurAnimLower(nullptr)
	, m_bRepeat(false)
	, m_bRepeatLower(false)
	, CComponent(COMPONENT_TYPE::ANIMATOR3D)
{
	m_pBoneFinalMatBuffer = new CStructuredBuffer;

	map<wstring, CAnimation3D*>::const_iterator iter = _origin.m_mapAnim.begin();
	for (; iter != _origin.m_mapAnim.end(); ++iter)
	{
		CreateAnimation(iter->first, &(iter->second->m_sClip), iter->second->m_iFrameCount);
	}
}

CAnimator3D::~CAnimator3D()
{
	SAFE_DELETE(m_pBoneFinalMatBuffer);
	Safe_Del_Map(m_mapAnim);
}


void CAnimator3D::finaltick()
{
	//wstring strFileName = CPathMgr::GetInst()->GetContentPath();
	//strFileName += L"LinkBoneName.txt";
	//std::ofstream ofs(strFileName, std::ios::out);

	//if (!m_pVecBones->empty())
	//{
	//	for (int i = 0; i < m_pVecBones->size(); ++i)
	//	{
	//		ofs << std::to_wstring(i) << " : " << m_pVecBones->at(i).strBoneName << std::endl;
	//	}
	//}

	//ofs.close();

	if (IsValid(m_pCurAnim))
	{
		// 애니메이션이 Finish 상태이고, 반복 재생일 경우
		if (m_pCurAnim->m_bFinish && m_bRepeat)
		{
			// 다시 0프레임으로 리셋시켜 동작하게 한다.
			m_pCurAnim->Reset();
		}

		m_pCurAnim->finaltick();
	}

	if (IsValid(m_pCurAnimLower))
	{
		if (m_pCurAnimLower->m_bFinish && m_bRepeatLower)
		{
			// 다시 0프레임으로 리셋시켜 동작하게 한다.
			m_pCurAnimLower->Reset();
		}

		m_pCurAnimLower->finaltick();
	}

	// 컴퓨트 쉐이더 연산여부
	m_bFinalMatUpdate = false;
}

void CAnimator3D::SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip)
{
	vector<tMTAnimClip>::const_iterator iter = _vecAnimClip->begin();

	for (; iter != _vecAnimClip->end(); ++iter)
	{
		if (m_iFrmlimit < iter->iEndFrame)
		{
			m_iFrmlimit = iter->iEndFrame;
		}

		CreateAnimation(iter->strAnimName, &(*iter));
	}
}


void CAnimator3D::UpdateData()
{
	if (!IsValid(m_pCurAnim))
	{
		// Animation3D Update Compute Shader
		CAnimation3DShader* pUpdateShader = (CAnimation3DShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"Animation3DUpdateShader").Get();

		// Bone Data
		Ptr<CMesh> pMesh = MeshRender()->GetMesh();
		check_mesh(pMesh);

		pUpdateShader->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer());
		pUpdateShader->SetOffsetMatBuffer(pMesh->GetBoneOffsetBuffer());
		pUpdateShader->SetOutputBuffer(m_pBoneFinalMatBuffer);

		UINT iBoneCount = (UINT)m_pVecBones->size();
		pUpdateShader->SetBoneCount(iBoneCount);
		pUpdateShader->SetFrameIndex(0);
		pUpdateShader->SetNextFrameIdx(0);
		pUpdateShader->SetFrameRatio(0);
		pUpdateShader->SetExtraAnimBool(0);

		// 업데이트 쉐이더 실행
		pUpdateShader->Execute();

		m_bFinalMatUpdate = true;
	}
	else if (!m_bFinalMatUpdate)
	{
		// Animation3D Update Compute Shader
		CAnimation3DShader* pUpdateShader = (CAnimation3DShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"Animation3DUpdateShader").Get();

		// Bone Data
		Ptr<CMesh> pMesh = MeshRender()->GetMesh();
		check_mesh(pMesh);

		pUpdateShader->SetFrameDataBuffer(pMesh->GetBoneFrameDataBuffer());
		pUpdateShader->SetOffsetMatBuffer(pMesh->GetBoneOffsetBuffer());
		pUpdateShader->SetOutputBuffer(m_pBoneFinalMatBuffer);

		UINT iBoneCount = (UINT)m_pVecBones->size();
		pUpdateShader->SetBoneCount(iBoneCount);
		pUpdateShader->SetFrameIdxRatio(m_pCurAnim->m_iFrameIdx, m_pCurAnim->m_fRatio);

		if (m_pCurAnimLower)
		{
			pUpdateShader->SetFrameIdxRatioLower(m_pCurAnimLower->m_iFrameIdx, m_pCurAnimLower->m_fRatio);
			pUpdateShader->SetBoneDividedPoint(m_iBoneDivPoint);
		}
		else
		{
			pUpdateShader->SetBoneDividedPoint(0);
		}
		// 업데이트 쉐이더 실행
		pUpdateShader->Execute();
		m_bFinalMatUpdate = true;
	}

	// t30 레지스터에 최종행렬 데이터(구조버퍼) 바인딩		
	m_pBoneFinalMatBuffer->UpdateData(30, PIPELINE_STAGE::VS);
}

const tMTBone& CAnimator3D::GetBoneByName(const wstring& _strBoneName)
{
	for (size_t i = 0; i < m_pVecBones->size(); ++i)
	{
		if ((*m_pVecBones)[i].strBoneName == _strBoneName)
			return (*m_pVecBones)[i];
	}
}

void CAnimator3D::ClearData()
{
	m_pBoneFinalMatBuffer->Clear();

	UINT iMtrlCount = MeshRender()->GetMtrlCount();
	Ptr<CMaterial> pMtrl = nullptr;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		pMtrl = MeshRender()->GetSharedMaterial(i);
		if (nullptr == pMtrl)
			continue;

		pMtrl->SetAnim3D(false); // Animation Mesh 알리기
		pMtrl->SetBoneCount(0);
	}
}

void CAnimator3D::CreateAnimation(const wstring& _strKey, int _iStartFrame, int _iEndFrame, int _iFrameCount)
{
	CAnimation3D* pAnim = FindAnimation(_strKey);
	assert(!pAnim);

	// 새로운 애니메이션 추가
	pAnim = new CAnimation3D;
	pAnim->m_pOwner = this;
	pAnim->Create(_strKey, _iStartFrame, _iEndFrame, _iFrameCount);
	pAnim->m_iFrmlimit = m_iFrmlimit;
	m_mapAnim.insert(make_pair(_strKey, pAnim));
}

void CAnimator3D::CreateAnimation(const wstring& _strKey, const tMTAnimClip* _pClip, int _iFrameCount)
{
	CAnimation3D* pAnim = FindAnimation(_strKey);
	assert(!pAnim);

	// 새로운 애니메이션 추가
	pAnim = new CAnimation3D;
	pAnim->m_pOwner = this;
	pAnim->Create(_strKey, _pClip, _iFrameCount);
	pAnim->m_iFrmlimit = m_iFrmlimit;
	m_mapAnim.insert(make_pair(_strKey, pAnim));
}

CAnimation3D* CAnimator3D::FindAnimation(const wstring& _strKey)
{
	map<wstring, CAnimation3D*>::iterator iter = m_mapAnim.find(_strKey);

	if (iter == m_mapAnim.end())
	{
		return nullptr;
	}

	return iter->second;
}

void CAnimator3D::Play(const wstring& _strKey, bool _bRepeat)
{
	// 실행할 애니메이션을 찾기
	CAnimation3D* pCurAnim = FindAnimation(_strKey);

	if (IsValid(pCurAnim))
	{
		m_pCurAnim = pCurAnim;
	}

	// 반복 여부 저장
	m_bRepeat = _bRepeat;

	// 실행할 애니메이션 리셋
	m_pCurAnim->Reset();
}

void CAnimator3D::PlayLowerAnim(const wstring& _strKey, bool _bRepeat)
{
	// 실행할 애니메이션을 찾기
	CAnimation3D* pCurLowerAnim = FindAnimation(_strKey);

	if (IsValid(pCurLowerAnim))
	{
		m_pCurAnimLower = pCurLowerAnim;
	}

	// 반복 여부 저장
	m_bRepeatLower = _bRepeat;

	// 실행할 애니메이션 리셋
	m_pCurAnimLower->Reset();
}

void CAnimator3D::ChangeAnimName(const wstring& _strOrignKey, const wstring& _strNewKey)
{
	CAnimation3D* pAnim = FindAnimation(_strNewKey);
	assert(!pAnim);
	
	pAnim = FindAnimation(_strOrignKey);
	if (nullptr != pAnim)
	{
		CAnimation3D* pNewAnim = pAnim->Clone();
		pNewAnim->SetName(_strNewKey);
		pNewAnim->m_sClip.strAnimName = _strNewKey;
		m_mapAnim.insert(make_pair(_strNewKey, pNewAnim));
		EraseAnimation(_strOrignKey);
		m_pCurAnim = FindAnimation(_strNewKey);
		m_pCurAnim->Reset();
	}
}

void CAnimator3D::EraseAnimation(const wstring& _strKey)
{
	if (m_pCurAnim != nullptr && m_pCurAnim->GetName() == _strKey)
		m_pCurAnim = nullptr;

	if (m_pCurAnimLower != nullptr && m_pCurAnimLower->GetName() == _strKey)
		m_pCurAnimLower = nullptr;

	map<wstring, CAnimation3D*>::iterator Target_iter = m_mapAnim.find(_strKey.c_str());
	assert(Target_iter != m_mapAnim.end());
	delete Target_iter->second;
	m_mapAnim.erase(Target_iter);
}

void CAnimator3D::CreateDefaultAnimation()
{
	int count = 0;
	wstring strName;
	while (true)
	{
		strName = L"New" + std::to_wstring(count++);
		if (FindAnimation(strName) == nullptr)
		{
			break;
		}
	}

	CreateAnimation(strName, 0, 0, 30);
}

void CAnimator3D::check_mesh(Ptr<CMesh> _pMesh)
{
	UINT iBoneCount = _pMesh->GetBoneCount();
	if (m_pBoneFinalMatBuffer->GetElementCount() != iBoneCount)
	{
		m_pBoneFinalMatBuffer->Create(sizeof(Matrix), iBoneCount, SB_TYPE::UAV_INC, nullptr, false);
	}
}

void CAnimator3D::CreateAnimation(CAnimation3D* _animation)
{
	// 새로운 애니메이션 추가
	_animation->m_pOwner = this;
	m_mapAnim.insert(make_pair(_animation->GetName(), _animation));
}

void CAnimator3D::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "ANIMATOR3D";
	_emitter << YAML::Value << YAML::BeginMap;

	_emitter << YAML::Key << "Frmlimit";
	_emitter << YAML::Value << m_iFrmlimit;

	size_t size = m_mapAnim.size();
	_emitter << YAML::Key << "AnimationCount";
	_emitter << YAML::Value << size;
	map<wstring, CAnimation3D*>::iterator iter = m_mapAnim.begin();
	int i = 0;
	for (; iter != m_mapAnim.end(); ++iter)
	{
		_emitter << YAML::Key << "Animation3D_" + std::to_string(i++);
		_emitter << YAML::Value << YAML::BeginMap;
		iter->second->SaveToYAML(_emitter);
		_emitter << YAML::EndMap;
	}

	_emitter << YAML::EndMap;
}

void CAnimator3D::LoadFromYAML(YAML::Node& _node)
{
	// MeshRender do this. if the animation clip changes, FrmLimit should change too.
	// SAFE_LOAD_FROM_YAML(int, m_iFrmlimit, _node["ANIMATOR3D"]["Frmlimit"]);

	size_t size = 0;
	SAFE_LOAD_FROM_YAML(size_t, size, _node["ANIMATOR3D"]["AnimationCount"]);

	for (size_t i = 0; i < size; ++i)
	{
		CAnimation3D* anim = new CAnimation3D;
		YAML::Node node = _node["ANIMATOR3D"]["Animation3D_" + std::to_string(i)];
		anim->LoadFromYAML(node);
		CreateAnimation(anim);
	}
}