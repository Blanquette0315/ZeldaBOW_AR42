#include "pch.h"
#include "UIFunc.h"

bool IS_LevelRelativePath(DWORD_PTR _data)
{
	// if that data is Level RelativePath
	if (wstring::npos != ((wstring*)_data)->find(L".lv"))
	{
		return true;
	}
	else
	{
		return false;
	}
}