/*
** 异步WinHttp处理类
** author
**   taoabc@gmail.com
*/
#ifndef ULT_ASYNC_WINHTTP_H_
#define ULT_ASYNC_WINHTTP_H_

#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "Winhttp.lib")

namespace ult{

class WinHttpHandle {

public:

  WinHttpHandle(void) :
      handle_(NULL) {
  }

  ~WinHttpHandle(void) {
    Close();
  }

  operator HINTERNET() const {
    return handle_;
  }

  bool Attach(HINTERNET handle) {
    Close();
    handle_ = handle;
    return handle_ != NULL;
  }

  HINTERNET Detach(void) {
    HINTERNET h = handle_;
    handle_ = NULL;
    return h;
  }

  void Close(void) {
    if (handle_ != NULL) {
      ::WinHttpCloseHandle(handle_);
      handle_ = NULL;
    }
  }

  HINTERNET GetHandle(void) const {
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

private:

  HINTERNET handle_;
}; //class WinHttpHandle

class WinHttpSession : public WinHttpHandle {

public:

  HRESULT Initialize(void) {
    if (!Attach(::WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC))) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }
}; //class WinHttpSession

class WinHttpConnection : public WinHttpHandle {

public:

  HRESULT Initialize(HINTERNET session, const wchar_t* server, unsigned short port) {
    if (!Attach(::WinHttpConnect(session, server, port, 0))) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }
}; //class WinHttpConnection

class WinHttpRequest : public WinHttpHandle {

public:

  HRESULT Initialize(HINTERNET connection, const wchar_t* verb, const wchar_t* path, bool secure = false) {
    DWORD flag = secure ? WINHTTP_FLAG_SECURE : 0;
    if (!Attach(::WinHttpOpenRequest(connection, verb, path, NULL, WINHTTP_NO_REFERER,
         WINHTTP_DEFAULT_ACCEPT_TYPES, flag))) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    if (WINHTTP_INVALID_STATUS_CALLBACK == ::WinHttpSetStatusCallback(GetHandle(), Callback,
        WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, NULL)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  HRESULT SendRequest(const wchar_t* headers, DWORD headers_len, const void* optional, DWORD optional_len, DWORD total_len) {
    
  }

protected:

  virtual HRESULT OnCallback(DWORD code, const void* info, DWORD length) {
  }

private:

  static void CALLBACK Callback(HINTERNET handle, DWORD_PTR context, DWORD code, void* info, DWORD length) {
    WinHttpRequest* pthis = (WinHttpRequest*)context;
    pthis->OnCallback(code, info, length);
  }
};

} //namespace ult

#endif