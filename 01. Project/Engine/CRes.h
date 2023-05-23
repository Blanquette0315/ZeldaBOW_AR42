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

    bool            m_bEngineRes;   // 엔진에서 제공해주는 리소스인지 Editor에서 만들어진 리소스인지 확인
    bool            m_bChangeName;  // 리소스의 이름이 변경되면 체크해준다. (우선 지금은 Mtrl이름이 변경되면 CheckRelativePath()를 통과시켜주기 위함이다.)

public:
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

    // 파일로 저장
    virtual void Save(const wstring& _strRelativePath) = 0;

    // 파일로 부터 로딩
    virtual int Load(const wstring& _strFilePath) = 0;

public:
    const wstring& GetKey() const { return m_strKey; }
    const wstring& GetRelativePath() const { return m_strRelativePath; }
    RES_TYPE GetResType() const { return m_eResType; }
    bool IsEngineRes() const { return m_bEngineRes; }

    void ChangeName(bool _Change) { m_bChangeName = _Change; }
    bool IsChangeName() { return m_bChangeName; }

protected:
    void SetKey(const wstring& _strKey) { m_strKey = _strKey; if (m_bEngineRes){SetName(m_strKey);} }
    void SetRelativePath(const wstring& _strRelativePath) { m_strRelativePath = _strRelativePath; }

protected:
    // 리소스의 경로 검사(실제 파일과 메모리상의 경로가 1:1로 배칭되는지 검사해준다.)
    // 이전에 경로에 아무것도 없을 때는 들어온 경로로 설정한 뒤 true를 반환
    // 들어온 경로가 리소스가 지닌 경로와 같을 때는 재 저장을 할 수 있도록 true를 반환
    // 이미 경로가 지정이 되고 입력으로 들어온 경로와 다를 경우 false를 반환해 저장을 막는다.
    bool CheckRelativePath(const wstring& _strRelativePath);

    // Save를 할때 해당 함수를 호출해 본인의 키와 경로를 먼저 저장한다.
    void SaveKeyPath(YAML::Emitter& _emitter);
    // Load를 할때 해당 함수를 호출해 본인의 키와 경로를 먼저 읽어온다.
    void LoadKeyPath(YAML::Node& _node);

    virtual CRes* Clone() = 0;

public:
    CRes(RES_TYPE _eResType, bool _bEngineRes = false);
    CRes(const CRes& _other);
    ~CRes();

    friend class CResMgr;
    friend class CLandScape;
    friend class CFBXLoader;

    template<typename T>
    friend class Ptr;
};

