/*
** Õ¨≤ΩWinHttp«Î«Û
** author
**   taoabc@gmail.com
*/
#ifndef ULT_NET_SYNCWINHTTP_H_
#define ULT_NET_SYNCWINHTTP_H_

#include "./winhttp-foundation.h"

namespace ult {

class SyncWinHttpRequest {

public:

  SyncWinHttpRequest(void) {
    buffer_ = new char[kBufferLength];
  }

  virtual ~SyncWinHttpRequest(void) {
    if (buffer_ != NULL) {
      delete[] buffer_;
    }
  }

  HRESULT Initialize(HINTERNET connection, const wchar_t* verb, const wchar_t* path, int scheme = INTERNET_SCHEME_HTTP) {
    DWORD flag = (scheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
    if (!handle_.Attach(::WinHttpOpenRequest(connection, verb, path, NULL, WINHTTP_NO_REFERER,
         WINHTTP_DEFAULT_ACCEPT_TYPES, flag))) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  HRESULT SendRequest(const wchar_t* headers, DWORD headers_len, const void* optional, DWORD optional_len, DWORD total_len) {
    if (FALSE == ::WinHttpSendRequest(handle_, headers, headers_len,
        const_cast<void*>(optional), optional_len, total_len, reinterpret_cast<DWORD_PTR>(this))) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    if (total_len <= optional_len) {
      return RecieveResponse();
    }
    return S_OK;
  }

  HRESULT WriteData(const void* data, DWORD length) {
    DWORD write;
    if (FALSE == ::WinHttpWriteData(handle_, data, length, &write)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return RecieveResponse();
  }

protected:

  virtual HRESULT OnContentLength(DWORD length) {
    return S_OK;
  }

  virtual HRESULT OnReadComplete(const void* info, DWORD length) {
    return S_OK;
  }

  virtual HRESULT OnResponseComplete(HRESULT hr) {
    return S_OK;
  }

private:  

  HRESULT QueryDataAvailable(void) {
    if (FALSE == ::WinHttpQueryDataAvailable(handle_, &avaiable_size_)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return S_OK;
  }

  HRESULT RecieveResponse(void) {
    HRESULT hr;
    if (FALSE == ::WinHttpReceiveResponse(handle_, NULL)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    DWORD status;
    DWORD len = sizeof (status);
    if (FALSE == ::WinHttpQueryHeaders(handle_, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &status, &len, WINHTTP_NO_HEADER_INDEX)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    if (status != HTTP_STATUS_OK) {
      return E_FAIL;
    }
    DWORD content_length;
    len = sizeof (content_length);
    if (TRUE == ::WinHttpQueryHeaders(handle_, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &content_length, &len, WINHTTP_NO_HEADER_INDEX)) {  
      OnContentLength(content_length);
    }
    while (true) {
      hr = QueryDataAvailable();
      if (FAILED(hr) || avaiable_size_ <= 0) {
        break;
      }
      DWORD read;
      if (FALSE == ::WinHttpReadData(handle_, buffer_, kBufferLength, &read)) {
        return HRESULT_FROM_WIN32(::GetLastError());
      }
      OnReadComplete(buffer_, read);
    }
    return OnResponseComplete(S_OK);
  }

  WinHttpHandle handle_;
  void* buffer_;
  bool data_written_;
  DWORD avaiable_size_;

  enum {
    kBufferLength = 8 * 1024,
  };

}; //class SyncWinHttpRequest
} //namespace ult
#endif