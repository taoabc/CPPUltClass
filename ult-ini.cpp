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

Ini::Ini(const std::wstring& filename)
{
	config_filename_ = filename;
}

void Ini::AssignFile( const std::wstring& filename )
{
	config_filename_ = filename;
}

bool Ini::WriteString(const std::wstring& section,
											const std::wstring& entry,
											const std::wstring& str)
{
	if (config_filename_.empty()) {
		return false;
	}
  return 0 != WritePrivateProfileString(section.c_str(),
		  entry.c_str(), str.c_str(), config_filename_.c_str());
}

std::wstring Ini::GetString(const std::wstring& section,
														const std::wstring& entry,
														const wchar_t* string_default /*= NULL*/)
{
	if (config_filename_.empty()) {
		return L"";
	}
  wchar_t buf[kMaxValueLen];
	GetPrivateProfileString(section.c_str(), entry.c_str(), string_default, buf, kMaxValueLen, config_filename_.c_str());
	std::wstring result(buf);
	return result;
}

bool Ini::WriteInt(const std::wstring& section, const std::wstring& entry, int num)
{
  wchar_t buf[kMaxIntLength];
  swprintf(buf, kMaxIntLength, L"%d", num);
	return WriteString(section.c_str(), entry.c_str(), buf);
}

int Ini::GetInt( const std::wstring& section, const std::wstring& entry, int num_default /*= -1*/ )
{
	return GetPrivateProfileInt(section.c_str(), entry.c_str(), num_default, config_filename_.c_str());
}
} //namespace ult