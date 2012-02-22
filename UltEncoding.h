#pragma once
#include <string>

namespace Ult
{
	std::wstring Utf8ToUnicode(const char* src,    int len);
	std::string  UnicodeToUtf8(const wchar_t* src, int len);
	std::string  UnicodeToUtf8(const wchar_t* src);
	std::wstring AnsiToUnicode(const char* src,    int len);
	std::wstring AnsiToUnicode(const char* src);
	std::string  UnicodeToAnsi(const wchar_t* src, int len);
	std::string  UnicodeToAnsi(const wchar_t* src);
}