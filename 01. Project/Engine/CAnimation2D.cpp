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
	, m_bFinish(false)
{
}

CAnimation2D::~CAnimation2D()
{
}

void CAnimation2D::finaltick()
{
	// �ִϸ��̼��� �������� fianltick�� ������ �ʴ´�.
	if (m_bFinish)
		return;

	// �ð� ����
	m_fAcctime += FDT;

	// ���� �ð��� �ش� ������ �����ð��� �Ѿ�� ���� ���������� �Ѿ��.
	if (m_fAcctime > m_vecFrm[m_iCurIdx].fDuration)
	{
		m_fAcctime = 0.f;
		++m_iCurIdx;

		// �ִ� �����ӿ� �����ϸ� Finish ���·� ��ȯ�Ѵ�.
		if (m_vecFrm.size() <= m_iCurIdx)
		{
			m_iCurIdx = (int)m_vecFrm.size() - 1;
			m_bFinish = true;
		}
	}
}

void CAnimation2D::Create(const wstring& _Key, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, float _fStep, int _iMaxFrm, float _FPS)
{
	// Animation �̸��� ������ Ű������ �����Ѵ�.
	SetName(_Key);

	// Atlas Texture
	m_AtlasTex = _AtlasTex;

	// Pixel��ǥ�� UV��ǥ�� �ٲٱ� ���� ������ ������ ���� ���� Texture�� �ػ�
	float TexWidth = (float)m_AtlasTex->GetWidth();
	float TexHeight = (float)m_AtlasTex->GetHeight();

	// tAnim2DFrm (Animation Frame Info) ä���
	for (int i = 0; i < _iMaxFrm; ++i)
	{
		tAnim2DFrm TemAnim2DFrm = {};

		// ��Ʋ�� �̹����� �ش� �ε��� �ִϸ��̼� ��� Pixel ��ǥ�� UV��ǥ�� �ٲپ� ����ü�� �־��ش�.
		TemAnim2DFrm.vLeftTop = Vec2((_vLeftTop.x + _fStep * (float)i) / TexWidth, _vLeftTop.y / TexHeight);
		TemAnim2DFrm.vSlice = Vec2(_vSlice.x / TexWidth, _vSlice.y / TexHeight);
		TemAnim2DFrm.fDuration = 1.f / _FPS;
		
		TemAnim2DFrm.vFullsize = Vec2(200.f / TexWidth, 200.f / TexHeight);

		m_vecFrm.push_back(TemAnim2DFrm);
	}
}

void CAnimation2D::Create(const wstring& _strKey, Ptr<CTexture> _AtlasTex, const vector<tAnim2DFrm>& _vecFrm)
{
	// Animation �̸��� ������ Ű������ �����Ѵ�.
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
	// Texture Ŭ����
	CTexture::Clear(12);

	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::ANIMATION2D);

	// �ִϸ��̼�2D ����ü�� ��� �뵵�� 0���� �ʱ�ȭ�ؼ� ConstantBuffer�� ����
	tAnim2DInfo TemAnim2DInfo = {};
	TemAnim2DInfo.iAnim2DUse = 0;

	pCB->SetData(&TemAnim2DInfo);
	pCB->UpdateData(PIPELINE_STAGE::PS);
}








//void CAnimation2D::SaveToFile(FILE* _File)
//{
//	// �ִϸ��̼� �̸� ����
//	// �ִϸ��̼��� ��� ������ �̸��� �� ���ϸ����Ϳ� ����ϴ� Ű���̱� ������ Ű���� �����ϴ� �Ͱ� ����.
//	CEntity::SaveToFile(_File);
//
//	// ������ ����, ������ ����
//	// ������ vector�� ��� ���� �����ͱ� ������ ������ �����Ѵ�.
//	// ���� �ε����� ��� �ε��ؼ� ó�� ����ϸ� ������ 0������ ������ ���̱� ������ �������� �ʴ´�.
//	size_t iFrameCount = m_vecFrm.size();
//	fwrite(&iFrameCount, sizeof(size_t), 1, _File);
//	fwrite(m_vecFrm.data(), sizeof(tAnim2DFrm), iFrameCount, _File);
//
//	// ���� ��Ʋ�� �ؽ�ó
//	SaveResourceRef<CTexture>(m_AtlasTex, _File);
//}
//
//void CAnimation2D::LoadFromFile(FILE* _File)
//{
//	// �ִϸ��̼� �̸� �ε�
//	CEntity::LoadFromFile(_File);
//
//	// ������ ����, ������ �ε�
//	size_t iFrameCount = 0;
//	fread(&iFrameCount, sizeof(size_t), 1, _File);
//
//	for (size_t i = 0; i < iFrameCount; ++i)
//	{
//		tAnim2DFrm frm = {};
//		fread(&frm, sizeof(tAnim2DFrm), 1, _File);
//		m_vecFrm.push_back(frm);
//	}
//
//	// ���� ��Ʋ�� �ε�
//	LoadResourceRef<CTexture>(m_AtlasTex, _File);
//}
