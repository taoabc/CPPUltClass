#pragma once
#include <string>
#include <vector>

namespace ult {
void DebugPrint(const std::wstring& str);
void DebugPrint(const std::string& str);
void DebugPrint(int n);
bool ReadFileToString(const std::wstring& file, std::string* dest);
bool RegQueryString(HKEY parent_key, const std::wstring& key_name, const std::wstring& value_name, std::wstring* result);
bool RegQueryWord(HKEY parent_key, const wchar_t* key_name, const wchar_t* value_name, DWORD* value);
std::wstring NumberToString(int num);
DWORD GetShellVersion(void);
void DisorderInteger(int begin_number, int end_number, std::vector<int>* vec);
void SwapInteger(int* x, int* y);
int GetRandomNumber(int min_number, int max_number);
}