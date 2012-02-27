#include "StdAfx.h"
#include "text-encoding.h"
#include <Windows.h>

namespace ult {
std::wstring Utf8ToUnicode(const char* src, int len)
{
	int buf_len = ::MultiByteToWideChar(CP_UTF8, 0, src, len, NULL, 0);
  wchar_t* buf = new wchar_t [buf_len];
	::MultiByteToWideChar(CP_UTF8, 0, src, len, buf, buf_len);
  std::wstring result(buf);
  delete[] buf;
	return result;
}

std::wstring Utf8ToUnicode(const char* src)
{
	return Utf8ToUnicode(src, strlen(src) + 1);
}

std::string UnicodeToUtf8(const wchar_t* src, int len)
{
	int buf_len = ::WideCharToMultiByte(CP_UTF8, 0, src, len, NULL, 0, NULL, NULL);
	char* buf = new char [buf_len];
	::WideCharToMultiByte(CP_UTF8, 0, src, len, buf, buf_len, NULL, NULL);
	std::string result(buf);
  delete[] buf;
	return result;
}

std::string UnicodeToUtf8(const wchar_t* src)
{
	return UnicodeToUtf8(src, wcslen(src)+1);
}

std::wstring AnsiToUnicode(const char* src, int len)
{
	int buf_len = ::MultiByteToWideChar(CP_ACP, 0, src, len, NULL, 0);
  wchar_t* buf = new wchar_t [buf_len];
	::MultiByteToWideChar(CP_ACP, 0, src, len, buf, buf_len);
	std::wstring result(buf);
  delete[] buf;
	return result;
}

std::wstring AnsiToUnicode(const char* src)
{
	return AnsiToUnicode(src, strlen(src)+1);
}

std::string UnicodeToAnsi(const wchar_t* src, int len)
{
	int buf_len = ::WideCharToMultiByte(CP_ACP, 0, src, len, NULL, 0, NULL, NULL);
  char* buf = new char [buf_len];
	::WideCharToMultiByte(CP_ACP, 0, src, len, buf, buf_len, NULL, NULL);
	std::string result(buf);
  delete[] buf;
	return result;
}

std::string UnicodeToAnsi(const wchar_t* src)
{
	return UnicodeToAnsi(src, wcslen(src) + 1);
}
}//namespace Ult