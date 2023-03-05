#include "pch.h"
#include "CParticleUpdateShader.h"

#include "CDevice.h"
#include "CStructuredBuffer.h"

CParticleUpdateShader::CParticleUpdateShader()
	: CComputeShader(128,1,1)
	, m_ParticleBuffer(nullptr)
	, m_ParticleShare(nullptr)
{
}

CParticleUpdateShader::~CParticleUpdateShader()
{
}

void CParticleUpdateShader::SetParticleBuffer(CStructuredBuffer* _Buffer)
{
	assert(_Buffer);

	// 해당 구조화 버퍼의 Element 수가 우리가 사용할 최대 파티클 갯수와 동일하기 때문에 받아온다.
	m_ParticleBuffer = _Buffer;
	m_Param.iArr[0] = m_ParticleBuffer->GetElementCount();
}

void CParticleUpdateShader::UpdateData()
{
	assert(m_ParticleBuffer);
	assert(m_ParticleShare);

	m_ParticleBuffer->UpdateData_CS(0, false);
	m_ParticleShare->UpdateData_CS(1, false);

	// 스레드 그룹 갯수를 정해준다. (1차원만 사용중이기 떄문에 x값만 증가하며, 만약을 대비해 1을 추가로 더 할당해준다.)
	m_iGroupX = m_Param.iArr[0] / m_iGroupPerThreadX + 1;
	m_iGroupY = 1;
	m_iGroupZ = 1;
}

void CParticleUpdateShader::Clear()
{
	m_ParticleBuffer->Clear();
	m_ParticleShare->Clear();
}
