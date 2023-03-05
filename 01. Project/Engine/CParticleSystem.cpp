#include "pch.h"
#include "CParticleSystem.h"

#include "CResMgr.h"
#include "CTimeMgr.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

#include "CTransform.h"

CParticleSystem::CParticleSystem()
	: CRenderComponent(COMPONENT_TYPE::PARTICLESYSTEM)
	, m_iMaxCount(1)
	, m_iAliveCount(0)
	, m_vStartScale{}
	, m_vEndScale{}
	, m_vStartColor{}
	, m_vEndColor{}
	, m_vMinMaxSpeed{}
	, m_vMinMaxLifeTime{}
	, m_Frequency(0.f)
	, m_fSpawnRange(0.f)
	, m_fAccTime(0.f)
	, m_ParticleBuffer(nullptr)
	, m_ParticleShare(nullptr)
	, m_WorldSpawn(0)
{
	// �ش� ��ƼŬ �ý��� �������� ������ ������ ��ƼŬ�� ����ϱ� ������ �̰����� �޽ÿ� ������ �׳� �������ִ� ���̴�.
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));

	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_iMaxCount, SB_TYPE::UAV_INC, nullptr);

	m_ParticleShare = new CStructuredBuffer;
	m_ParticleShare->Create(sizeof(tParticleShare), 1, SB_TYPE::UAV_INC, nullptr, true);

	if (m_UpdateCS == nullptr)
	{
		// �⺻������ ��ƼŬ ������Ʈ ���̴��� �־��ش�.
		SetCS((CParticleUpdateShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateShader").Get());
	}
}

CParticleSystem::CParticleSystem(const CParticleSystem& _origin)
	: CRenderComponent(_origin)
	, m_iMaxCount(_origin.m_iMaxCount)
	, m_iAliveCount(0)
	, m_vStartScale(_origin.m_vStartScale)
	, m_vEndScale(_origin.m_vEndScale)
	, m_vStartColor(_origin.m_vStartColor)
	, m_vEndColor(_origin.m_vEndColor)
	, m_vMinMaxSpeed(_origin.m_vMinMaxSpeed)
	, m_vMinMaxLifeTime(_origin.m_vMinMaxLifeTime)
	, m_Frequency(_origin.m_Frequency)
	, m_fSpawnRange(_origin.m_fSpawnRange)
	, m_fAccTime(0.f)
	, m_ParticleBuffer(nullptr)
	, m_ParticleShare(nullptr)
	, m_WorldSpawn(_origin.m_WorldSpawn)
	, m_UpdateCS (_origin.m_UpdateCS)
{
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));

	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_iMaxCount, SB_TYPE::UAV_INC, nullptr);

	m_ParticleShare = new CStructuredBuffer;
	m_ParticleShare->Create(sizeof(tParticleShare), 1, SB_TYPE::UAV_INC, nullptr, true);
}

CParticleSystem::~CParticleSystem()
{
	if (nullptr != m_ParticleBuffer)
		delete m_ParticleBuffer;

	if (nullptr != m_ParticleShare)
		delete m_ParticleShare;
}

void CParticleSystem::finaltick()
{
	// ��ƼŬ ���� �ð�
	float fAliveTime = 1.f / m_Frequency;

	// ���� �ð�
	m_fAccTime += DT;

	// �����ð��� ��ƼŬ ���� �ð��� �Ѿ��
	if (fAliveTime < m_fAccTime)
	{
		float f = (m_fAccTime / fAliveTime);
		UINT iAliveCount = (UINT)f;
		// floor()�� �����Լ��̴�. ex) f = 2.8; floor(f); => 2��� ����� ���ش�.
		// ceil()�� �ø��Լ��̴�.
		// 0.5�ʴ� �ϳ��� ��ƼŬ�� �����Ǿ�� �ϰ�, ���� �����ð��� 1.2�� �϶�, iAliveCount
		// �̹� �����ӿ� �����Ǿ�� �ϴ� ��ƼŬ ���� 2���� �ǰ�, 1.2 / 0.5�� ��� 2.4�� floor�� ���� f - floor(f)�� 0.4�� �ȴ�.
		// ��, ������ �����ð��� �ٽ� �������־� �� �ð��������� �ٽ� DT�� �����س����� ���̴�.
		m_fAccTime = f - floor(f);

		//tParticleShare share = { iAliveCount, };
		tParticleShare share = { 10, };
		m_ParticleShare->SetData(&share, 1);
	}

	else
	{
		tParticleShare share = {};
		m_ParticleShare->SetData(&share, 1);
	}

	// render�� �ϱ� ���� ������Ʈ �ؾ��� ����ȭ ���۸� ������Ʈ �صд�.
	m_UpdateCS->SetParticleBuffer(m_ParticleBuffer);
	m_UpdateCS->SetParticleShareData(m_ParticleShare);
	m_UpdateCS->SetWorldSpawn(m_WorldSpawn);
	m_UpdateCS->SetObjectWorldPos(Transform()->GetWorldPos());
	m_UpdateCS->SetSpawnRange(m_fSpawnRange);
	m_UpdateCS->SetMinMaxSpeed(m_vMinMaxSpeed);
	m_UpdateCS->SetMinMaxLifeTime(m_vMinMaxLifeTime);
	m_UpdateCS->Execute();
}

void CParticleSystem::render()
{
	Transform()->UpdateData();
	m_ParticleBuffer->UpdateData(16, PIPELINE_STAGE::VS | PIPELINE_STAGE::GS | PIPELINE_STAGE::PS);

	// ������ ������ ��ƼŬ�� CPU���� ������ GPU�޸𸮷� ������ �� ������, ������ ������ ��� �����͸� �����ϴ� ����� Ŀ����.
	//for (UINT i = 0; i < m_iMaxCount; ++i)
	//{
	//	GetCurMaterial()->SetScalarParam(INT_0, &i);
	//	GetCurMaterial()->UpdateData();
	//	GetMesh()->render();
	//}

	// �ν��Ͻ��� �̿��� �ѹ��� DrowCall�� �ѹ��� ó���Ѵ�.
	// �ν��Ͻ��� �̿��ϸ� �� 3ȸ�� ����ȭ�� �̷�� ����.
	// 1. CPU - GPU�� ������ ���� ���
	// 2. CPU�� ����
	// 3. 1���� DrawCall�� ������������ ���� (������������ ������ �������� �ʰ� �ϳ��� ���������ο��� �������� ������.)
	// ��� FPS�� 1000�뿴�� ���� 4000�� ������ �����ߴ�.

	GetCurMaterial()->SetScalarParam(INT_1, &m_WorldSpawn);
	GetCurMaterial()->SetScalarParam(VEC4_0, &m_vStartScale);
	GetCurMaterial()->SetScalarParam(VEC4_1, &m_vEndScale);
	GetCurMaterial()->SetScalarParam(VEC4_2, &m_vStartColor);
	GetCurMaterial()->SetScalarParam(VEC4_3, &m_vEndColor);

	GetCurMaterial()->UpdateData();
	GetMesh()->render_particle(m_iMaxCount);

	m_ParticleBuffer->Clear();
}

void CParticleSystem::SetMaxCount(UINT _MaxCount)
{
	if (m_iMaxCount != _MaxCount)
	{
		m_iMaxCount = _MaxCount;
		m_ParticleBuffer->Create(sizeof(tParticle), m_iMaxCount, SB_TYPE::UAV_INC, nullptr);
	}
}








void CParticleSystem::SaveToFile(FILE* _File)
{
	CRenderComponent::SaveToFile(_File);

	// ��ǻƮ �Ο޴� ���� ����
	bool bCS = !!m_UpdateCS.Get();
	fwrite(&bCS, sizeof(bool), 1, _File);

	if (bCS)
	{
		SaveWStringToFile(m_UpdateCS->GetKey(), _File);
	}

	// ��ƼŬ �ɼ�
	fwrite(&m_iMaxCount, sizeof(UINT), 1, _File);
	fwrite(&m_iAliveCount, sizeof(UINT), 1, _File);
	fwrite(&m_vStartScale, sizeof(Vec4), 1, _File);
	fwrite(&m_vEndScale, sizeof(Vec4), 1, _File);
	fwrite(&m_vStartColor, sizeof(Vec4), 1, _File);
	fwrite(&m_vEndColor, sizeof(Vec4), 1, _File);
	fwrite(&m_vMinMaxSpeed, sizeof(Vec2), 1, _File);
	fwrite(&m_vMinMaxLifeTime, sizeof(Vec2), 1, _File);
	fwrite(&m_fSpawnRange, sizeof(float), 1, _File);
	fwrite(&m_Frequency, sizeof(float), 1, _File);
	fwrite(&m_fAccTime, sizeof(float), 1, _File);
	fwrite(&m_WorldSpawn, sizeof(int), 1, _File);
}

void CParticleSystem::LoadFromFile(FILE* _File)
{
	CRenderComponent::LoadFromFile(_File);

	// ��ǻƮ ���̴� ����
	bool bCS = false;
	fread(&bCS, sizeof(bool), 1, _File);

	if (bCS)
	{
		wstring strKey;
		LoadWStringFromFile(strKey, _File);
		m_UpdateCS = dynamic_cast<CParticleUpdateShader*>(CResMgr::GetInst()->FindRes<CComputeShader>(strKey).Get());
	}

	// ��ƼŬ �ɼ�
	fread(&m_iMaxCount, sizeof(UINT), 1, _File);
	fread(&m_iAliveCount, sizeof(UINT), 1, _File);
	fread(&m_vStartScale, sizeof(Vec4), 1, _File);
	fread(&m_vEndScale, sizeof(Vec4), 1, _File);
	fread(&m_vStartColor, sizeof(Vec4), 1, _File);
	fread(&m_vEndColor, sizeof(Vec4), 1, _File);
	fread(&m_vMinMaxSpeed, sizeof(Vec2), 1, _File);
	fread(&m_vMinMaxLifeTime, sizeof(Vec2), 1, _File);
	fread(&m_fSpawnRange, sizeof(float), 1, _File);
	fread(&m_Frequency, sizeof(float), 1, _File);
	fread(&m_fAccTime, sizeof(float), 1, _File);
	fread(&m_WorldSpawn, sizeof(int), 1, _File);
}