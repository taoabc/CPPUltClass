/*
** Ìá¹©×Ö·û´®²Ù×÷
** author
**   taoabc@gmail.com
*/
#ifndef ULT_STRING_OPERATE_H_
#define ULT_STRING_OPERATE_H_

#include <string>
#include <vector>
#include <Windows.h>
#include <cwctype>

namespace ult {
namespace stringoperate {

inline std::wstring MultiByteToUnicode(const char* src, int len, unsigned int codepage) {
  std::wstring dest;
  if (len > 0) {
    wchar_t* buf = new wchar_t[len];
    int outlen = ::MultiByteToWideChar(codepage, 0, src, len, buf, len);
    dest.assign(buf, outlen);
    delete[] buf;
  }
  return dest;
}

inline std::string UnicodeToMultiByte(const wchar_t* src, int len, unsigned int codepage) {
  std::string dest;
  if (len > 0) {
    char* buf = new char[len*2];
    int outlen = ::WideCharToMultiByte(codepage, 0, src, len, buf, len*2, NULL, NULL);
    dest.assign(buf, outlen);
    delete[] buf;
  }
  return dest;
}

inline std::wstring Utf8ToUnicode(const char* src, int len) {
  return MultiByteToUnicode(src, len, CP_UTF8);
}

inline std::wstring Utf8ToUnicode(const std::string& src) {
  return Utf8ToUnicode(src.c_str(), static_cast<int>(src.length()));
}

inline std::string UnicodeToUtf8(const wchar_t* src, int len) {
  return UnicodeToMultiByte(src, len, CP_UTF8);
}

inline std::string UnicodeToUtf8(const std::wstring& src) {
  return UnicodeToUtf8(src.c_str(), static_cast<int>(src.length()));
}

inline std::wstring AnsiToUnicode(const char* src, int len) {
  return MultiByteToUnicode(src, len, CP_ACP);
}

inline std::wstring AnsiToUnicode(const std::string& src) {
  return AnsiToUnicode(src.c_str(), static_cast<int>(src.length()));
}

inline std::string UnicodeToAnsi(const wchar_t* src, int len) {
  return UnicodeToMultiByte(src, len, CP_ACP);
}

inline std::string UnicodeToAnsi(const std::wstring& src) {
  return UnicodeToAnsi(src.c_str(), static_cast<int>(src.length()));
}

class StringSpliter {

public:

  static bool Split(const std::wstring& src,
                    const std::wstring& separator,
                    std::vector<std::wstring>* vec) {
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

};

inline bool SplitString(const std::wstring& src, const std::wstring& separator,
                        std::vector<std::wstring>* vec ) {
  return StringSpliter::Split(src, separator, vec);
}

class StringNoCaseComparer {

public:

  static int Compare(const std::wstring& comp1, const std::wstring& comp2) {
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
  
  static bool Eq(const wchar_t& c1, const wchar_t& c2) {
    return std::towupper(c1) == std::towupper(c2);
  }

  static bool Lt(const wchar_t& c1, const wchar_t& c2) {
    return std::towupper(c1) < std::towupper(c2);
  }
};

inline int CompareStringNoCase( const std::wstring& comp1, const std::wstring& comp2 ) {
  return StringNoCaseComparer::Compare(comp1, comp2);
}

class StringIntConverter {

public:

  static void UInt64ToString(unsigned __int64 num, wchar_t* s) {
    wchar_t temp[32];
    int pos = 0;
    do {
      temp[pos++] = (wchar_t)(L'0' + (int)(num % 10));
      num /= 10;
    } while (0 != num);
    do {
      *s++ = temp[--pos];
    } while (pos > 0);
    *s = L'\0';
  }

};

inline void IntToString(__int64 num, wchar_t* s) {
  if (num < 0) {
    *s++ = L'-';
    num = -num;
  }
  StringIntConverter::UInt64ToString(num, s);
}

inline void UIntToString(unsigned __int64 num, wchar_t* s) {
  StringIntConverter::UInt64ToString(num, s);
}

inline std::string UrlEncode(const char* s, size_t len) {
  std::string encoded;
  char* buf = new char[16];
  unsigned char c;
  for (size_t i = 0; i < len; ++i) {
    c = s[i];
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_') {
      encoded += c;
      continue;
    }
    if (c == ' ') {
      encoded += '+';
      continue;
    }
    sprintf(buf, "%x", c);
    encoded += '%';
    encoded += buf;
  }
  delete[] buf;
  return encoded;
}

inline std::string UrlEncode(const std::string& s) {
  return UrlEncode(s.c_str(), s.length());
}

} //namespace stringoperate

using namespace stringoperate;

} //namespace ult

#endif // ULT_STRING_H_