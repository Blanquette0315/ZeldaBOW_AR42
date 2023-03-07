#pragma once
#include "CEntity.h"

#include "Ptr.h"

class CRes :
    public CEntity
{
private:
    wstring         m_strKey;
    wstring         m_strRelativePath;
    const RES_TYPE  m_eResType;
    int             m_iRefCount;

    bool            m_bEngineRes;   // �������� �������ִ� ���ҽ����� Editor���� ������� ���ҽ����� Ȯ��
    bool            m_bChangeName;  // ���ҽ��� �̸��� ����Ǹ� üũ���ش�. (�켱 ������ Mtrl�̸��� ����Ǹ� CheckRelativePath()�� ��������ֱ� �����̴�.)

private:
    void AddRef()
    {
        ++m_iRefCount;
    }

    void Release()
    {
        --m_iRefCount;
        assert(!(m_iRefCount < 0));
        
        if (0 == m_iRefCount)
        {
            delete this;
        }
    }

public:
    int GetRefCount() const { return m_iRefCount; }

    // ���Ϸ� ����
    virtual void Save(const wstring& _strRelativePath) = 0;

    // ���Ϸ� ���� �ε�
    virtual int Load(const wstring& _strFilePath) = 0;

public:
    const wstring& GetKey() const { return m_strKey; }
    const wstring& GetRelativePath() const { return m_strRelativePath; }
    RES_TYPE GetResType() const { return m_eResType; }
    bool IsEngineRes() const { return m_bEngineRes; }

    void ChangeName(bool _Change) { m_bChangeName = _Change; }

private:
    void SetKey(const wstring& _strKey) { m_strKey = _strKey; }
    void SetRelativePath(const wstring& _strRelativePath) { m_strRelativePath = _strRelativePath; }

protected:
    // ���ҽ��� ��� �˻�(���� ���ϰ� �޸𸮻��� ��ΰ� 1:1�� ��Ī�Ǵ��� �˻����ش�.)
    // ������ ��ο� �ƹ��͵� ���� ���� ���� ��η� ������ �� true�� ��ȯ
    // ���� ��ΰ� ���ҽ��� ���� ��ο� ���� ���� �� ������ �� �� �ֵ��� true�� ��ȯ
    // �̹� ��ΰ� ������ �ǰ� �Է����� ���� ��ο� �ٸ� ��� false�� ��ȯ�� ������ ���´�.
    bool CheckRelativePath(const wstring& _strRelativePath);

    // Save�� �Ҷ� �ش� �Լ��� ȣ���� ������ Ű�� ��θ� ���� �����Ѵ�.
    void SaveKeyPath(FILE* _pFile);
    // Load�� �Ҷ� �ش� �Լ��� ȣ���� ������ Ű�� ��θ� ���� �о�´�.
    void LoadKeyPath(FILE* _pFile);

    virtual CRes* Clone() = 0;

public:
    CRes(RES_TYPE _eResType, bool _bEngineRes = false);
    CRes(const CRes& _other);
    ~CRes();

    friend class CResMgr;
    friend class CLandScape;

    template<typename T>
    friend class Ptr;
};

