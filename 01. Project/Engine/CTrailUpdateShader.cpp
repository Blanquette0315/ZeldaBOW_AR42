#include "pch.h"
#include "CTrailUpdateShader.h"

CTrailUpdateShader::CTrailUpdateShader()
	: CComputeShader(128, 1, 1)
{
}

CTrailUpdateShader::~CTrailUpdateShader()
{
}

void CTrailUpdateShader::UpdateData()
{
	m_VecPosBuffer->UpdateData_CS(0, false);
	m_OutputBuffer->UpdateData_CS(1, false);

	m_iGroupX = m_Param.iArr[0] / m_iGroupPerThreadX + 1;
	m_iGroupY = 1;
	m_iGroupZ = 1;
}

void CTrailUpdateShader::Clear()
{
	m_VecPosBuffer->Clear();
	m_VecPosBuffer = nullptr;

	m_OutputBuffer->Clear();
	m_OutputBuffer = nullptr;
}


