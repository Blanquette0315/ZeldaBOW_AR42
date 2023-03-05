#pragma once

#include "Ptr.h"
#include "CTexture.h"

class CConstBuffer;

class CDevice
	: public CSingleton<CDevice>
{
private:
	HWND							m_hWnd;

	ComPtr<ID3D11Device>			m_pDevice;				// GPU 메모리 관리
	ComPtr<ID3D11DeviceContext>		m_pDeviceContext;		// GPU Rendering

	Ptr<CTexture>					m_pRenderTargetTex;
	Ptr<CTexture>					m_pDepthStencilTex;		// DepthStencilTexture 안에 View도 포함되어있다.

	ComPtr<IDXGISwapChain>			m_pSwapChain;			// RenderTarget(FrontBuffer, BackBuffer) 를 관리 및 역할 교체 지시
	D3D11_VIEWPORT					m_tViewPort;			// 백버퍼를 윈도우에 그릴 영역(위치, 크기) 지정

	Vec2							m_vRenderResolution;	// 렌더링 버퍼 해상도

	ComPtr<ID3D11SamplerState>		m_arrSampler[(UINT)SAMPLER_TYPE::END];

	CConstBuffer*					m_arrCB[(UINT)CB_TYPE::END];

	ComPtr<ID3D11RasterizerState>	m_arrRS[(UINT)RS_TYPE::END];
	ComPtr<ID3D11DepthStencilState> m_arrDS[(UINT)DS_TYPE::END];
	ComPtr<ID3D11BlendState>		m_arrBS[(UINT)BS_TYPE::END];

public:
	int init(HWND _hWnd, Vec2 _vResolution);
	void Present();

public:
	ID3D11Device* GetDevice() { return m_pDevice.Get(); }
	ID3D11DeviceContext* GetContext() { return m_pDeviceContext.Get(); }
	CConstBuffer* GetConstBuffer(CB_TYPE _eType) { return m_arrCB[(UINT)_eType]; }
	ComPtr<ID3D11RasterizerState> GetRasterizerState(RS_TYPE _eType) { return m_arrRS[(UINT)_eType]; }
	ComPtr<ID3D11DepthStencilState> GetDepthStencilState(DS_TYPE _eType) { return m_arrDS[(UINT)_eType]; }
	ComPtr<ID3D11BlendState> GetBlendState(BS_TYPE _eType) { return m_arrBS[(UINT)_eType]; }

	Vec2 GetRenderResolution() { return m_vRenderResolution; }

private:
	int CreateSwapchain();
	int CreateTarget();
	int CreateConstBuffer();
	int CreateSampler();
	int CreateRasterizerState();
	int CreateDepthStencilState();
	int CreateBlendState();

public:
	CDevice();
	~CDevice();
};

