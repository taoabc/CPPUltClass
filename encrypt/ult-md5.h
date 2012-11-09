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

//this function have a big problem with tail part read
inline std::wstring MD5File(const std::wstring& file_name) {
  ult::FileMap file_map;
  if (!file_map.Open(file_name)) {
    return L"";
  }
  UINT64 file_size = file_map.GetSize();
  if (file_size == 0) {
    return L"";
  }
  md5_state_s state;
  md5_byte_t digest[16];
  char hex_output[16*2 + 1];

  md5_init(&state);
  if (!file_map.CreateMapping()) {
    return L"";
  }
  LPVOID file_view = NULL;
  //first, try 1GB size
  DWORD map_size = 0x40000000;
  UINT64 cursor = 0;
  DWORD before_cursor = 0;
  DWORD useful_size = 0;
  //get allocation granularity
  SYSTEM_INFO sys_info;
  ::GetSystemInfo(&sys_info);
  DWORD glt = sys_info.dwAllocationGranularity;
  //main loop to read file from map view
  while (cursor < file_size) {
    if (map_size > (file_size - cursor)) {
      map_size = (DWORD)(file_size - cursor);
    }
    //sub loop to try a situable size of file view
    //I assume that when mapping 1MB size fail, that fial
    do {
      //the offset must be a multiple of the allocation granularity
      before_cursor = cursor % glt;
      //map_size = 1024 * 1024 * 1024 * 50;
      file_view = file_map.MapView(cursor - before_cursor, map_size);
      if (file_view == NULL) {
        map_size >>= 1;
      }
    } while (file_view == NULL && map_size > 0x100000);
    //if success, deal with it
    if (file_view != 0) {
      useful_size = map_size - before_cursor;
      md5_append(&state, (const md5_byte_t*)file_view+before_cursor, useful_size);
      cursor += useful_size;
    } else {
      return L"";
    }
  }
  md5_finish(&state, digest);
  for (int di = 0; di < 16; ++di) {
    sprintf_s(hex_output + di * 2, 3, "%02x", digest[di]);
  }
  return ult::AnsiToUnicode(hex_output);
}
} //namespace ult

#endif