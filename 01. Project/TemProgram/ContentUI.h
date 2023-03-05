#pragma once
#include "UI.h"

class TreeUI;

class ContentUI :
    public UI
{
private:
    TreeUI*             m_Tree;
    vector<wstring>     m_vecContentName; // 리소스 파일들의 이름을 저장하는 vector

private:
    void SetResourceToInspector(DWORD_PTR _res);
    // 폴더에 있는 리소스 이름을 알아오는 함수이다.
    void FindContentFileName(const wstring& _strFolderPath);
    // 파일의 확장자를 확인해 리소스 타입을 확인해주는 함수이다.
    RES_TYPE GetResTypeByExt(wstring _filename);

public:
    // 현재 리소스 목록을 트리에 갱신
    void ResetContent();

    // Content 폴더 밑에 파일들을 메모리에 불러옴
    void ReloadContent();

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    ContentUI();
    ~ContentUI();
};

