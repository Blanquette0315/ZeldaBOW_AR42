#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CMRT.h"

void CRenderMgr::init()
{
	Vec2 vRenderResol = CDevice::GetInst()->GetRenderResolution();

	// 후처리 용도, 렌더 타겟 복사용
	m_RTCopyTex = CResMgr::GetInst()->CreateTexture(L"RTCopyTex"
		, (UINT)vRenderResol.x, (UINT)vRenderResol.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);

	m_RTCopyTex->UpdateData(60, PIPELINE_STAGE::PS);

	// Deferred Decal Test용 Position Target Tex 복사용
	m_PTCopyTex = CResMgr::GetInst()->CreateTexture(L"PTCopyTex"
		, (UINT)vRenderResol.x, (UINT)vRenderResol.y
		, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_SHADER_RESOURCE);

	m_PTCopyTex->UpdateData(61, PIPELINE_STAGE::PS);

	// MRT 생성
	CreateMRT();

	// Deferred_MergeMtrl에 파라미터 셋팅
	// Deferred_MergeMtrl의 파라미터를 이곳에서 셋팅하는 이유는 ResMgr의 init 단계에서는 아직 DeferredMRT의 랜더 타겟 텍스처가
	// 생성되기 전이기 때문에 파라미터 설정을 하고 싶어도 할 수 없다.
	// 파라미터 셋팅을 하기 제일 편하다고 생각되는 곳이 이곳이기 때문에 이곳에서 해준것이다.
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
}

void CRenderMgr::CreateMRT()
{
	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();

	// =============
	// Swapchain MRT
	// =============
	{
		Ptr<CTexture> arrRTTex[8] = {};

		// 클리어 색상 정하기
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
											, (UINT)vRenderResolution.x, (UINT)vRenderResolution.y
											, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CResMgr::GetInst()->CreateTexture(L"NormalTargetTex"
											, (UINT)vRenderResolution.x, (UINT)vRenderResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CResMgr::GetInst()->CreateTexture(L"PositionTargetTex"
											, (UINT)vRenderResolution.x, (UINT)vRenderResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CResMgr::GetInst()->CreateTexture(L"DataTargetTex"
											, (UINT)vRenderResolution.x, (UINT)vRenderResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		// 클리어 색상 정하기
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
	//  해당 MRT는 빛만 따로 처리해서 빛만 랜더링할 랜더 타겟이다.
	// RenderTargetTexture의 경우 난반사광과 반사광 이렇게 2개만 만들 것이다.
	// 이미시브의 경우 주체가 물체이기 때문에 이곳이 아닌 DeferredMRT에서 해줄 것이다.
	// ============
	{
		Ptr<CTexture> arrRTTex[8] =
		{
			CResMgr::GetInst()->CreateTexture(L"DiffuseTargetTex"
											, (UINT)vRenderResolution.x, (UINT)vRenderResolution.y
											, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			CResMgr::GetInst()->CreateTexture(L"SpecularTargetTex"
											, (UINT)vRenderResolution.x, (UINT)vRenderResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		// 클리어 색상 정하기
		Vec4 arrClear[8] = {
			Vec4(0.f,0.f,0.f,0.f),
			Vec4(0.f,0.f,0.f,0.f)
		};

		// 깊이 텍스처가 필요없으니 빼주었다.
		Ptr<CTexture> pDSTex = nullptr;

		m_arrMRT[(UINT)MRT_TYPE::LIGHT] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Create(arrRTTex, arrClear, pDSTex);
	}

	// =============
	// ShadowMap MRT
	// =============
	{
		Ptr<CTexture> arrRTTex[8] =
		{
			CResMgr::GetInst()->CreateTexture(L"DepthMapTex"
											, 4096, 4096
											, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		Vec4 arrClear[8] = {
				 Vec4(0.f, 0.f, 0.f, 0.f)
		};

		// Depth Stencil Texture 만들기
		Ptr<CTexture> pDepthStencilTex
			= CResMgr::GetInst()->CreateTexture(L"DepthMapDSTex"
				, 4096, 4096
				, DXGI_FORMAT_D32_FLOAT, D3D11_BIND_DEPTH_STENCIL);


		m_arrMRT[(UINT)MRT_TYPE::SHADOW] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::SHADOW]->Create(arrRTTex, arrClear, pDepthStencilTex);
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