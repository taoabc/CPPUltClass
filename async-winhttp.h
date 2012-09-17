/*
** 异步WinHttp处理类
** author
**   taoabc@gmail.com
*/
#ifndef ULT_ASYNC_WINHTTP_H_
#define ULT_ASYNC_WINHTTP_H_

#include "handle-base.h"

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

  HRESULT Initialize(void) {
    if (!handle_.Attach(::WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC))) {
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

class WinHttpRequest {

public:

  WinHttpRequest(void) {
    buffer = new char[kBufferLength];
  }

  ~WinHttpRequest(void) {
    if (buffer != NULL) {
      delete[] buffer;
    }
  }

  HRESULT Initialize(HINTERNET connection, const wchar_t* verb, const wchar_t* path, int scheme = INTERNET_SCHEME_HTTP) {
    DWORD flag = (scheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
    if (!handle_.Attach(::WinHttpOpenRequest(connection, verb, path, NULL, WINHTTP_NO_REFERER,
         WINHTTP_DEFAULT_ACCEPT_TYPES, flag))) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    if (WINHTTP_INVALID_STATUS_CALLBACK == ::WinHttpSetStatusCallback(handle_, Callback,
        WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, NULL)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  HRESULT SendRequest(const wchar_t* headers, DWORD headers_len, const void* optional, DWORD optional_len, DWORD total_len) {
    if (FALSE == ::WinHttpSendRequest(handle_, headers, headers_len,
        const_cast<void*>(optional), optional_len, total_len, reinterpret_cast<DWORD_PTR>(this))) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

protected:

  virtual HRESULT OnCallback(DWORD code, const void* info, DWORD length) {
    return S_OK;
  }

  virtual HRESULT OnReadComplete(const void* info, DWORD length) {
    return S_OK;
  }

  virtual HRESULT OnResponseComplete(HRESULT hr) {
    return S_OK;
  }

private:

  static void CALLBACK Callback(HINTERNET handle, DWORD_PTR context, DWORD code, void* info, DWORD length) {
    WinHttpRequest* pthis = (WinHttpRequest*)context;
    HRESULT hr = pthis->OnBaseCallback(code, info, length);
    if (FAILED(hr)) {
      pthis->OnResponseComplete(hr);
    }
  }

  HRESULT OnBaseCallback(DWORD code, const void* info, DWORD length) {
    switch (code) {
    case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
    case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
      if (FALSE == ::WinHttpReceiveResponse(handle_, NULL)) {
        return HRESULT_FROM_WIN32(::GetLastError());
      }
      break;
    case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE: 
      {
        DWORD status;
        DWORD len = sizeof (status);
        if (FALSE == ::WinHttpQueryHeaders(handle_, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX, &status, &len, WINHTTP_NO_HEADER_INDEX)) {
          return HRESULT_FROM_WIN32(::GetLastError());
        }
        if (status != HTTP_STATUS_OK) {
          return E_FAIL;
        }
        if (S_OK != QueryDataAvailable()) {
          return E_FAIL;
        }
      }
      break;
    case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
      {
        DWORD size = *(reinterpret_cast<const DWORD*>(info));
        if (size > 0) {
          if (FALSE == ::WinHttpReadData(handle_, buffer, kBufferLength, NULL)) {
            return HRESULT_FROM_WIN32(::GetLastError());
          }
        } else if (size == 0) {
          OnResponseComplete(S_OK);
        }
      }
      break;
    case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
      if (length > 0) {
        OnReadComplete(info, length);
        QueryDataAvailable();
      }
      break;
    case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
      OnResponseComplete(HRESULT_FROM_WIN32(::GetLastError()));
      break;
    default:
      return OnCallback(code, info, length);
      break;
    }
  }

  HRESULT QueryDataAvailable(void) {
    if (FALSE == ::WinHttpQueryDataAvailable(handle_, NULL)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  WinHttpHandle handle_;
  void* buffer;

  enum {
    kBufferLength = 8 * 1024,
  };
}; //class WinHttpRequest

} //namespace ult

#endif