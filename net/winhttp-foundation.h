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

private:

  WinHttpHandle handle_;
}; //class WinHttpConnection
} //namespace ult

#endif