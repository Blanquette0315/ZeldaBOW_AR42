#include "pch.h"
#include "CAnimation2D.h"

#include "CDevice.h"
#include "CConstBuffer.h"

#include "CTimeMgr.h"
#include "CTexture.h"

CAnimation2D::CAnimation2D()
	: m_iCurIdx(-1)
	, m_pOwner(nullptr)
	, m_fAcctime(0.f)
{
}

CAnimation2D::~CAnimation2D()
{
}

void CAnimation2D::finaltick()
{
	// 애니메이션이 끝났으면 fianltick을 돌리지 않는다.
	if (m_bFinish)
		return;

	// 시간 누적
	m_fAcctime += DT;

	// 누적 시간이 해당 프레임 유지시간을 넘어서면 다음 프레임으로 넘어간다.
	if (m_fAcctime > m_vecFrm[m_iCurIdx].fDuration)
	{
		m_fAcctime = 0.f;
		++m_iCurIdx;

		// 최대 프레임에 도달하면 Finish 상태로 전환한다.
		if (m_vecFrm.size() <= m_iCurIdx)
		{
			m_iCurIdx = (int)m_vecFrm.size() - 1;
			m_bFinish = true;
		}
	}
}

void CAnimation2D::Create(const wstring& _Key, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS)
{
	// Animation 이름은 본인의 키값으로 설정한다.
	SetName(_Key);

	// Atlas Texture
	m_AtlasTex = _AtlasTex;

	// Pixel좌표를 UV좌표로 바꾸기 위해 비율로 나누기 위해 구한 Texture의 해상도
	float TexWidth = (float)m_AtlasTex->GetWidth();
	float TexHeight = (float)m_AtlasTex->GetHeight();

	// tAnim2DFrm (Animation Frame Info) 채우기
	for (UINT i = 0; i < _iMaxFrm; ++i)
	{
		tAnim2DFrm TemAnim2DFrm = {};

		// 아틀라스 이미지의 해당 인덱스 애니메이션 장면 Pixel 좌표를 UV좌표로 바꾸어 구조체에 넣어준다.
		TemAnim2DFrm.vLeftTop = Vec2((_vLeftTop.x + _fStep * (float)i) / TexWidth, _vLeftTop.y / TexHeight);
		TemAnim2DFrm.vSlice = Vec2(_vSlice.x / TexWidth, _vSlice.y / TexHeight);
		TemAnim2DFrm.fDuration = 1.f / _FPS;
		
		TemAnim2DFrm.vFullsize = Vec2(200.f / TexWidth, 200.f / TexHeight);

		m_vecFrm.push_back(TemAnim2DFrm);
	}
}

void CAnimation2D::Create(const wstring& _strKey, Ptr<CTexture> _AtlasTex, const vector<tAnim2DFrm>& _vecFrm)
{
	// Animation 이름은 본인의 키값으로 설정한다.
	SetName(_strKey);

	m_iCurIdx = 0;

	// Atlas Texture
	m_AtlasTex = _AtlasTex;

	for (UINT i = 0; i < _vecFrm.size(); ++i)
	{
		tAnim2DFrm TemAnim2DFrm = {};

		TemAnim2DFrm.vLeftTop = _vecFrm[i].vLeftTop;
		TemAnim2DFrm.vSlice = _vecFrm[i].vSlice;
		TemAnim2DFrm.vOffset = _vecFrm[i].vOffset;
		TemAnim2DFrm.vFullsize = _vecFrm[i].vFullsize;
		TemAnim2DFrm.fDuration = _vecFrm[i].fDuration;

		// TemAnim2DFrm.vFullsize = Vec2(200.f / TexWidth, 200.f / TexHeight);
		m_vecFrm.push_back(TemAnim2DFrm);
	}
}


void CAnimation2D::UpdateData()
{
	m_AtlasTex->UpdateData(12, PIPELINE_STAGE::PS);

	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::ANIMATION2D);

	tAnim2DInfo TemAnim2DInfo = {};
	TemAnim2DInfo.iAnim2DUse = 1;
	TemAnim2DInfo.vLeftTop = m_vecFrm[m_iCurIdx].vLeftTop;
	TemAnim2DInfo.vSlice = m_vecFrm[m_iCurIdx].vSlice;
	TemAnim2DInfo.vFullSize = m_vecFrm[m_iCurIdx].vFullsize;
	TemAnim2DInfo.vOffset = m_vecFrm[m_iCurIdx].vOffset;

	pCB->SetData(&TemAnim2DInfo);
	pCB->UpdateData(PIPELINE_STAGE::PS);
}

void CAnimation2D::Clear()
{
	// Texture 클리어
	CTexture::Clear(12);

	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::ANIMATION2D);

	// 애니메이션2D 구조체의 사용 용도를 0으로 초기화해서 ConstantBuffer로 전달
	tAnim2DInfo TemAnim2DInfo = {};
	TemAnim2DInfo.iAnim2DUse = 0;

	pCB->SetData(&TemAnim2DInfo);
	pCB->UpdateData(PIPELINE_STAGE::PS);
}








void CAnimation2D::SaveToFile(FILE* _File)
{
	// 애니메이션 이름 저장
	// 애니메이션의 경우 본인의 이름이 곧 에니메이터에 등록하는 키값이기 때문에 키값을 저장하는 것과 같다.
	CEntity::SaveToFile(_File);

	// 프레임 개수, 데이터 저장
	// 프레임 vector는 모두 순수 데이터기 때문에 통으로 저장한다.
	// 현재 인덱스의 경우 로딩해서 처음 사용하면 무조건 0번부터 실행할 것이기 때문에 저장하지 않는다.
	size_t iFrameCount = m_vecFrm.size();
	fwrite(&iFrameCount, sizeof(size_t), 1, _File);
	fwrite(m_vecFrm.data(), sizeof(tAnim2DFrm), iFrameCount, _File);

	// 참조 아틀라스 텍스처
	SaveResourceRef<CTexture>(m_AtlasTex, _File);
}

void CAnimation2D::LoadFromFile(FILE* _File)
{
	// 애니메이션 이름 로딩
	CEntity::LoadFromFile(_File);

	// 프레임 개수, 데이터 로딩
	size_t iFrameCount = 0;
	fread(&iFrameCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < iFrameCount; ++i)
	{
		tAnim2DFrm frm = {};
		fread(&frm, sizeof(tAnim2DFrm), 1, _File);
		m_vecFrm.push_back(frm);
	}

	// 참조 아틀라스 로딩
	LoadResourceRef<CTexture>(m_AtlasTex, _File);
}
