#include "pch.h"
#include "CRaymapShader.h"

#include "CConstBuffer.h"
#include "CStructuredBuffer.h"
#include "CTexture.h"

CRaymapShader::CRaymapShader()
	: CComputeShader(32, 32, 1)
	, m_iBrushIdx(0)
	, m_pInput(nullptr)
	, m_bBrushShow(true)
{
}

CRaymapShader::~CRaymapShader()
{
}

void CRaymapShader::UpdateData()
{
	m_Param.iArr[0] = (int)m_pRayMap->GetWidth();
	m_Param.iArr[1] = (int)m_pRayMap->GetHeight();
	m_Param.iArr[2] = m_iBrushIdx;
	m_Param.iArr[3] = (int)m_bBrushShow;
	m_Param.v2Arr[0] = m_vScale;

	// ���̸�
	m_pRayMap->UpdateData_CS(0, false);

	// �귯��
	m_pBrushTex->UpdateData_CS(0, true);

	// ��ŷ����
	m_pInput->UpdateData_CS(16, true);

	// ���� ��ų ������ �׷� �� ����
	m_iGroupX = ((UINT)m_pRayMap->GetWidth() / m_iGroupPerThreadX) + 1;
	m_iGroupY = ((UINT)m_pRayMap->GetHeight() / m_iGroupPerThreadY) + 1;
	m_iGroupZ = 1;
}

void CRaymapShader::Clear()
{
	if (nullptr != m_pRayMap)
	{
		m_pRayMap->Clear_CS(0);
		m_pRayMap = nullptr;
	}

	if (nullptr != m_pBrushTex)
	{
		m_pBrushTex->Clear(0);
		m_pBrushTex = nullptr;
	}

	if (nullptr != m_pInput)
	{
		m_pInput->Clear();
		m_pInput = nullptr;
	}
}