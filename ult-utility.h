#pragma once
#include <string>

namespace ult {
void DebugPrint(const std::wstring& str);
void DebugPrint(const std::string& str);
void DebugPrint(int n);
bool ReadFileToString(const std::wstring& file, std::string* dest);
std::wstring RegQueryString(HKEY parent_key, const std::wstring& key_name, const std::wstring& value_name);
bool RegQueryWord(HKEY parent_key, const wchar_t* key_name, const wchar_t* value_name, DWORD* value);
std::wstring NumberToString(int num);
DWORD GetShellVersion(void);
}