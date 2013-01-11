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

namespace ult {

namespace detail {

struct MD5String {
  std::wstring operator()(const std::string& str) {
    if (str.empty() || str.length() > 0x7fffffff) {
      return L"";
    }

    md5_state_s state;
    md5_init(&state);
    md5_append(&state, (const md5_byte_t*)str.c_str(), str.length());
    md5_byte_t digest[16];
    md5_finish(&state, digest);
    char hex_output[16*2 + 1];
    for (int di = 0; di < 16; ++di) {
      sprintf_s(hex_output + di * 2, 3, "%02x", digest[di]);
    }
    return ult::AnsiToUnicode(hex_output);
  }
};

struct MD5File {
  std::wstring operator()(const std::wstring& file_name) {
    ult::FileMap file_map;
    if (!file_map.Open(file_name)) {
      return L"";
    }
    ULONGLONG file_size = file_map.GetFileSize();
    if (file_size == 0) {
      return L"";
    }
    md5_state_s state;
    md5_byte_t digest[16];

    md5_init(&state);
    if (!file_map.CreateMapping()) {
      return L"";
    }
    LPVOID file_view = NULL;
    ULONGLONG cursor = 0;
    DWORD map_size = -1;
    //minimum map size to try map file, 0x100000 B = 1 MB
    static const DWORD minimum_map_size = 0x100000;
    //main loop to read file from map view
    while (cursor < file_size) {
      //first part try and last part may trigger this
      if (map_size > (file_size - cursor)) {
        map_size = (DWORD)(file_size - cursor);
      }
      //sub loop to try a situable size of file view
      do {
        file_view = file_map.MapView(cursor, map_size);
        if (file_view == NULL) {
          map_size >>= 1;
        }
      } while (file_view == NULL && map_size > minimum_map_size);
      //if success, deal with it
      if (file_view != NULL) {
        md5_append(&state, (const md5_byte_t*)file_view, map_size);
        cursor += map_size;
      } else {
        return L"";
      }
    }
    md5_finish(&state, digest);
    char hex_output[16*2 + 1];
    for (int di = 0; di < 16; ++di) {
      sprintf_s(hex_output + di * 2, 3, "%02x", digest[di]);
    }
    return ult::AnsiToUnicode(hex_output);
  }
};
}
  
inline std::wstring MD5String(const std::string& str) {
  return detail::MD5String()(str);
}

inline std::wstring MD5String(const std::wstring& str) {
  return MD5String(ult::UnicodeToAnsi(str));
}

inline std::wstring MD5File(const std::wstring& file_name) {
  return detail::MD5File()(file_name);
}
} //namespace ult

#endif