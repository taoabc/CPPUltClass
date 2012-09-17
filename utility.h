#pragma once

#include <windows.h>

namespace ult {

DWORD GetShellVersion(void) {
  HMODULE hmodule_dll;
  DWORD dwversion = 0;
  hmodule_dll = LoadLibrary(L"Shell32.dll");
  if (NULL != hmodule_dll) {
    DLLGETVERSIONPROC pfn_dll_get_version;
    pfn_dll_get_version = (DLLGETVERSIONPROC)GetProcAddress(hmodule_dll, "DllGetVersion");
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
    FreeLibrary(hmodule_dll);
  }
  return dwversion;
}
}