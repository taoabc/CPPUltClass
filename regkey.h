/*
** author
**   taoabc@gmail.com
*/
#ifndef ULT_REGKEY_H_
#define ULT_REGKEY_H_

#include <Windows.h>
#include <string>

namespace ult {

class RegKey {

public:

  RegKey(void) :
      hkey_(NULL) {
  }

  ~RegKey(void) {
    Close();
  }

  void Attach(HKEY hkey) {
    Close();
    hkey_ = hkey;
  }

  HKEY Detach(void) {
    HKEY t = hkey_;
    hkey_ = NULL;
    return t;
  }

  operator HKEY(void) const {
    return hkey_;
  }

  bool Open(HKEY key, const std::wstring& subkey, REGSAM desired = KEY_READ) {
    if (!Close()) {
      return false;
    }
    return false;
  }

  bool Open(const std::wstring& key, REGSAM desired = KEY_READ) {
    return false;
  }

  bool Close(void) {
    if (hkey_ == NULL) {
      return true;
    }
    bool result = (ERROR_SUCCESS == ::RegCloseKey(hkey_));
    hkey_ = NULL;
    return result;
  }

  bool Create(HKEY hkey, const std::wstring& subkey) {
    return false;
  }

  bool Create(const std::wstring& key) {
    return false;
  }

  bool DeleteValue(const std::wstring& value) {
  }

  bool DeleteSubKey(const std::wstring& subkey) {
  }

  bool RecurseDeleteKey(const std::wstring& key) {
  }

private:

  HKEY hkey_;
}; // class RegKey
} //namespace ult

#endif