/*
** md5 ¼ÆËã·â×°Àà
** author
**   taoabc@gmail.com
** based md5.h md5.c
** modify md5.c to md5.cpp
**   add include stdafx.h, change #include <string.h> to <string>
*/
#ifndef ULT_ENCRYPT_ULTMD5_H_
#define ULT_ENCRYPT_ULTMD5_H_

#include "./md5.h"
#include "../file-map.h"
#include "../string-operate.h"
#include <string>
#include <windows.h>
#include <algorithm>

namespace ult {
  
inline std::wstring MD5String(const std::string& str) {
  md5_state_s state;
  md5_byte_t digest[16];
  char hex_output[16*2 + 1];
  if (str.empty() || str.length() > 0x7fffffff) {
    return L"";
  }

  md5_init(&state);
  md5_append(&state, (const md5_byte_t*)str.c_str(), str.length());
  md5_finish(&state, digest);
  for (int di = 0; di < 16; ++di) {
    sprintf_s(hex_output + di * 2, 3, "%02x", digest[di]);
  }
  return ult::AnsiToUnicode(hex_output);
}

inline std::wstring MD5File(const std::wstring& file_name) {
  ult::FileMap file_map;
  if (!file_map.Open(file_name)) {
    return L"";
  }
  md5_state_s state;
  md5_byte_t digest[16];
  char hex_output[16*2 + 1];

  md5_init(&state);
  if (!file_map.MapFile()) {
    return L"";
  }
  LPVOID map_view = file_map.GetMapView();
  unsigned __int64 file_size = file_map.GetSize();
  if (file_size == 0) {
    return L"";
  }
  //because md5.cpp use int to get len, so here number is no greater than max of int
  //here I asume int is 4 bytes
  unsigned __int64 max_int = 0x7fffffff;
  unsigned __int64 cursor = 0;
  while (true) {
    unsigned __int64 left = file_size - cursor;
    unsigned __int64 tocalc = max_int > left ? left : max_int;
    md5_append(&state, (const md5_byte_t*)map_view+cursor, (int)tocalc);
    if (left < max_int) {
      break;
    }
    cursor += tocalc;
  }
  md5_finish(&state, digest);
  for (int di = 0; di < 16; ++di) {
    sprintf_s(hex_output + di * 2, 3, "%02x", digest[di]);
  }
  return ult::AnsiToUnicode(hex_output);
}
} //namespace ult

#endif