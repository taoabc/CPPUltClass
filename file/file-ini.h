/*
** ini文件操作类
** author
**   taoabc@gmail.com
*/
#ifndef ULT_FILE_FILEINI_H_
#define ULT_FILE_FILEINI_H_

#include <string>
#include <Windows.h>

namespace ult {

class Ini {

public:

  Ini(void) {
    config_filename_.clear();
  }

  ~Ini(void) {
  }

  Ini(const std::wstring& filename) : config_filename_(filename) {
  }

  void AssignFile(const std::wstring& filename) {
    config_filename_.assign(filename);
  }

  bool WriteString(const std::wstring& section,
                   const std::wstring& entry,
                   const std::wstring& str) {
    if (config_filename_.empty()) {
      return false;
    }
    return 0 != WritePrivateProfileString(section.c_str(),
        entry.c_str(), str.c_str(), config_filename_.c_str());
  }

  std::wstring GetString(const std::wstring& section,
                         const std::wstring& entry,
                         const std::wstring& string_default = L"") {
    if (config_filename_.empty()) {
      return L"";
    }
    wchar_t buf[kMaxValueLen];
    const wchar_t* pdef = string_default.empty() ? NULL : string_default.c_str();
    GetPrivateProfileString(section.c_str(), entry.c_str(), pdef, buf,
        kMaxValueLen, config_filename_.c_str());
    std::wstring result(buf);
    return result;
  }

  bool WriteInt(const std::wstring& section,
                const std::wstring& entry,
                int number) {
    wchar_t buf[kMaxIntLength];
    swprintf(buf, kMaxIntLength, L"%d", number);
    return WriteString(section, entry, buf);
  }

  int GetInt(const std::wstring& section,
             const std::wstring& entry,
             int num_default = -1) {
    return GetPrivateProfileInt(section.c_str(), entry.c_str(), num_default,
        config_filename_.c_str());
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