#pragma once
#include "CRenderComponent.h"

#include "CParticleUpdateShader.h"
// ��ƼŬ�� ���� ������ �׶��׶� �ٲ�Ƿ� ��ƼŬ ���� �ϳ��� �����ϴ� ��ƼŬ ����ü�� ��� ���۷� ���ε� �� �� ����.
// ���� ����ȭ ���۸� �̿��� ���ε��� �� ���̴�.

// ��ƼŬ ����ü�� ������ ��ƼŬ ���̴������� ����ϹǷ� register.fx�� �������� �ʰ�, ��ƼŬ ���̴� �ʿ� ������ ���̴�.



// ��ƼŬ�� ������� ����µ�, 3D �����󿡼� ī�޶� ȸ���� �ٶ󺸸� ����� �ƴ� ����� ������ ���̰� �ȴ�. ���� �̰��� �ٲپ�
// ī�޶� ��� ���⿡�� �ش� ��ƼŬ�� �ٶ󺸴��� �׻� ī�޶� �ٶ󺸴� �������� ��ƼŬ�� �׷����� �Ѵ�.
// ���� Mesh�� �� Mesh�� ����ѵ�, GeometryShader�� �̿��� ������ ���� ���� ī�޶� �ٶ󺸴� �������� �簢�� Mesh�� ���̴����� ������� ���̴�.



class CStructuredBuffer;

class CParticleSystem :
    public CRenderComponent
{
private:
    UINT                        m_iMaxCount;    // ��ƼŬ�� �ִ� �� ��ŭ ����ȭ ���۸� ������ �ϱ� ������ ����� �ξ���.
    UINT                        m_iAliveCount;

    Vec4                        m_vStartScale;  // ó�� ���۽� ��ƼŬ�� ũ��
    Vec4                        m_vEndScale;    // ������ �Ҹ� ������ ��ƼŬ ũ��

    Vec4                        m_vStartColor;  // ó�� ���۽� ��ƼŬ�� ����
    Vec4                        m_vEndColor;    // ������ �Ҹ� ������ ��ƼŬ ����

    Vec2                        m_vMinMaxSpeed;
    Vec2                        m_vMinMaxLifeTime; // ��ƼŬ �ּ� ���� �ð�, ��ƼŬ �ִ� ���� �ð� (�ִ� �ּ� �ð����� �����ϰ� �����ǵ��� ���� ���̴�.)

    float                       m_fSpawnRange;  // ��������

    float                       m_Frequency;    // �ʴ� ��ƼŬ ���� ��
    float                       m_fAccTime;     // ���� �ð�

    int                         m_WorldSpawn;   // ��ƼŬ�� �����ǥ�� ��������, ������ǥ�� ��������

    CStructuredBuffer*          m_ParticleBuffer;   // ��ƼŬ ����ȭ ����
    CStructuredBuffer*          m_ParticleShare;    // ��ƼŬ ���� ����ȭ ���� 
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

