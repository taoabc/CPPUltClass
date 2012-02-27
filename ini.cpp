#include "StdAfx.h"
#include "ini.h"
#include <cstdio>
#include <Windows.h>

namespace ult {
Ini::Ini(void)
{
	config_filename_.clear();
}

Ini::~Ini(void)
{
}

Ini::Ini(const wchar_t* filename)
{
	config_filename_.assign(filename);
}

void Ini::AssignFile(const wchar_t* filename)
{
	config_filename_.assign(filename);
}

bool Ini::WriteString(const wchar_t* section, const wchar_t* entry, const wchar_t* str)
{
	if (config_filename_.empty()) {
		return false;
	}
  return 0 != WritePrivateProfileString(section, entry, str, config_filename_.c_str());
}

std::wstring Ini::GetString(const wchar_t* section, const wchar_t* entry, const wchar_t* string_default /*= NULL*/)
{
	if (config_filename_.empty()) {
		return L"";
	}
  wchar_t buf[kMaxValueLen];
	GetPrivateProfileString(section, entry, string_default, buf, kMaxValueLen, config_filename_.c_str());
	std::wstring result(buf);
	return result;
}

bool Ini::WriteInt(const wchar_t* section, const wchar_t* entry, int number)
{
  wchar_t buf[kMaxIntLength];
  swprintf(buf, kMaxIntLength, L"%d", number);
	return WriteString(section, entry, buf);
}

int Ini::GetInt( const wchar_t* section, const wchar_t* entry, int num_default /*= -1*/ )
{
	return GetPrivateProfileInt(section, entry, num_default, config_filename_.c_str());
}
} //namespace ult