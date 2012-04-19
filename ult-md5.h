/*
** md5 º∆À„∑‚±’¿‡
** author
**   taoabc@gmail.com
** based md5.h md5.c
** modify md5.c to md5.cpp
**   add include stdafx.h, change #include <string.h> to <string>
*/
#ifndef ULT_MD5_H_
#define ULT_MD5_H_

#include <string>

namespace ult {

std::string MD5File(const std::wstring& file_name);
std::string MD5String(const std::string& str);
}

#endif