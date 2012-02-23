#pragma once
#include <atlstr.h>

namespace Ult
{
	CStringW Utf8ToUnicode(const char* src,    int len);
	CStringW Utf8ToUnicode(const char* src);

	CStringA UnicodeToUtf8(const wchar_t* src, int len);
	CStringA UnicodeToUtf8(const wchar_t* src);

	CStringW AnsiToUnicode(const char* src,    int len);
	CStringW AnsiToUnicode(const char* src);

	CStringA UnicodeToAnsi(const wchar_t* src, int len);
	CStringA UnicodeToAnsi(const wchar_t* src);
}