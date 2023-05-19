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
	m_fAccTime += FDT;

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
		m_fAccTime = fAliveTime * (f - floor(f));

		//tParticleShare share = { iAliveCount, };
		tParticleShare share = { m_iAliveCount, };
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

void CParticleSystem::render(UINT _iSubset)
{
	render();
}

void CParticleSystem::SetMaxCount(UINT _MaxCount)
{
	if (m_iMaxCount != _MaxCount)
	{
		m_iMaxCount = _MaxCount;
		m_ParticleBuffer->Create(sizeof(tParticle), m_iMaxCount, SB_TYPE::UAV_INC, nullptr);
	}
}

void CParticleSystem::SaveToYAML(YAML::Emitter& _emitter)
{
	_emitter << YAML::Key << "PARTICLESYSTEM";
	_emitter << YAML::Value << YAML::BeginMap;
	CRenderComponent::SaveToYAML(_emitter);
	// ��ǻƮ ���̴� ���� ����
	bool bCS = !!m_UpdateCS.Get();
	_emitter << YAML::Key << "CS_Exist";
	_emitter << YAML::Value << bCS;
	if (bCS)
	{
		_emitter << YAML::Key << "ComputeShader";
		_emitter << YAML::Value << WStringToString(m_UpdateCS->GetKey());
	}
	// ��ƼŬ �ɼ�
	_emitter << YAML::Key << "MaxCount";
	_emitter << YAML::Value << m_iMaxCount;
	_emitter << YAML::Key << "AliveCount";
	_emitter << YAML::Value << m_iAliveCount;
	_emitter << YAML::Key << "StartScale";
	_emitter << YAML::Value << m_vStartScale;
	_emitter << YAML::Key << "EndScale";
	_emitter << YAML::Value << m_vEndScale;
	_emitter << YAML::Key << "StartColor";
	_emitter << YAML::Value << m_vStartColor;
	_emitter << YAML::Key << "EndColor";
	_emitter << YAML::Value << m_vEndColor;
	_emitter << YAML::Key << "MinMaxSpeed";
	_emitter << YAML::Value << m_vMinMaxSpeed;
	_emitter << YAML::Key << "MinMaxLifeTime";
	_emitter << YAML::Value << m_vMinMaxLifeTime;
	_emitter << YAML::Key << "SpawnRange";
	_emitter << YAML::Value << m_fSpawnRange;
	_emitter << YAML::Key << "Frequency";
	_emitter << YAML::Value << m_Frequency;
	_emitter << YAML::Key << "AccTime";
	_emitter << YAML::Value << m_fAccTime;
	_emitter << YAML::Key << "WorldSpawn";
	_emitter << YAML::Value << m_WorldSpawn;

	_emitter << YAML::EndMap;
}

void CParticleSystem::LoadFromYAML(YAML::Node& _node)
{
	YAML::Node node = _node["PARTICLESYSTEM"];
	CRenderComponent::LoadFromYAML(node);
	// ��ǻƮ ���̴� ����
	bool bCS = node["CS_Exist"].as<bool>();
	if (bCS)
	{
		wstring strKey = StringToWString(node["ComputeShader"].as<string>());
		m_UpdateCS = dynamic_cast<CParticleUpdateShader*>(CResMgr::GetInst()->FindRes<CComputeShader>(strKey).Get());
	}
	// ��ƼŬ �ɼ�
	m_iMaxCount = node["MaxCount"].as<UINT>();
	m_iAliveCount = node["AliveCount"].as<UINT>();
	m_vStartScale = node["StartScale"].as<Vec4>();
	m_vEndScale = node["EndScale"].as<Vec4>();
	m_vStartColor = node["StartColor"].as<Vec4>();
	m_vEndColor = node["EndColor"].as<Vec4>();
	m_vMinMaxSpeed = node["MinMaxSpeed"].as<Vec2>();
	m_vMinMaxLifeTime = node["MinMaxLifeTime"].as<Vec2>();
	m_fSpawnRange = node["SpawnRange"].as<float>();
	m_Frequency = node["Frequency"].as<float>();
	m_fAccTime = node["AccTime"].as<float>();
	m_WorldSpawn = node["WorldSpawn"].as<int>();
}