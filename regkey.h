/*
** ×¢²á±íÏà¹Ø
** author
**   taoabc@gmail.com
*/
#ifndef ULT_REGKEY_H_
#define ULT_REGKEY_H_

#include <Windows.h>
#include <string>
#include <cassert>

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

  bool Open(HKEY hkey, const std::wstring& subkey, REGSAM desired = KEY_READ) {
    if (!Close()) {
      return false;
    }
    HKEY temp;
    if (ERROR_SUCCESS != ::RegOpenKeyEx(hkey, subkey.c_str(), 0, desired, &temp)) {
      return false;
    }
    hkey_ = temp;
    return true;
  }

  bool Open(const std::wstring& key, REGSAM desired = KEY_READ) {
    HKEY hkey;
    std::wstring subkey;
    if (!ToParentAndSub(key, &hkey, &subkey)) {
      return false;
    }
    return Open(hkey, subkey, desired);
  }

  bool Close(void) {
    if (hkey_ == NULL) {
      return true;
    }
    bool result = (ERROR_SUCCESS == ::RegCloseKey(hkey_));
    hkey_ = NULL;
    return result;
  }

  bool Create(HKEY hkey, const std::wstring& subkey, DWORD options = REG_OPTION_NON_VOLATILE,
      DWORD desired = KEY_READ | KEY_WRITE, LPDWORD disposition = NULL) {
    HKEY temp;
    if (ERROR_SUCCESS != ::RegCreateKeyEx(hkey, subkey.c_str(), 0, NULL, options, desired, NULL, &temp, disposition)) {
      return false;
    }
    hkey_ = temp;
    return true;
  }

  bool Create(const std::wstring& key, DWORD options = REG_OPTION_NON_VOLATILE,
      DWORD desired = KEY_READ | KEY_WRITE, LPDWORD disposition = NULL) {
    HKEY hkey;
    std::wstring subkey;
    if (!ToParentAndSub(key, &hkey, &subkey)) {
      return false;
    }
    return Create(hkey, subkey, options, desired, disposition);
  }

  bool DeleteValue(const std::wstring& value) {
    assert(hkey_ != NULL);
    return ERROR_SUCCESS == ::RegDeleteValue(hkey_, value.c_str());
  }

  bool DeleteSubKey(const std::wstring& subkey) {
    assert(hkey_ != NULL);
    return ERROR_SUCCESS == ::RegDeleteKey(hkey_, subkey.c_str());
  }

  bool RecurseDeleteKey(const std::wstring& key) {
    assert(hkey_ != NULL);
    ult::RegKey regkey;
    if (!regkey.Open(hkey_, key, KEY_READ | KEY_WRITE)) {
      return false;
    }
    std::wstring name;
    while (regkey.EnumKey(0, &name)) {
      if (!regkey.RecurseDeleteKey(name)) {
        return false;
      }
    }
    return DeleteSubKey(key);
  }

  bool Flush(void) {
    assert(hkey_ != NULL);
    return ERROR_SUCCESS == ::RegFlushKey(hkey_);
  }

  bool EnumKey(DWORD index, std::wstring* name, std::wstring* defined_class = NULL, PFILETIME writetime = NULL) {
    assert(hkey_ != NULL);
    wchar_t buf_name[MAX_PATH];
    DWORD buf_name_len = MAX_PATH;
    wchar_t buf_class[MAX_PATH];
    DWORD buf_class_len = MAX_PATH;
    if (ERROR_SUCCESS != ::RegEnumKeyEx(hkey_, index, buf_name, &buf_name_len, NULL, buf_class, &buf_class_len, writetime)) {
      return false;
    }
    name->assign(buf_name, buf_name_len);
    if (defined_class != NULL) {
      defined_class->assign(buf_class, buf_class_len);
    }
    return true;
  }

  bool GetValue(const std::wstring& field, LPVOID data = NULL, LPDWORD data_len = NULL, LPDWORD type = NULL) {
    assert(hkey_ != NULL);
    LONG ret = ::RegQueryValueEx(hkey_, field.c_str(), NULL, type, (LPBYTE)data, data_len);
    return ret == ERROR_SUCCESS;
  }

  bool SetValue(const std::wstring& field, DWORD type, LPCVOID data, DWORD data_len) {
    assert(hkey_ != NULL);
    LONG ret = ::RegSetValueEx(hkey_, field.c_str(), 0, type, (const BYTE*)data, data_len);
    return ret == ERROR_SUCCESS;
  }

  DWORD GetDwordValue(const std::wstring& field) {
    DWORD type;
    DWORD data;
    DWORD len = sizeof (data);
    if (!GetValue(field, &data, &len, &type)) {
      return -1;
    }
    if (type != REG_DWORD) {
      return -1;
    }
    return data;
  }

  bool SetDwordValue(const std::wstring& field, DWORD value) {
    DWORD len = sizeof (value);
    if (!SetValue(field, REG_DWORD, &value, len)) {
      return false;
    }
    return true;
  }

  std::wstring GetStringValue(const std::wstring& field) {
    DWORD type;
    static const size_t kBufferSize = 2048;
    wchar_t* data = new wchar_t[kBufferSize];
    DWORD len = sizeof (wchar_t) * kBufferSize;
    if (!GetValue(field, data, &len, &type)) {
      delete[] data;
      return L"";
    }
    std::wstring result(data);
    delete[] data;
    if (type != REG_SZ) {
      return L"";
    }
    return result;
  }

  bool SetStringValue(const std::wstring& field, const std::wstring& value) {
    DWORD len = value.length() * sizeof (wchar_t);
    if (!SetValue(field, REG_SZ, value.c_str(), len)) {
      return false;
    }
    return true;
  }

private:

  struct ParentKeyMap {
    LPCWSTR fullname;
    LPCWSTR shortname;
    HKEY hkey;
  };

  bool ToParentAndSub(const std::wstring& key, HKEY* parent, std::wstring* sub) {
    int count = sizeof(key_map_) / sizeof (key_map_[0]);
    bool result = false;
    int a = sizeof (key_map_);
    for (int i = 0; i < count; ++i) {
      if (0 == key.find(key_map_[i].fullname) || 0 == key.find(key_map_[i].shortname)) {
        *parent = key_map_[i].hkey;
        int pos = key.find(L'\\');
        if (pos != std::wstring::npos) {
          *sub = key.substr(pos+1);
        }
        result = true;
        break;
      }
    }
    return result;
  }

  static const ParentKeyMap key_map_[5];
  HKEY hkey_;
}; // class RegKey

__declspec(selectany) const RegKey::ParentKeyMap RegKey::key_map_[5] = {
  {L"HKEY_CLASSES_ROOT", L"HKCR", HKEY_CLASSES_ROOT},
  {L"HKEY_CURRENT_CONFIG", L"HKCC", HKEY_CURRENT_CONFIG},
  {L"HKEY_CURRENT_USER", L"HKCU", HKEY_CURRENT_USER},
  {L"HKEY_LOCAL_MACHINE", L"HKLM", HKEY_LOCAL_MACHINE},
  {L"HKEY_USERS", L"HKU", HKEY_USERS},
};

} //namespace ult

#endif