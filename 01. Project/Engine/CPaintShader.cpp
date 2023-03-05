#include "pch.h"
#include "CPaintShader.h"


CPaintShader::CPaintShader()
	: CComputeShader(32, 32, 1)
{
}

CPaintShader::~CPaintShader()
{
}

void CPaintShader::UpdateData()
{
	// Target Texture를 u0에 바인딩건다.
	m_pTargetTex->UpdateData_CS(0);

	// 그룹 수 계산
	// 텍스처 크기가 몇짜리를 사용하던지 범용적으로 사용할 수 있도록 스레드를 보다 넉넉히 주었다.
	m_iGroupX = m_pTargetTex->GetWidth() / m_iGroupPerThreadX + 1;
	m_iGroupY = m_pTargetTex->GetHeight() / m_iGroupPerThreadY + 1;
	m_iGroupZ = 1;
}

void CPaintShader::Clear()
{
	CTexture::Clear_CS(0);
}