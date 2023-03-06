#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CMRT.h"

void CRenderMgr::init()
{
	Vec2 vRenderResol = CDevice::GetInst()->GetRenderResolution();

	// ��ó�� �뵵, ���� Ÿ�� �����
	m_RTCopyTex = CResMgr::GetInst()->CreateTexture(L"RTCopyTex"
		, (UINT)vRenderResol.x, (UINT)vRenderResol.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);

	m_RTCopyTex->UpdateData(60, PIPELINE_STAGE::PS);

	// Deferred Decal Test�� Position Target Tex �����
	m_PTCopyTex = CResMgr::GetInst()->CreateTexture(L"PTCopyTex"
		, (UINT)vRenderResol.x, (UINT)vRenderResol.y
		, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_SHADER_RESOURCE);

	m_PTCopyTex->UpdateData(61, PIPELINE_STAGE::PS);

	// MRT ����
	CreateMRT();

	// Deferred_MergeMtrl�� �Ķ���� ����
	// Deferred_MergeMtrl�� �Ķ���͸� �̰����� �����ϴ� ������ ResMgr�� init �ܰ迡���� ���� DeferredMRT�� ���� Ÿ�� �ؽ�ó��
	// �����Ǳ� ���̱� ������ �Ķ���� ������ �ϰ� �; �� �� ����.
	// �Ķ���� ������ �ϱ� ���� ���ϴٰ� �����Ǵ� ���� �̰��̱� ������ �̰����� ���ذ��̴�.
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Deferred_MergeMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"ColorTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex"));
	pMtrl->SetTexParam(TEX_3, CResMgr::GetInst()->FindRes<CTexture>(L"SpecularTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DirLightMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
	pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DataTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
	pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DataTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"SpotLightMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
	pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DataTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DecalMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"Deferred_DecalMtrl");
}

void CRenderMgr::CreateMRT()
{
	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();

	// =============
	// Swapchain MRT
	// =============
	{
		Ptr<CTexture> arrRTTex[8] = {};

		// Ŭ���� ���� ���ϱ�
		Vec4 arrClear[8] = { Vec4(0.2f, 0.2f, 0.2f, 1.f), };

		Ptr<CTexture> pDSTex = nullptr;

		arrRTTex[0] = CResMgr::GetInst()->FindRes<CTexture>(L"RenderTargetTex");
		pDSTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");

		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Create(arrRTTex, arrClear, pDSTex);
	}

	// ============
	// Deferred MRT
	// ============
	{
		Ptr<CTexture> arrRTTex[8] =
		{
			CResMgr::GetInst()->CreateTexture(L"ColorTargetTex"
											, vRenderResolution.x, vRenderResolution.y
											, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CResMgr::GetInst()->CreateTexture(L"NormalTargetTex"
											, vRenderResolution.x, vRenderResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CResMgr::GetInst()->CreateTexture(L"PositionTargetTex"
											, vRenderResolution.x, vRenderResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CResMgr::GetInst()->CreateTexture(L"DataTargetTex"
											, vRenderResolution.x, vRenderResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		// Ŭ���� ���� ���ϱ�
		Vec4 arrClear[8] = {
			Vec4(0.f,0.f,0.f,0.f),
			Vec4(0.f,0.f,0.f,0.f),
			Vec4(0.f,0.f,0.f,0.f),
			Vec4(0.f,0.f,0.f,0.f)
		};

		Ptr<CTexture> pDSTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");


		m_arrMRT[(UINT)MRT_TYPE::DEFERRED] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->Create(arrRTTex, arrClear, pDSTex);
	}

	// ============
	//   Light MRT
	//  �ش� MRT�� ���� ���� ó���ؼ� ���� �������� ���� Ÿ���̴�.
	// RenderTargetTexture�� ��� ���ݻ籤�� �ݻ籤 �̷��� 2���� ���� ���̴�.
	// �̹̽ú��� ��� ��ü�� ��ü�̱� ������ �̰��� �ƴ� DeferredMRT���� ���� ���̴�.
	// ============
	{
		Ptr<CTexture> arrRTTex[8] =
		{
			CResMgr::GetInst()->CreateTexture(L"DiffuseTargetTex"
											, vRenderResolution.x, vRenderResolution.y
											, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CResMgr::GetInst()->CreateTexture(L"SpecularTargetTex"
											, vRenderResolution.x, vRenderResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		// Ŭ���� ���� ���ϱ�
		Vec4 arrClear[8] = {
			Vec4(0.f,0.f,0.f,0.f),
			Vec4(0.f,0.f,0.f,0.f)
		};

		// ���� �ؽ�ó�� �ʿ������ ���־���.
		Ptr<CTexture> pDSTex = nullptr;

		m_arrMRT[(UINT)MRT_TYPE::LIGHT] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Create(arrRTTex, arrClear, pDSTex);
	}
}

void CRenderMgr::ClearMRT()
{
	for (int i = 0; i < (UINT)MRT_TYPE::END; ++i)
	{
		if (nullptr != m_arrMRT[i])
		{
			m_arrMRT[i]->Clear();
		}
	}
}