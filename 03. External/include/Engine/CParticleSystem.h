#pragma once
#include "CRenderComponent.h"

#include "CParticleUpdateShader.h"
// 파티클의 입자 갯수는 그때그때 바뀌므로 파티클 입자 하나를 설명하는 파티클 구조체를 상수 버퍼로 바인딩 걸 수 없다.
// 따라서 구조화 버퍼를 이용해 바인딩을 걸 것이다.

// 파티클 구조체는 오로지 파티클 셰이더에서만 사용하므로 register.fx에 선언하지 않고, 파티클 셰이더 쪽에 선언할 것이다.



// 파티클은 평면으로 만드는데, 3D 공간상에서 카메라가 회전해 바라보면 평면이 아닌 평면의 측면이 보이게 된다. 따라서 이것을 바꾸어
// 카메라가 어느 방향에서 해당 파티클을 바라보던지 항상 카메라를 바라보는 방향으로 파티클이 그려져야 한다.
// 따라서 Mesh를 점 Mesh로 사용한뒤, GeometryShader를 이용해 정점을 분할 시켜 카메라를 바라보는 방향으로 사각형 Mesh를 셰이더에서 만들어줄 것이다.



class CStructuredBuffer;

class CParticleSystem :
    public CRenderComponent
{
private:
    UINT                        m_iMaxCount;    // 파티클의 최대 수 만큼 구조화 버퍼를 만들어야 하기 때문에 멤버로 두었다.
    UINT                        m_iAliveCount;

    Vec4                        m_vStartScale;  // 처음 시작시 파티클의 크기
    Vec4                        m_vEndScale;    // 마지막 소멸 직전의 파티클 크기

    Vec4                        m_vStartColor;  // 처음 시작시 파티클의 색상
    Vec4                        m_vEndColor;    // 마지막 소멸 직전의 파티클 색상

    Vec2                        m_vMinMaxSpeed;
    Vec2                        m_vMinMaxLifeTime; // 파티클 최소 생존 시간, 파티클 최대 생존 시간 (최대 최소 시간사이 랜덤하게 설정되도록 만들 것이다.)

    float                       m_fSpawnRange;  // 생성범위

    float                       m_Frequency;    // 초당 파티클 생성 수
    float                       m_fAccTime;     // 누적 시간

    int                         m_WorldSpawn;   // 파티클을 상대좌표로 생성할지, 월드좌표로 생성할지

    CStructuredBuffer*          m_ParticleBuffer;   // 파티클 구조화 버퍼
    CStructuredBuffer*          m_ParticleShare;    // 파티클 공용 구조화 버퍼 
    Ptr<CParticleUpdateShader>  m_UpdateCS;

public:
    virtual void finaltick() override;
    virtual void render() override;

public:
    void SetFrequency(float _Frequency) { m_Frequency = _Frequency; }
    void SetSpawnRange(float _SpawnRange) { m_fSpawnRange = _SpawnRange; }
    void SetCS(CParticleUpdateShader* _CS) { m_UpdateCS = _CS; }

    void SetMaxCount(UINT _MaxCount);

    void Set_SE_Scale(Vec4 _StartScale, Vec4 _EndScale) { m_vStartScale = _StartScale; m_vEndScale = _EndScale; }
    void Set_SE_Color(Vec4 _StartColor, Vec4 _EndColor) { m_vStartColor = _StartColor; m_vEndColor = _EndColor; }
    void SetMinMaxLifeTime(Vec2 _MinMaxTime) { m_vMinMaxLifeTime = _MinMaxTime; }
    void SetMinMaxSpeed(Vec2 _MinMaxSpeed) { m_vMinMaxSpeed = _MinMaxSpeed; }
    void SetWorldSpawn(bool _WorldSpwan) { m_WorldSpawn = (int)_WorldSpwan; }

public:
    UINT GetMaxCount() { return m_iMaxCount; }
    UINT GetAliveCount() { return m_iAliveCount; }
    Vec4 GetStartColor() { return m_vStartColor; }
    Vec4 GetEndColor() { return m_vEndColor; }
    Vec4 GetStartScale() { return m_vStartScale; }
    Vec4 GetEndScale() { return m_vEndScale; }
    Vec2 GetMinMaxSpeed() { return m_vMinMaxSpeed; }
    Vec2 GetMinMaxLifeTime() { return m_vMinMaxLifeTime; }
    float GetSpawnRange() { return m_fSpawnRange; }
    float GetFrequency() { return m_Frequency; }
    bool GetWorldSpawn() { return (bool)m_WorldSpawn; }
    Ptr<CParticleUpdateShader> GetCS() { return m_UpdateCS; }

public:
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
    CLONE(CParticleSystem);

public:
    CParticleSystem();
    CParticleSystem(const CParticleSystem& _origin);
    ~CParticleSystem();
};

