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
#include "../file-io.h"
#include "../string-operate.h"
#include <string>
#include <vector>
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
    ult::File file;
    if (!file.Open(file_name)) {
      return L"";
    }
    ULONGLONG remain = file.GetSize();
    if (remain == 0) {
      return L"";
    }
    md5_state_s state;
    md5_byte_t digest[16];

    md5_init(&state);
    static const size_t buffer_len = 8192;
    std::vector<char> buffer(buffer_len);
    DWORD readed;
    while (remain > 0) {
      if (!file.Read(buffer.data(), buffer_len, &readed)) {
        break;
      }
      md5_append(&state, (const md5_byte_t*)buffer.data(), readed);
      remain -= readed;
    }
    md5_finish(&state, digest);
    char hex_output[16*2 + 1];
    for (int di = 0; di < 16; ++di) {
      sprintf_s(hex_output + di * 2, 3, "%02x", digest[di]);
    }
    return ult::AnsiToUnicode(hex_output);
  }
};
} //namespace detail
  
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