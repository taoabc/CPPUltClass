/*
** md5 计算封装类
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
  UINT64 cursor = 0;
  //get allocation granularity
  SYSTEM_INFO sys_info;
  ::GetSystemInfo(&sys_info);
  DWORD glt = sys_info.dwAllocationGranularity;
  DWORD map_size = 0xffffffff - 0xffffffff % glt;
  //main loop to read file from map view
  while (cursor < file_size) {
    //first part try and last part may trigger this
    if (map_size > (file_size - cursor)) {
      map_size = (DWORD)(file_size - cursor);
    }
    //sub loop to try a situable size of file view
    do {
      //only first part try and last part
      //map_size may not be multiple of the allocation granularity
      //当尝试第一块数据的时候，如果大小没有跟分配粒度对齐
      //那么这个大小必定是文件大小，所以，如果成功，就不会出现两次映射的情况
      //当尝试中间块数据的时候，则表明已经出现过映射未成功的现象
      //那么不会触发上一句的map_size重新赋值，则map_size仍然为分配粒度的整数倍
      //如果是尝试读最后一块，那么下一次就不需要再读取，所以map_size是否为分配粒度无关
      file_view = file_map.MapView(cursor, map_size);
      if (file_view == NULL) {
        map_size >>= 1; 
        //the offset must be a multiple of the allocation granularity
        //plus a glt is avoid always calc 3 times than 2 times
        //确保大小为分配粒度的整数倍，向上多取一个粒度是为了保证在取一半文件大小的时候
        //如果是向下减掉一个粒度，那么读文件，需要多读几次，最后一段势必相当小
        map_size = (map_size + glt-1) - (map_size + glt-1) % glt;
      }
    } while (file_view == NULL && map_size > 0x100000);
    //if success, deal with it
    if (file_view != NULL) {
      md5_append(&state, (const md5_byte_t*)file_view, map_size);
      cursor += map_size;
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