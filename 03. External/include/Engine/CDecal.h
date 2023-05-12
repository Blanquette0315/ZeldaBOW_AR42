#pragma once
#include "CRenderComponent.h"

// ���� ��Į�� ���η� ��ģ ������ ǥ�����ִ� ��Į�� ������ �־��� ������
// ���η� ����ؾ��� ���� ��Į ������Ʈ�� ȸ�������־�� ����� ���´�.
// ������ ��Į�� UV�� x,y ������ ����߱� �����̴�.
// ���� ȸ���� ���Ѵٸ� ������ ��� z,x�� ������־�� �Ѵ�.
// Unreal Engine�� ��쵵 ��Į ������Ʈ�� ȸ���� ����ϴ� ����̴�.

class CDecal :
    public CRenderComponent
{
private:
    Ptr<CTexture>   m_pDecalTex;
    bool            m_bLighting;    // ��Ȯ���� Forward�� ����������, Deferred�� ����������

public:
    void SetRenderType(bool _bSet); // m_bLgihting�� ��, forward������� Ȥ�� Deferred ������� ������������ ���� �˸��� ������ �������ش�.
    bool IsDeferred() { return m_bLighting; }
    void SetDecalTexture(Ptr<CTexture> _Tex) { m_pDecalTex = _Tex; }
    Ptr<CTexture> GetDecalTexture() { return m_pDecalTex; }

public:
    virtual void finaltick() override;
    virtual void render() override;
    virtual void render(UINT _iSubset) override;

public:
    virtual void SaveToYAML(YAML::Emitter& _emitter) override;
    virtual void LoadFromYAML(YAML::Node& _node) override;
    CLONE(CDecal);
public:
    CDecal();
    ~CDecal();
};