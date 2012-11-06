/*
** 异步WinHttp处理类
** author
**   taoabc@gmail.com
*/
#ifndef ULT_NET_ASYNCWINHTTPREQUEST_H_
#define ULT_NET_ASYNCWINHTTPREQUEST_H_

#include "./winhttp-foundation.h"

namespace ult{

class AsyncWinHttpRequest {

public:

  AsyncWinHttpRequest(void) :
      callback_setted_(false) {
    buffer_ = new char[kBufferLength];
  }

  virtual ~AsyncWinHttpRequest(void) {
    if (buffer_ != NULL) {
      delete[] buffer_;
    }
    if (callback_setted_) {
      ::WinHttpSetStatusCallback(handle_, NULL, 0, NULL);
    }
  }

  void Close(void) {
    handle_.Close();
  }

  HRESULT Initialize(HINTERNET connection, const wchar_t* verb, const wchar_t* path, int scheme = INTERNET_SCHEME_HTTP) {
    DWORD flag = (scheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
    if (!handle_.Attach(::WinHttpOpenRequest(connection, verb, path, NULL, WINHTTP_NO_REFERER,
         WINHTTP_DEFAULT_ACCEPT_TYPES, flag))) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    if (WINHTTP_INVALID_STATUS_CALLBACK == ::WinHttpSetStatusCallback(handle_, Callback,
        WINHTTP_CALLBACK_FLAG_ALL_NOTIFICATIONS, NULL)) {
      callback_setted_ = false;
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    callback_setted_ = true;
    return S_OK;
  }

  HRESULT SendRequest(const wchar_t* headers, DWORD headers_len, const void* optional, DWORD optional_len, DWORD total_len) {
    if (FALSE == ::WinHttpSendRequest(handle_, headers, headers_len,
        const_cast<void*>(optional), optional_len, total_len, reinterpret_cast<DWORD_PTR>(this))) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  HRESULT WriteData(const void* data, DWORD length) {
    if (FALSE == ::WinHttpWriteData(handle_, data, length, NULL)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  HRESULT RecieveResponse(void) {
    if (FALSE == ::WinHttpReceiveResponse(handle_, NULL)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

protected:

  virtual HRESULT OnCallback(DWORD code, const void* info, DWORD length) {
    return S_OK;
  }

  virtual HRESULT OnContentLength(DWORD length) {
    return S_OK;
  }

  virtual HRESULT OnReadComplete(const void* info, DWORD length) {
    return S_OK;
  }

  virtual HRESULT OnResponseComplete(HRESULT hr) {
    return S_OK;
  }

  virtual HRESULT OnSendRequestComplete(void) {
    return S_OK;
  }

  virtual HRESULT OnWriteDataComplete(void) {
    return S_OK;
  }

private:

  static void CALLBACK Callback(HINTERNET handle, DWORD_PTR context, DWORD code, void* info, DWORD length) {
    AsyncWinHttpRequest* pthis = (AsyncWinHttpRequest*)context;
    HRESULT hr = pthis->OnBaseCallback(code, info, length);
    if (FAILED(hr)) {
      pthis->OnResponseComplete(hr);
    }
  }

  HRESULT OnBaseCallback(DWORD code, const void* info, DWORD length) {
    switch (code) {
    case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
      OnSendRequestComplete();
      break;
    case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
      OnWriteDataComplete();
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
        DWORD content_length;
        len = sizeof (content_length);
        if (TRUE == ::WinHttpQueryHeaders(handle_, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX, &content_length, &len, WINHTTP_NO_HEADER_INDEX)) {
          OnContentLength(content_length);
        }
      }
      break;
    case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
      {
        DWORD size = *(reinterpret_cast<const DWORD*>(info));
        if (size > 0) {
          if (FALSE == ::WinHttpReadData(handle_, buffer_, kBufferLength, NULL)) {
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
    return S_OK;
  }

  HRESULT QueryDataAvailable(void) {
    if (FALSE == ::WinHttpQueryDataAvailable(handle_, NULL)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  //pritave variable
  WinHttpHandle handle_;
  void* buffer_;
  bool callback_setted_;
  //private constant
  enum {
    kBufferLength = 8 * 1024,
  };
}; //class WinHttpRequest

} //namespace ult

#endif