/*
** Ìá¹©×Ö·û´®²Ù×÷
** author
**   taoabc@gmail.com
*/
#ifndef ULT_STRING_H_
#define ULT_STRING_H_

#include <string>
#include <vector>
#include <Windows.h>
#include <cwctype>

namespace ult {

inline std::wstring Utf8ToUnicode(const char* src, int len) {
  int buf_len = ::MultiByteToWideChar(CP_UTF8, 0, src, len, NULL, 0);
  wchar_t* buf = new wchar_t [buf_len];
  int out_len = ::MultiByteToWideChar(CP_UTF8, 0, src, len, buf, buf_len);
  std::wstring result(buf, out_len);
  delete[] buf;
  return result;
}

inline std::wstring Utf8ToUnicode(const std::string& src) {
  return Utf8ToUnicode(src.c_str(), static_cast<int>(src.length()));
}

inline std::string UnicodeToUtf8(const wchar_t* src, int len) {
  int buf_len = ::WideCharToMultiByte(CP_UTF8, 0, src, len, NULL, 0, NULL, NULL);
  char* buf = new char [buf_len];
  int out_len = ::WideCharToMultiByte(CP_UTF8, 0, src, len, buf, buf_len, NULL, NULL);
  std::string result(buf, out_len);
  delete[] buf;
  return result;
}

inline std::string UnicodeToUtf8(const std::wstring& src) {
  return UnicodeToUtf8(src.c_str(), static_cast<int>(src.length()));
}

inline std::wstring AnsiToUnicode(const char* src, int len) {
  int buf_len = ::MultiByteToWideChar(CP_ACP, 0, src, len, NULL, 0);
  wchar_t* buf = new wchar_t [buf_len];
  memset(buf, 0, buf_len);
  int out_len = ::MultiByteToWideChar(CP_ACP, 0, src, len, buf, buf_len);
  std::wstring result(buf, out_len);
  delete[] buf;
  return result;
}

inline std::wstring AnsiToUnicode(const std::string& src) {
  return AnsiToUnicode(src.c_str(), static_cast<int>(src.length()));
}

inline std::string UnicodeToAnsi(const wchar_t* src, int len) {
  int buf_len = ::WideCharToMultiByte(CP_ACP, 0, src, len, NULL, 0, NULL, NULL);
  char* buf = new char [buf_len];
  int out_len = ::WideCharToMultiByte(CP_ACP, 0, src, len, buf, buf_len, NULL, NULL);
  std::string result(buf, out_len);
  delete[] buf;
  return result;
}

inline std::string UnicodeToAnsi(const std::wstring& src) {
  return UnicodeToAnsi(src.c_str(), static_cast<int>(src.length()));
}

inline bool SplitString( const std::wstring& src, const std::wstring& separator,
    std::vector<std::wstring>* vec ) {
  if (src.empty() || separator.empty()) {
    return false;
  }
  int pos;
  std::wstring tmp(src);
  std::wstring item;
  int separator_len = separator.length();
  while (true) {
    pos = tmp.find(separator);
    if (0 <= pos) {
      item = tmp.substr(0, pos);
      vec->push_back(item);
      tmp = tmp.substr(pos + separator_len);
    } else {
      vec->push_back(tmp);
      break;
    }
  }
  return true;
}

class StringNoCaseComparer {

public:

  int Compare(const std::wstring& comp1, const std::wstring& comp2) {
    int len1 = comp1.length();
    int len2 = comp2.length();
    if (len1 != len2) {
      return len1 < len2 ? -1 : 1;
    }
    for (int i = 0; i < len1; ++i) {
      if (!Eq(comp1.at(i), comp2.at(i))) {
        return Lt(comp1.at(i), comp2.at(i)) ? -1 : 1;
      }
    }
    return 0;
  }

private:
  
  bool Eq(const wchar_t& c1, const wchar_t& c2) {
    return std::towupper(c1) == std::towupper(c2);
  }

  bool Lt(const wchar_t& c1, const wchar_t& c2) {
    return std::towupper(c1) < std::towupper(c2);
  }
};

inline int CompareStringNoCase( const std::wstring& comp1, const std::wstring& comp2 ) {
  StringNoCaseComparer comper;
  return comper.Compare(comp1, comp2);
}

}

#endif // ULT_CONV_H_