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
		// ���⼭ ���� nullptr�� ����ġ�� �� �̻� �ݺ����� ���� �ʱ� ������
		// _arrTex�� ���� Ÿ���� �տ��� ���� ä���־�� �Ѵ�.
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

	// ViewPort ����	
	m_tViewPort.TopLeftX = 0;
	m_tViewPort.TopLeftY = 0;

	m_tViewPort.Width = m_arrTarget[0]->GetWidth();
	m_tViewPort.Height = m_arrTarget[0]->GetHeight();

	m_tViewPort.MinDepth = 0;
	m_tViewPort.MaxDepth = 1;
}

void CMRT::OMSet()
{
	CMaterial::Clear(); // �� ��ġ�� �Ű��� �ʿ䰡 �ִ�.
	// OMSetRenderTargets() �Լ��� ���ڷ� �޴� ���� ID3D11RenderTargetView�̱� ������
	// �츮�� ���� Ptr�� ������� m_arrTarget�� �Ű��־� �����ؾ� �Ѵ�.
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
