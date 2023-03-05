#pragma once
#include "UI.h"

class TreeUI;

class ContentUI :
    public UI
{
private:
    TreeUI*             m_Tree;
    vector<wstring>     m_vecContentName; // ���ҽ� ���ϵ��� �̸��� �����ϴ� vector

private:
    void SetResourceToInspector(DWORD_PTR _res);
    // ������ �ִ� ���ҽ� �̸��� �˾ƿ��� �Լ��̴�.
    void FindContentFileName(const wstring& _strFolderPath);
    // ������ Ȯ���ڸ� Ȯ���� ���ҽ� Ÿ���� Ȯ�����ִ� �Լ��̴�.
    RES_TYPE GetResTypeByExt(wstring _filename);

public:
    // ���� ���ҽ� ����� Ʈ���� ����
    void ResetContent();

    // Content ���� �ؿ� ���ϵ��� �޸𸮿� �ҷ���
    void ReloadContent();

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    ContentUI();
    ~ContentUI();
};

