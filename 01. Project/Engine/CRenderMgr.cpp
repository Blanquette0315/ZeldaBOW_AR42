#include "pch.h"
#include "CRenderMgr.h"

#include "GlobalComponent.h"
#include "CResMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

#include "CLevelMgr.h"
#include "CLevel.h"

#include "CMRT.h"

#include "CFontMgr.h"

CRenderMgr::CRenderMgr()
	: m_arrMRT{}
	, m_EditorCam(nullptr)
	, m_pLight2DBuffer(nullptr)
	, m_pLight3DBuffer(nullptr)
{
	m_pLight2DBuffer = new CStructuredBuffer();
	m_pLight2DBuffer->Create(sizeof(tLightInfo), 2, SB_TYPE::SRV_ONLY, nullptr, true);

	m_pLight3DBuffer = new CStructuredBuffer();
	m_pLight3DBuffer->Create(sizeof(tLightInfo), 2, SB_TYPE::SRV_ONLY, nullptr, true);
}

CRenderMgr::~CRenderMgr()
{
	if (nullptr != m_pLight2DBuffer)
		delete m_pLight2DBuffer;

	if (nullptr != m_pLight3DBuffer)
		delete m_pLight3DBuffer;

	Safe_Del_Array(m_arrMRT);
}

void CRenderMgr::tick()
{
	m_vecCam.clear();
	m_vecLight2D.clear();
	m_vecLight3D.clear();
	m_arrFont.clear();
}

void CRenderMgr::render()
{
	ClearMRT();
	
	//static Ptr<CTexture> pRTTex = CResMgr::GetInst()->FindRes<CTexture>(L"RenderTargetTex");
	//static Ptr<CTexture> pDSTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");
	//CONTEXT->OMSetRenderTargets(1, pRTTex->GetRTV().GetAddressOf(), pDSTex->GetDSV().Get());

	UpdateNoiseTexture();

	UpdateEffectTexture();

	UpdateLight2D();

	UpdateLight3D();

	static CConstBuffer* pGlobalCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	pGlobalCB->SetData(&g_global);
	pGlobalCB->UpdateData(PIPELINE_STAGE::ALL_STAGE);
	pGlobalCB->UpdateData_CS();

	CLevel* pCurLevel = CLevelMgr::GetInst()->GetCurLevel();

	// clear InFrustumObj vector before sort objs.
	CFrustum::ClearInFrustumObjs();
    
	if (GetMainCam() != nullptr)
	{
		Vec3 vCamPos = GetMainCam()->Transform()->GetWorldPos();
		Vec3 vCamDirFront = GetMainCam()->Transform()->GetWorldDir(DIR::FRONT);
		Vec3 vCamDirUp = GetMainCam()->Transform()->GetWorldDir(DIR::UP);
		CSound::g_pFMOD->set3DListenerAttributes(0, (FMOD_VECTOR*)(&vCamPos), 0, (FMOD_VECTOR*)(&vCamDirFront), (FMOD_VECTOR*)(&vCamDirUp));
	}

	if (LEVEL_STATE::PLAY == pCurLevel->GetState())
	{
		render_game();
	}
	else
	{
		render_editor();
	}

	// ��Ʈ ���
	for (size_t i = 0; i < m_arrFont.size(); ++i)
	{
		wchar_t szFloat[50] = {};
		swprintf_s(szFloat, 50, m_arrFont[i].strFont.c_str());
		CFontMgr::GetInst()->DrawFont(szFloat, m_arrFont[i].vecPos.x,
									m_arrFont[i].vecPos.y, m_arrFont[i].fSize,
									FONT_RGBA(m_arrFont[i].vecColor.x, m_arrFont[i].vecColor.y,
									m_arrFont[i].vecColor.z, m_arrFont[i].vecColor.w));
	}
}

void CRenderMgr::render_game()
{
	// Dir 광원 시점에서 Shadow맵핑을 위한 DepthMap 생성
	render_dynamic_shadowdepth();

	// 렌더링의 기준을 카메라로 설정
	for (size_t i = 0; i < m_vecCam.size(); ++i)
	{
		m_vecCam[i]->render();
	}
}

void CRenderMgr::render_editor()
{
	assert(m_EditorCam);

	// Dir 광원 시점에서 Shadow맵핑을 위한 DepthMap 생성
	render_dynamic_shadowdepth();

	m_EditorCam->render();
}

void CRenderMgr::render_dynamic_shadowdepth()
{
	m_arrMRT[(UINT)MRT_TYPE::SHADOW]->OMSet();

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		if (LIGHT_TYPE::DIRECTIONAL == m_vecLight3D[i]->GetLightType())
			m_vecLight3D[i]->render_depthmap();
	}
}

void CRenderMgr::UpdateNoiseTexture()
{
	Ptr<CTexture> NoiseTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\noise\\TestNoise.png");
	NoiseTex->UpdateData(15, PIPELINE_STAGE::ALL_STAGE);
	NoiseTex->UpdateData_CS(15, true);

	g_global.vNoiseResolution = Vec2(NoiseTex->GetWidth(), NoiseTex->GetHeight());
}

void CRenderMgr::UpdateLight2D()
{
	// ���ۿ� ������ ���ڶ� ��� ���Ҵ��Ͽ� ũ�⸦ �÷��ش�.
	if (m_pLight2DBuffer->GetElementCount() < (UINT)m_vecLight2D.size())
	{
		// ������ �������� CRenderMgr�� ������ �ִ� 2D������ �� ���ٸ�, 2D������ ������ŭ ���� ũ�⸦ �÷��� �����Ѵ�.
		m_pLight2DBuffer->Create(m_pLight2DBuffer->GetElementSize(), (UINT)m_vecLight2D.size(), SB_TYPE::SRV_ONLY, nullptr, true);
	}

	// ���� ������ ����ȭ ���۷� �ű��.
	vector<tLightInfo> vecLight2DInfo;
	vecLight2DInfo.clear();

	for (size_t i = 0; i < m_vecLight2D.size(); ++i)
	{
		vecLight2DInfo.push_back(m_vecLight2D[i]->GetLightInfo());
	}

	m_pLight2DBuffer->SetData(vecLight2DInfo.data(), (UINT)vecLight2DInfo.size());

	// ����ȭ ���۸� t13 �������Ϳ� ���ε�
	m_pLight2DBuffer->UpdateData(13, PIPELINE_STAGE::ALL_STAGE);

	// ���� ���� �۷ι� �����Ϳ� ������Ʈ
	g_global.iLight2DCount = (UINT)m_vecLight2D.size();
}

void CRenderMgr::UpdateLight3D()
{
	// ���� ������ ���ڶ� ��� ���Ҵ�
	if (m_pLight3DBuffer->GetElementCount() < (UINT)m_vecLight3D.size())
	{
		m_pLight3DBuffer->Create(m_pLight3DBuffer->GetElementSize(), (UINT)m_vecLight3D.size(), SB_TYPE::SRV_ONLY, nullptr, true);
	}

	// ���������� ����ȭ���۷� �ű�
	vector<tLightInfo> vecLight3DInfo;
	vecLight3DInfo.clear();

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		vecLight3DInfo.push_back(m_vecLight3D[i]->GetLightInfo());
	}

	m_pLight3DBuffer->SetData(vecLight3DInfo.data(), (UINT)vecLight3DInfo.size());

	// ����ȭ ���۸� t14 �������Ϳ� ���ε�
	m_pLight3DBuffer->UpdateData(14, PIPELINE_STAGE::ALL_STAGE);

	// ���� ���� �۷ι� �����Ϳ� ������Ʈ
	g_global.iLight3DCount = (UINT)m_vecLight3D.size();
}

void CRenderMgr::UpdateEffectTexture()
{
	Ptr<CTexture> BurnRampTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\burn\\BurnRamp.dds");
	BurnRampTex->UpdateData(16, PIPELINE_STAGE::ALL_STAGE);
	BurnRampTex->UpdateData_CS(16, true);
}

void CRenderMgr::CopyRenderTarget()
{
	// static�� ����ϸ� ó�� �ѹ��� ResMgr�� FindRes�� ȣ���� �ѹ��� ã�� �����Ѵ�.
	static Ptr<CTexture> RTTex = CResMgr::GetInst()->FindRes<CTexture>(L"RenderTargetTex");

	// ���ε� ����
	ID3D11ShaderResourceView* SRV = nullptr;
	CONTEXT->PSSetShaderResources(60, 1, &SRV);

	// ����Ÿ�� ���� �ޱ�
	// CopyResource�� �� �ؽ�ó�� �ػ󵵰� ���� �ٸ��� ������ �߻��ϴ� �����ؾ� �Ѵ�.
	// ù ���ڴ� ������, �ι�° ���ڴ� ����̴�.
	CONTEXT->CopyResource(m_RTCopyTex->GetTex2D().Get(), RTTex->GetTex2D().Get());

	// t60 �������� ���ε�
	m_RTCopyTex->UpdateData(60, PIPELINE_STAGE::PS);
}

void CRenderMgr::CopyPositionTarget()
{
	static Ptr<CTexture> PTTex = CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex");

	ID3D11ShaderResourceView* SRV = nullptr;
	CONTEXT->PSSetShaderResources(61, 1, &SRV);

	CONTEXT->CopyResource(m_PTCopyTex->GetTex2D().Get(), PTTex->GetTex2D().Get());

	m_PTCopyTex->UpdateData(61, PIPELINE_STAGE::ALL_STAGE);
}

void CRenderMgr::CopyDataTarget()
{
	static Ptr<CTexture> DataTex = CResMgr::GetInst()->FindRes<CTexture>(L"DataTargetTex");

	ID3D11ShaderResourceView* SRV = nullptr;
	CONTEXT->PSSetShaderResources(62, 1, &SRV);

	CONTEXT->CopyResource(m_DataCopyTex->GetTex2D().Get(), DataTex->GetTex2D().Get());

	m_DataCopyTex->UpdateData(62, PIPELINE_STAGE::ALL_STAGE);
}

CCamera* CRenderMgr::GetMainCam()
{
	if (LEVEL_STATE::PLAY == CLevelMgr::GetInst()->GetLevelState())
	{
		if (m_vecCam.empty())
			return nullptr;

		return m_vecCam[0];
	}
	else
	{
		return m_EditorCam;
	}
}