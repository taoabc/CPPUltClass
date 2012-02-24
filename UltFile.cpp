#include "StdAfx.h"
#include "UltFile.h"
#include <Windows.h>
#include "../Utility.h"

CString Ult::GetMaxFreeSapceDisk( unsigned int* freeSpace )
{
	DWORD bufLen = GetLogicalDriveStrings(0, NULL);
	wchar_t* buf = new wchar_t[bufLen];
	CString str;
	str.Format(L"%d", bufLen);
	Ult::DebugPrint(str);
	delete[] buf;
	return L"";
}
