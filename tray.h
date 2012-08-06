/* 
** 提供托盘图标操作
** author:
**   taoabc@gmail.com
*/
#ifndef ULT_TRAY_H_
#define ULT_TRAY_H_

#include <string>
#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>

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

  Tray(void) : 
      is_created_(false) {
    memset(&notify_icon_data_, 0, sizeof(notify_icon_data_));
    DWORD shell_version = GetShellVersion();
    shell_major_version_ = HIWORD(shell_version);
    shell_minor_version_ = LOWORD(shell_version);
  }

  ~Tray(void) {
    DeleteIcon();
  }

  bool Create(HWND hwnd, UINT uid, UINT ucallback_msg, HICON htray_icon, const wchar_t* sztip) {
    memset(&notify_icon_data_, 0, sizeof (notify_icon_data_));
    notify_icon_data_.cbSize = GetNOTIFYICONDATASizeForOS();
    notify_icon_data_.hWnd = hwnd;
    notify_icon_data_.uID = uid;
    notify_icon_data_.uCallbackMessage = ucallback_msg;
    notify_icon_data_.hIcon = htray_icon;
    if (sztip != NULL) {
      wcscpy_s(notify_icon_data_.szTip, sztip);
    }

    is_created_ = AddTrayIcon();
    return is_created_;
  }

  bool Create(HWND hwnd, UINT uid, UINT ucallback_msg, UINT tray_icon_resid, const wchar_t* sztip) {
    HICON tray_icon = LoadIconResource(GetModuleHandle(NULL), tray_icon_resid);
    bool result = Create(hwnd, uid, ucallback_msg, tray_icon, sztip);
    DestroyIcon(tray_icon);
    return result;
  }

  bool SetIcon(HICON hicon) {
    notify_icon_data_.hIcon = hicon;
    notify_icon_data_.uFlags |= NIF_ICON;
    return (0 != Shell_NotifyIcon(NIM_MODIFY, &notify_icon_data_));
  }

  bool SetIcon(UINT resid) {
    return SetIcon(GetModuleHandle(NULL), resid);
  }

  bool SetIcon(HINSTANCE hinst, UINT resid) {
    HICON hicon = LoadIconResource(hinst, resid);
    bool result = false;
    if (hicon != NULL) {
      result = SetIcon(hicon);
      DestroyIcon(hicon);
    }
    return result;
  }

  bool ShowBallonInfo(const wchar_t* info_title, const wchar_t* info, UINT timeout) {
    if (shell_major_version_ < 5) {
      return false;
    }
    SetBallonInfo(info_title, info, timeout);
    ShowBallonInfo();
    return true;
  }

  bool ShowBallonInfo(void) {
    if (shell_major_version_ < 5) {
      return false;
    }
    notify_icon_data_.uFlags |= NIF_INFO;
    Shell_NotifyIcon(NIM_MODIFY, &notify_icon_data_);
    return true;
  }

  bool SetBallonInfo(const wchar_t* info_title, const wchar_t* info, UINT timeout) {
    if (shell_major_version_ < 5) {
      return false;
    }
    notify_icon_data_.dwInfoFlags = NIIF_USER;
    notify_icon_data_.uTimeout = timeout;
    wcscpy_s(notify_icon_data_.szInfoTitle, info_title);
    wcscpy_s(notify_icon_data_.szInfo, info);
    return true;
  }

  bool SetBallonInfoContent(const wchar_t* info) {
    if (shell_major_version_ < 5) {
      return false;
    }
    wcscpy_s(notify_icon_data_.szInfo, info);
    return true;
  }

  bool DeleteIcon(void) {
    if (!is_created_) {
      return true;
    } else {
      bool delete_result = (0 != Shell_NotifyIcon(NIM_DELETE, &notify_icon_data_));
      is_created_ = false;
      return delete_result;
    }
  }

private:

  bool AddTrayIcon(void) {
    notify_icon_data_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    return (0 != Shell_NotifyIcon(NIM_ADD, &notify_icon_data_));
  }

  HICON LoadIconResource(HINSTANCE hinst, UINT resid) {
    const wchar_t* resname = MAKEINTRESOURCE(resid);
    HICON hicon = (HICON)LoadImage(hinst, resname, IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    if (hicon == NULL) {
      hicon = (HICON)LoadImage(hinst, resname, IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    }
    return hicon;
  }

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
    if (shell_major_version_ < 5) {
      return sizeof(NOTIFYICONDATA_1);
    } else if (shell_major_version_ == 5) {
      return sizeof(NOTIFYICONDATA_2);
    } else if (shell_major_version_ == 6) {
      if (shell_minor_version_ == 0) {
        //这里因为系统版本的定义而不能使用
        //return NOTIFYICONDATA_V3_SIZE;
        return sizeof (NOTIFYICONDATA_3);
      } else if (shell_minor_version_ > 0) {
        return sizeof (NOTIFYICONDATA);
      }
    }
    return sizeof (NOTIFYICONDATA);
  }

  NOTIFYICONDATA notify_icon_data_;
  WORD shell_major_version_;
  WORD shell_minor_version_;
  bool is_created_;
};

}

#endif