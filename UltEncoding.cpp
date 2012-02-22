#include "StdAfx.h"
#include "UltEncoding.h"
#include <Windows.h>

std::wstring Ult::Utf8ToUnicode(const char* src, int len)
{
	int destLen = ::MultiByteToWideChar(CP_UTF8, 0, src, len, NULL, 0);
	wchar_t *buf = new wchar_t[destLen];
	::MultiByteToWideChar(CP_UTF8, 0, src, len, buf, destLen*sizeof(wchar_t));
	std::wstring dest;
	dest.assign(buf, destLen);
	delete[] buf;
	return dest;
}

std::string Ult::UnicodeToUtf8(const wchar_t* src, int len)
{
	int destLen = ::WideCharToMultiByte(CP_UTF8, 0, src, len, NULL, 0, NULL, NULL);
	char *buf = new char[destLen];
	::WideCharToMultiByte(CP_UTF8, 0, src, len, buf, destLen, NULL, NULL);
	std::string dest;
	dest.assign(buf, destLen);
	delete[] buf;
	return dest;
}

std::string Ult::UnicodeToUtf8(const wchar_t* src)
{
	return UnicodeToUtf8(src, wcslen(src)+1);
}

std::wstring Ult::AnsiToUnicode(const char* src, int len)
{
	int destLen = ::MultiByteToWideChar(CP_ACP, 0, src, len, NULL, 0);
	wchar_t *buf = new wchar_t[destLen];
	::MultiByteToWideChar(CP_ACP, 0, src, strlen(src)+1, buf, destLen*sizeof(wchar_t));
	std::wstring dest;
	dest.assign(buf, destLen);
	delete[] buf;
	return dest;
}

std::wstring Ult::AnsiToUnicode(const char* src)
{
	return AnsiToUnicode(src, strlen(src)+1);
}

std::string Ult::UnicodeToAnsi(const wchar_t* src, int len)
{
	int destLen = ::WideCharToMultiByte(CP_ACP, 0, src, len, NULL, 0, NULL, NULL);
	char* buf = new char[destLen];
	::WideCharToMultiByte(CP_ACP, 0, src, len, buf, destLen, NULL, NULL);
	std::string dest;
	dest.assign(buf, destLen);
	delete[] buf;
	return dest;
}

std::string Ult::UnicodeToAnsi(const wchar_t* src)
{
	return UnicodeToAnsi(src, wcslen(src) + 1);
}