#include "StdAfx.h"
#include "ult-string.h"
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

std::wstring Utf8ToUnicode(const std::string& src)
{
	return Utf8ToUnicode(src.c_str(), static_cast<int>(src.length() + 1));
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

std::string UnicodeToUtf8(const std::wstring& src)
{
	return UnicodeToUtf8(src.c_str(), static_cast<int>(src.length() + 1));
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

std::wstring AnsiToUnicode(const std::string& src)
{
	return AnsiToUnicode(src.c_str(), static_cast<int>(src.length() + 1));
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

std::string UnicodeToAnsi(const std::wstring& src)
{
	return UnicodeToAnsi(src.c_str(), static_cast<int>(src.length() + 1));
}
}//namespace Ult