/* 
** 提供托盘图标操作
** author:
**   taoabc@gmail.com
*/
#ifndef ULT_TRAY_H_
#define ULT_TRAY_H_

#include <string>
#include <windows.h>

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
  Tray(void);
  ~Tray(void);

  bool Create(HWND hwnd, UINT uid, UINT ucallback_msg, HICON htray_icon, wchar_t* sztip);
  bool AddTrayIcon(void);
  bool ShowBallonInfo(const wchar_t* info_title, const wchar_t* info, UINT timeout);
  bool ShowBallonInfo(void);
  bool SetBallonInfo(const wchar_t* info_title, const wchar_t* info, UINT timeout);
  bool SetBallonInfoContent(const wchar_t* info);

private:
  DWORD GetShellVersion(void);
  DWORD GetNOTIFYICONDATASizeForOS(void);

  NOTIFYICONDATA notify_icon_data_;
  WORD shell_major_version_;
  WORD shell_minor_version_;
};

}

#endif