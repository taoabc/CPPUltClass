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

#ifndef NIIF_USER
#define NIIF_USER 0x00000004
#endif

namespace ult {

class Tray {

public:

  Tray(void) : 
      is_created_(false),
      tray_icon_(NULL) {
    memset(&notify_icon_data_, 0, sizeof(notify_icon_data_));
    DWORD shell_version = GetShellVersion();
    shell_major_version_ = HIWORD(shell_version);
    shell_minor_version_ = LOWORD(shell_version);
  }

  ~Tray(void) {
    DeleteIcon();
  }

  bool Create(HWND hwnd, UINT uid, UINT ucallback_msg, HICON htray_icon, const std::wstring& tip) {
    memset(&notify_icon_data_, 0, sizeof (notify_icon_data_));
    notify_icon_data_.cbSize = GetNOTIFYICONDATASizeForOS();
    notify_icon_data_.hWnd = hwnd;
    notify_icon_data_.uID = uid;
    notify_icon_data_.uCallbackMessage = ucallback_msg;
    notify_icon_data_.hIcon = htray_icon;
    notify_icon_data_.uFlags = NIF_ICON | NIF_MESSAGE;
    if (!tip.empty()) {
      notify_icon_data_.uFlags |=  NIF_TIP;
      wcscpy(notify_icon_data_.szTip, tip.c_str());
    }

    is_created_ = AddTrayIcon();
    return is_created_;
  }

  bool Create(HWND hwnd, UINT uid, UINT ucallback_msg, HICON htray_icon) {
    return Create(hwnd, uid, ucallback_msg, htray_icon, L"");
  }

  bool Create(HWND hwnd, UINT uid, UINT ucallback_msg, UINT tray_icon_resid, const std::wstring& tip) {
    if (tray_icon_ != NULL) {
      DestroyIcon(tray_icon_);
    }
    tray_icon_ = LoadIconResource(GetModuleHandle(NULL), tray_icon_resid);
    bool result = Create(hwnd, uid, ucallback_msg, tray_icon_, tip);
    
    return result;
  }

  bool Create(HWND hwnd, UINT uid, UINT ucallback_msg, UINT tray_icon_resid) {
    if (tray_icon_ != NULL) {
      DestroyIcon(tray_icon_);
    }
    tray_icon_ = LoadIconResource(GetModuleHandle(NULL), tray_icon_resid);
    bool result = Create(hwnd, uid, ucallback_msg, tray_icon_);

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

  bool ShowBallonInfo(void) {
    if (shell_major_version_ < 5) {
      return false;
    }
    notify_icon_data_.uFlags |= NIF_INFO;
    Shell_NotifyIcon(NIM_MODIFY, &notify_icon_data_);
    return true;
  }

  bool ShowBallonInfo(const std::wstring& info_title,
                      const std::wstring& info) {
    if (!SetBallonInfo(info_title, info)) {
      return false;
    }
    return ShowBallonInfo();
  }

  bool ShowBallonInfo(const std::wstring& info) {
    if (!SetBallonInfo(info)) {
      return false;
    }
    return ShowBallonInfo();
  }

  bool SetBallonInfo(const std::wstring& info_title,
                     const std::wstring& info) {
    if (shell_major_version_ < 5) {
      return false;
    }
    notify_icon_data_.dwInfoFlags = NIIF_USER;
    wcscpy(notify_icon_data_.szInfoTitle, info_title.c_str());
    wcscpy(notify_icon_data_.szInfo, info.c_str());
    return true;
  }

  bool SetBallonInfo(const std::wstring& info) {
    if (shell_major_version_ < 5) {
      return false;
    }
    wcscpy(notify_icon_data_.szInfo, info.c_str());
    return true;
  }

  bool DeleteIcon(void) {
    if (!is_created_) {
      return true;
    } else {
      bool delete_result = (0 != Shell_NotifyIcon(NIM_DELETE, &notify_icon_data_));
      DestroyIcon(tray_icon_);
      is_created_ = false;
      return delete_result;
    }
  }

private:

  bool AddTrayIcon(void) {
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
    if (shell_major_version_ >= 5) {
      return sizeof (notify_icon_data_);
    }
    return NOTIFYICONDATA_V1_SIZE;
  }

  NOTIFYICONDATA notify_icon_data_;
  WORD shell_major_version_;
  WORD shell_minor_version_;
  bool is_created_;
  HICON tray_icon_;
};

}

#endif