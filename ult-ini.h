/*
** ini文件操作类
** author
**   taoabc@gmail.com
*/
#ifndef ULT_INI_H_
#define ULT_INI_H_

#include <string>
//#include <cstdio>
#include <Windows.h>

namespace ult {

class Ini {

public:

  Ini(void) {
    config_filename_.clear();
  }

  ~Ini(void) {
  }

  Ini(const wchar_t* filename) : config_filename_(filename) {
  }

  void AssignFile(const wchar_t* filename) {
    config_filename_.assign(filename);
  }

  bool WriteString(const wchar_t* section, const wchar_t* entry,
                   const wchar_t* str) {
    if (config_filename_.empty()) {
      return false;
    }
    return 0 != WritePrivateProfileString(section,
        entry, str, config_filename_.c_str());
  }

  std::wstring GetString(const wchar_t* section, const wchar_t* entry,
                         const wchar_t* string_default=NULL) {
    if (config_filename_.empty()) {
      return L"";
    }
    wchar_t buf[kMaxValueLen];
    GetPrivateProfileString(section, entry, string_default, buf, kMaxValueLen, config_filename_.c_str());
    std::wstring result(buf);
    return result;
  }

  bool WriteInt(const wchar_t* section, const wchar_t* entry, int number) {
    wchar_t buf[kMaxIntLength];
    swprintf(buf, kMaxIntLength, L"%d", number);
    return WriteString(section, entry, buf);
  }

  int GetInt(const wchar_t* section, const wchar_t* entry, int num_default = -1) {
    return GetPrivateProfileInt(section, entry, num_default, config_filename_.c_str());
  }

private:

	enum {
    kMaxValueLen = 1024,
    kMaxIntLength = 256
  };
	std::wstring config_filename_;
};

} //namespace ult

#endif // ULT_INI_H_