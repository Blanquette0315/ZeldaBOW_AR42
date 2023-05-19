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
	// 해당 파티클 시스템 랜더링은 무조건 재질을 파티클을 사용하기 때문에 이곳에서 메시와 재질을 그냥 지정해주는 것이다.
	SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"PointMesh"));

	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_iMaxCount, SB_TYPE::UAV_INC, nullptr);

	m_ParticleShare = new CStructuredBuffer;
	m_ParticleShare->Create(sizeof(tParticleShare), 1, SB_TYPE::UAV_INC, nullptr, true);

	if (m_UpdateCS == nullptr)
	{
		// 기본적으로 파티클 업데이트 셰이더를 넣어준다.
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
	// 파티클 생성 시간
	float fAliveTime = 1.f / m_Frequency;

	// 누적 시간
	m_fAccTime += FDT;

	// 누적시간이 파티클 생성 시간을 넘어서면
	if (fAliveTime < m_fAccTime)
	{
		float f = (m_fAccTime / fAliveTime);
		UINT iAliveCount = (UINT)f;
		// floor()은 내림함수이다. ex) f = 2.8; floor(f); => 2라는 결과를 내준다.
		// ceil()은 올림함수이다.
		// 0.5초당 하나의 파티클이 생성되어야 하고, 지금 누적시간이 1.2초 일때, iAliveCount
		// 이번 프레임에 생성되어야 하는 파티클 수는 2개가 되고, 1.2 / 0.5의 결과 2.4가 floor로 인해 f - floor(f)가 0.4가 된다.
		// 즉, 나머지 누적시간을 다시 셋팅해주어 그 시간에서부터 다시 DT를 누적해나가는 것이다.
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

	// render를 하기 전에 업데이트 해야할 구조화 버퍼를 업데이트 해둔다.
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

	// 갯수가 적으면 파티클을 CPU에서 연산해 GPU메모리로 보내줄 수 있지만, 갯수가 많아질 경우 데이터를 전달하는 비용이 커진다.
	//for (UINT i = 0; i < m_iMaxCount; ++i)
	//{
	//	GetCurMaterial()->SetScalarParam(INT_0, &i);
	//	GetCurMaterial()->UpdateData();
	//	GetMesh()->render();
	//}

	// 인스턴싱을 이용해 한번의 DrowCall로 한번에 처리한다.
	// 인스턴싱을 이용하면 총 3회의 최적화가 이루어 진다.
	// 1. CPU - GPU간 데이터 전송 비용
	// 2. CPU로 연산
	// 3. 1번의 DrawCall로 파이프라인을 실행 (파이프라인을 여러번 동작하지 않고 하나의 파이프라인에서 랜더링을 끝낸다.)
	// 결과 FPS가 1000대였던 것이 4000에 가까히 증가했다.

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
	// 컴퓨트 셰이더 정보 저장
	bool bCS = !!m_UpdateCS.Get();
	_emitter << YAML::Key << "CS_Exist";
	_emitter << YAML::Value << bCS;
	if (bCS)
	{
		_emitter << YAML::Key << "ComputeShader";
		_emitter << YAML::Value << WStringToString(m_UpdateCS->GetKey());
	}
	// 파티클 옵션
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
	// 컴퓨트 셰이더 참조
	bool bCS = node["CS_Exist"].as<bool>();
	if (bCS)
	{
		wstring strKey = StringToWString(node["ComputeShader"].as<string>());
		m_UpdateCS = dynamic_cast<CParticleUpdateShader*>(CResMgr::GetInst()->FindRes<CComputeShader>(strKey).Get());
	}
	// 파티클 옵션
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