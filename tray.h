/* 
** 提供托盘图标操作
** author:
**   taoabc@gmail.com
*/
#ifndef ULT_TRAY_H_
#define ULT_TRAY_H_

#include <string>
#include <windows.h>
#include <Shlwapi.h>

#ifndef NIIF_USER
#define NIIF_USER 0x00000004
#endif

namespace ult {

typedef struct _NOTIFYICONDATA_1 //The version of the structure supported by Shell v4
{
  DWORD cbSize;
  HWND hWnd;
  UINT uID;
  UINT uFlags;
  UINT uCallbackMessage;
  HICON hIcon;
  TCHAR szTip[64];
} NOTIFYICONDATA_1;

typedef struct _NOTIFYICONDATA_2 //The version of the structure supported by Shell v5
{
  DWORD cbSize;
  HWND hWnd;
  UINT uID;
  UINT uFlags;
  UINT uCallbackMessage;
  HICON hIcon;
  TCHAR szTip[128];
  DWORD dwState;
  DWORD dwStateMask;
  TCHAR szInfo[256];
  union 
  {
    UINT uTimeout;
    UINT uVersion;
  } DUMMYUNIONNAME;
  TCHAR szInfoTitle[64];
  DWORD dwInfoFlags;
} NOTIFYICONDATA_2;

typedef struct _NOTIFYICONDATA_3 //The version of the structure supported by Shell v6
{
  DWORD cbSize;
  HWND hWnd;
  UINT uID;
  UINT uFlags;
  UINT uCallbackMessage;
  HICON hIcon;
  TCHAR szTip[128];
  DWORD dwState;
  DWORD dwStateMask;
  TCHAR szInfo[256];
  union 
  {
    UINT uTimeout;
    UINT uVersion;
  } DUMMYUNIONNAME;
  TCHAR szInfoTitle[64];
  DWORD dwInfoFlags;
  GUID guidItem;
} NOTIFYICONDATA_3;

class Tray {

public:

  Tray(void) {
    memset(&notify_icon_data_, 0, sizeof(notify_icon_data_));
    DWORD shell_version = GetShellVersion();
    shell_major_version_ = HIWORD(shell_version);
    shell_minor_version_ = LOWORD(shell_version);
  }

  ~Tray(void) {
    if (0 != notify_icon_data_.cbSize) {
      Shell_NotifyIcon(NIM_DELETE, &notify_icon_data_);
    }
  }

  bool Create(HWND hwnd, UINT uid, UINT ucallback_msg, HICON htray_icon, wchar_t* sztip) {
    notify_icon_data_.cbSize = GetNOTIFYICONDATASizeForOS();
    notify_icon_data_.hWnd = hwnd;
    notify_icon_data_.uID = uid;
    notify_icon_data_.uCallbackMessage = ucallback_msg;
    notify_icon_data_.hIcon = htray_icon;
    wcscpy(notify_icon_data_.szTip, sztip);

    AddTrayIcon();

    return true;
  }

  bool AddTrayIcon(void) {
    notify_icon_data_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    Shell_NotifyIcon(NIM_ADD, &notify_icon_data_);
    return true;
  }

  bool ShowBallonInfo(const wchar_t* info_title, const wchar_t* info, UINT timeout) {
    if (5 > shell_major_version_) {
      return false;
    }
    SetBallonInfo(info_title, info, timeout);
    ShowBallonInfo();
    return true;
  }

  bool ShowBallonInfo(void) {
    if (5 > shell_major_version_) {
      return false;
    }
    notify_icon_data_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
    Shell_NotifyIcon(NIM_MODIFY, &notify_icon_data_);
    return true;
  }

  bool SetBallonInfo(const wchar_t* info_title, const wchar_t* info, UINT timeout) {
    if (5 > shell_major_version_) {
      return false;
    }
    notify_icon_data_.dwInfoFlags = NIIF_USER;
    notify_icon_data_.uTimeout = timeout;
    wcscpy(notify_icon_data_.szInfoTitle, info_title);
    wcscpy(notify_icon_data_.szInfo, info);
    return true;
  }

  bool SetBallonInfoContent(const wchar_t* info) {
    if (5 > shell_major_version_) {
      return false;
    }
    wcscpy(notify_icon_data_.szInfo, info);
    return true;
  }

private:

  DWORD GetShellVersion(void) {
    HINSTANCE hinst_dll;
    DWORD dwversion = 0;
    hinst_dll = LoadLibrary(L"Shell32.dll");
    if (NULL != hinst_dll) {
      DLLGETVERSIONPROC pfn_dll_get_version;
      pfn_dll_get_version = (DLLGETVERSIONPROC)GetProcAddress(hinst_dll, "DllGetVersion");
      if (NULL != pfn_dll_get_version) {
        DLLVERSIONINFO dvi;
        HRESULT hr;

        memset(&dvi, 0, sizeof(dvi));
        dvi.cbSize = sizeof(dvi);
        hr = pfn_dll_get_version(&dvi);
        if (SUCCEEDED(hr)) {
          dwversion = MAKELONG(dvi.dwMinorVersion, dvi.dwMajorVersion);
        }
      }
      FreeLibrary(hinst_dll);
    }
    return dwversion;
  }

  DWORD GetNOTIFYICONDATASizeForOS(void) {
    if (5 > shell_major_version_) {
      return sizeof(NOTIFYICONDATA_1);
    } else if (5 == shell_major_version_) {
      return sizeof(NOTIFYICONDATA_2);
    } else if (6 == shell_major_version_) {
      if (0 == shell_minor_version_) {
        //这里因为系统版本的定义而不能使用
        //return NOTIFYICONDATA_V3_SIZE;
        return sizeof (NOTIFYICONDATA_3);
      } else if (0 < shell_minor_version_) {
        return sizeof (NOTIFYICONDATA);
      }
    }
    return sizeof (NOTIFYICONDATA);
  }

  NOTIFYICONDATA notify_icon_data_;
  WORD shell_major_version_;
  WORD shell_minor_version_;

};

}

#endif