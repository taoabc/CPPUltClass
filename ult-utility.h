#pragma once
#include <string>

namespace ult {
void DebugPrint(const std::wstring& str);
void DebugPrint(const std::string& str);
void DebugPrint(int n);
bool IsFileExist(const std::wstring& file);
bool ReadFileToString(const std::wstring& file, std::string* dest);
std::wstring GetKeyValue(HKEY parent_key, const std::wstring& key_name, const std::wstring& value_name);
DWORD GetShellVersion(void);
}