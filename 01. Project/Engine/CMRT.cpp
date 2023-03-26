#include "pch.h"
#include "CMRT.h"

#include "CDevice.h"

CMRT::CMRT()
	: m_arrTarget{}
	, m_RTCount(0)
{
}

CMRT::~CMRT()
{
}

void CMRT::Create(Ptr<CTexture>(&_arrTex)[8], Vec4(&arrClear)[8], Ptr<CTexture> _pDesTex)
{
	for (int i = 0; i < 8; ++i)
	{
		// 여기서 만약 nullptr을 마주치면 더 이상 반복문을 돌지 않기 때문에
		// _arrTex에 랜더 타겟을 앞에서 부터 채워주어야 한다.
		if (nullptr == _arrTex[i].Get())
		{
			m_RTCount = i;
			break;
		}

		m_arrTarget[i] = _arrTex[i];
	}

	for (int i = 0; i < m_RTCount; ++i)
	{
		m_arrClearColor[i] = arrClear[i];
	}

	m_DSTex = _pDesTex;

	// ViewPort 설정	
	m_tViewPort.TopLeftX = 0;
	m_tViewPort.TopLeftY = 0;

	m_tViewPort.Width = m_arrTarget[0]->GetWidth();
	m_tViewPort.Height = m_arrTarget[0]->GetHeight();

	m_tViewPort.MinDepth = 0;
	m_tViewPort.MaxDepth = 1;
}

void CMRT::OMSet()
{
	CMaterial::Clear(); // 얘 위치를 옮겨줄 필요가 있다.
	// OMSetRenderTargets() 함수가 인자로 받는 것이 ID3D11RenderTargetView이기 때문에
	// 우리가 만든 Ptr로 만들어진 m_arrTarget을 옮겨주어 전달해야 한다.
	ID3D11RenderTargetView* arrRTV[8] = {};
	for (int i = 0; i < m_RTCount; ++i)
	{
		arrRTV[i] = m_arrTarget[i]->GetRTV().Get();
	}

	if (nullptr != m_DSTex.Get())
		CONTEXT->OMSetRenderTargets(m_RTCount, arrRTV, m_DSTex->GetDSV().Get());
	else
		CONTEXT->OMSetRenderTargets(m_RTCount, arrRTV, nullptr);
}

void CMRT::Clear()
{
	for (int i = 0; i < m_RTCount; ++i)
	{
		CONTEXT->ClearRenderTargetView(m_arrTarget[i]->GetRTV().Get(), m_arrClearColor[i]);
	}

	if (m_DSTex.Get())
		CONTEXT->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
