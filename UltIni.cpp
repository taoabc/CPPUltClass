#include "StdAfx.h"
#include "UltIni.h"
#include <Windows.h>

UltIni::UltIni(void)
{
	config_filename_.Empty();
}

UltIni::~UltIni(void)
{
}

UltIni::UltIni(const CString& filename)
{
	config_filename_ = filename;
}

void UltIni::AssignFile(const CString& filename)
{
	config_filename_ = filename;
}

bool UltIni::WriteString(const wchar_t* section, const wchar_t* entry, const wchar_t* str)
{
	if (config_filename_.IsEmpty()) {
		return false;
	}
	return 0 != WritePrivateProfileString(section, entry, str, config_filename_);
}

CString UltIni::GetString(const wchar_t* section, const wchar_t* entry, const wchar_t* string_default /*= NULL*/)
{
	if (config_filename_.IsEmpty()) {
		return L"";
	}
	CString ret;
	GetPrivateProfileString(section, entry, string_default, ret.GetBuffer(kMaxValueLen), kMaxValueLen, config_filename_);
	ret.ReleaseBuffer();
	return ret;
}

bool UltIni::WriteInt(const wchar_t* section, const wchar_t* entry, int number)
{
	CString str;
	str.Format(L"%d", number);
	return WriteString(section, entry, str);
}

int UltIni::GetInt( const wchar_t* section, const wchar_t* entry, int num_default /*= -1*/ )
{
	return GetPrivateProfileInt(section, entry, num_default, config_filename_);
}
