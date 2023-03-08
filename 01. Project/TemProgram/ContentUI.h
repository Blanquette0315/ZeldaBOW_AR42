#pragma once
#include "UI.h"

class TreeUI;

class ContentUI :
    public UI
{
private:
    TreeUI*             m_Tree;
    vector<wstring>     m_vecContentName; // ���ҽ� ���ϵ��� �̸��� �����ϴ� vector
    vector<wstring>     m_vecLevelName;     // All Levels Relative Path Container

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

    // Level path validation : It checks whether the same file exists in the level folder with the path received as a factor.
    bool Is_ValidLvPath(const wstring& _strLvRelativePath);

public:
    virtual void update() override;
    virtual void render_update() override;

public:
    ContentUI();
    ~ContentUI();
};

