/*
** winhttp»ù´¡Àà
** author
**   taoabc@gmail.com
*/
#ifndef ULT_NET_WINHTTPFOUNDATION_H_
#define ULT_NET_WINHTTPFOUNDATION_H_

#include "../base/handle-base.h"

#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "Winhttp.lib")

namespace ult{

class WinHttpHandle : public HandleBase<HINTERNET> {

public:

  ~WinHttpHandle(void) {
    Close();
  }

  void Close(void) {
    if (handle_ != NULL) {
      ::WinHttpCloseHandle(handle_);
      handle_ = NULL;
    }
  }

  operator HINTERNET() const {
    return handle_;
  }

  HRESULT SetOption(DWORD option, const void* value, DWORD length) {
    if (!::WinHttpSetOption(handle_, option, const_cast<void*>(value), length)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  HRESULT QueryOption(DWORD option, void* value, DWORD* length) const {
    if (!::WinHttpQueryOption(handle_, option, value, length)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }
}; //class WinHttpHandle

class WinHttpSession {

public:

  HRESULT Initialize(bool async = false) {
    DWORD flag = async ? WINHTTP_FLAG_ASYNC : 0;
    if (!handle_.Attach(::WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, flag))) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  HINTERNET GetHandle(void) const {
    return handle_.GetHandle();
  }

  void CloseHandle(void) {
    handle_.Close();
  }

private:

  WinHttpHandle handle_;
}; //class WinHttpSession

class WinHttpConnection {

public:

  HRESULT Initialize(HINTERNET session, const wchar_t* server, unsigned short port) {
    if (!handle_.Attach(::WinHttpConnect(session, server, port, 0))) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  HINTERNET GetHandle(void) const {
    return handle_.GetHandle();
  }

  void CloseHandle(void) {
    handle_.Close();
  }

private:

  WinHttpHandle handle_;
}; //class WinHttpConnection

inline bool UltWinHttpCrackUrl(const wchar_t* url, URL_COMPONENTS* uc) {
  memset(uc, 0, sizeof (*uc));
  uc->dwExtraInfoLength = -1;
  uc->dwHostNameLength = -1;
  uc->dwPasswordLength = -1;
  uc->dwSchemeLength = -1;
  uc->dwStructSize = sizeof (*uc);
  uc->dwUrlPathLength = -1;
  uc->dwUserNameLength = -1;
  return (TRUE == ::WinHttpCrackUrl(url, 0, 0, uc));
}

} //namespace ult

#endif