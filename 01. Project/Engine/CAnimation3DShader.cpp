#include "pch.h"
#include "CAnimation3DShader.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

CAnimation3DShader::CAnimation3DShader()
	: CComputeShader(256, 1, 1)
	, m_pFrameDataBuffer(nullptr)
	, m_pOffsetMatBuffer(nullptr)
	, m_pOutputBuffer(nullptr)
{
}

CAnimation3DShader::~CAnimation3DShader()
{
}

void CAnimation3DShader::UpdateData()
{
	// 구조화버퍼 전달
	m_pFrameDataBuffer->UpdateData_CS(16, true); // t16
	m_pOffsetMatBuffer->UpdateData_CS(17, true); // t17
	m_pOutputBuffer->UpdateData_CS(0, false);   // u0

	m_iGroupX = (m_Param.iArr[0] / m_iGroupPerThreadX) + 1;
	m_iGroupY = 1;
	m_iGroupZ = 1;
}


void CAnimation3DShader::Clear()
{
	// 전달한 구조화버퍼 클리어	
	if (nullptr != m_pFrameDataBuffer)
	{
		m_pFrameDataBuffer->Clear();
		m_pFrameDataBuffer = nullptr;
	}

	if (nullptr != m_pOffsetMatBuffer)
	{
		m_pOffsetMatBuffer->Clear();
		m_pOffsetMatBuffer = nullptr;
	}

	if (nullptr != m_pOutputBuffer)
	{
		m_pOutputBuffer->Clear();
		m_pOutputBuffer = nullptr;
	}
}

void CAnimation3DShader::Execute()
{
	UpdateData();

	// 상수 버퍼 업데이트
	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Param);
	pCB->UpdateData_CS();

	CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);
	CONTEXT->Dispatch(m_iGroupX, m_iGroupY, m_iGroupZ);

	Clear();

	// clear shared Param
	m_Param.v2Arr[0] = Vec2(0.f, 0.f);
	m_Param.v2Arr[1] = Vec2(0.f, 0.f);

	m_Param.iArr[1] = 0;
	m_Param.iArr[2] = 0;
}
