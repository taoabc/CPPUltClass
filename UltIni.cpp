#include "StdAfx.h"
#include "UltIni.h"
#include <Windows.h>

UltIni::UltIni(void)
{
	configFile_.Empty();
}

UltIni::~UltIni(void)
{
}

UltIni::UltIni(const CString& fileName)
{
	configFile_ = fileName;
}

void UltIni::AssignFile(const CString& fileName)
{
	configFile_ = fileName;
}

bool UltIni::WriteString(const wchar_t* section, const wchar_t* entry, const wchar_t* str)
{
	if (configFile_.IsEmpty()) {
		return false;
	}
	return WritePrivateProfileString(section, entry, str, configFile_);
}

CString UltIni::GetString(const wchar_t* section, const wchar_t* entry)
{
	if (configFile_.IsEmpty()) {
		return L"";
	}
	CString str;
	GetPrivateProfileString(section, entry, NULL, str.GetBuffer(MAX_VALUE_LEN), MAX_VALUE_LEN, configFile_);
	str.ReleaseBuffer();
	return str;
}