#include "StdAfx.h"
#include "UltConv.h"
#include <Windows.h>

namespace Ult {
CStringW Utf8ToUnicode(const char* src, int len)
{
	int l = ::MultiByteToWideChar(CP_UTF8, 0, src, len, NULL, 0);
	CStringW str;
	::MultiByteToWideChar(CP_UTF8, 0, src, len, str.GetBuffer(l), l);
	str.ReleaseBuffer();
	return str;
}

CStringW Utf8ToUnicode(const char* src)
{
	return Utf8ToUnicode(src, strlen(src) + 1);
}

CStringA UnicodeToUtf8(const wchar_t* src, int len)
{
	int l = ::WideCharToMultiByte(CP_UTF8, 0, src, len, NULL, 0, NULL, NULL);
	CStringA str;
	::WideCharToMultiByte(CP_UTF8, 0, src, len, str.GetBuffer(l), l, NULL, NULL);
	str.ReleaseBuffer();
	return str;
}

CStringA UnicodeToUtf8(const wchar_t* src)
{
	return UnicodeToUtf8(src, wcslen(src)+1);
}

CStringW AnsiToUnicode(const char* src, int len)
{
	int l = ::MultiByteToWideChar(CP_ACP, 0, src, len, NULL, 0);
	CStringW str;
	::MultiByteToWideChar(CP_ACP, 0, src, strlen(src)+1, str.GetBuffer(l), l);
	str.ReleaseBuffer();
	return str;
}

CStringW AnsiToUnicode(const char* src)
{
	return AnsiToUnicode(src, strlen(src)+1);
}

CStringA UnicodeToAnsi(const wchar_t* src, int len)
{
	int l = ::WideCharToMultiByte(CP_ACP, 0, src, len, NULL, 0, NULL, NULL);
	CStringA str;
	::WideCharToMultiByte(CP_ACP, 0, src, len, str.GetBuffer(l), l, NULL, NULL);
	str.ReleaseBuffer();
	return str;
}

CStringA UnicodeToAnsi(const wchar_t* src)
{
	return UnicodeToAnsi(src, wcslen(src) + 1);
}
}//namespace Ult