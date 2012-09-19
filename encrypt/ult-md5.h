/*
** md5 º∆À„∑‚±’¿‡
** author
**   taoabc@gmail.com
** based md5.h md5.c
** modify md5.c to md5.cpp
**   add include stdafx.h, change #include <string.h> to <string>
*/
#ifndef ULT_ENCRYPT_ULTMD5_H_
#define ULT_ENCRYPT_ULTMD5_H_

#include "./md5.h"
#include <string>
#include <windows.h>

namespace ult {

static const int kBufferSize = 1024;

inline std::string MD5String(const std::string& str) {
  md5_state_s state;
  md5_byte_t digest[16];
  char hex_output[16*2 + 1];

  md5_init(&state);
  md5_append(&state, (const md5_byte_t*)str.c_str(), str.length());
  md5_finish(&state, digest);
  for (int di = 0; di < 16; ++di) {
    sprintf_s(hex_output + di * 2, 3, "%02x", digest[di]);
  }
  return hex_output;
}

inline std::string MD5File(const std::wstring& file_name) {
  HANDLE hfile = CreateFile(file_name.c_str(), GENERIC_READ, FILE_SHARE_READ,
    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (INVALID_HANDLE_VALUE == hfile) {
    return "";
  }
  md5_state_s state;
  md5_byte_t digest[16];
  char hex_output[16*2 + 1];
  char buffer[kBufferSize];

  md5_init(&state);

  DWORD dwread;
  while (0 != ReadFile(hfile, buffer, kBufferSize, &dwread, NULL) && 0 < dwread) {
    md5_append(&state, (const md5_byte_t*)buffer, dwread);
  }
  CloseHandle(hfile);
  md5_finish(&state, digest);
  for (int di = 0; di < 16; ++di) {
    sprintf_s(hex_output + di * 2, 3, "%02x", digest[di]);
  }
  return hex_output;
}
} //namespace ult

#endif