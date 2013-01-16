/*
** 异步Winhttp下载
** 使用async-winhttp-request使用支撑层
** author
**   taoabc@gmail.com
*/

#ifndef ULT_NET_ASYNCWINHTTPDOWNLOADER_H_
#define ULT_NET_ASYNCWINHTTPDOWNLOADER_H_

#include "./async-winhttp-request.h"
#include "../simple-buffer.h"
#include "../file-io.h"
#include "../file-dir.h"
#include <basetyps.h>
#include <Unknwn.h>

#include <string>

namespace ult {

namespace base {

interface IAsyncHttpEvent {
  STDMETHOD(SetTotal)(ULONGLONG total) PURE;
  STDMETHOD(SetCompleted)(ULONGLONG completed) PURE;
};

class AsyncHttpBase : public AsyncWinHttpRequest {

public:

  AsyncHttpBase(void) {

  }

  virtual ~AsyncHttpBase(void) {
    this->Close();
    connection_.Close();
    session_.Close();
  }

protected:

  HRESULT InitRequest(const std::wstring& url) {
    HRESULT hr = session_.Initialize(true);
    URL_COMPONENTS uc;
    UltWinHttpCrackUrl(url.c_str(), &uc);
    std::wstring host_name(uc.lpszHostName, uc.dwHostNameLength);
    RETURN_IF_FAILED(connection_.Initialize(session_.GetHandle(), host_name.c_str(), uc.nPort));
    RETURN_IF_FAILED(Initialize(connection_.GetHandle(), L"GET", uc.lpszUrlPath, uc.nScheme));
    RETURN_IF_FAILED(SendRequest(NULL, 0, NULL, 0, 0));
    return S_OK;
  }

private:

  HRESULT OnSendRequestComplete(void) {
    return RecieveResponse();
  }

  WinHttpSession session_;
  WinHttpConnection connection_;
};

} //namespace base

interface IAsyncHttpBufferEvent : base::IAsyncHttpEvent {
  STDMETHOD(SetResult)(LPVOID buffer, ULONGLONG length) PURE;
};

interface IAsyncHttpFileEvent : base::IAsyncHttpEvent {
  STDMETHOD(SetResult)(LPCWSTR file) PURE;
};

class AsyncHttpBuffer : public base::AsyncHttpBase {

public:

  HRESULT Request(const std::wstring& url, IAsyncHttpBufferEvent* callback) {
    callback_ = callback;
    return InitRequest(url);
  }

private:

  HRESULT OnContentLength(DWORD length) {
    return callback_->SetTotal(length);
  }

  HRESULT OnReadComplete(const void* info, DWORD length) {
    buffer_.Append(info, length);
    return callback_->SetCompleted(buffer_.Size());
  }

  HRESULT OnResponseComplete(HRESULT hr) {
    if (FAILED(hr)) {
      return callback_->SetResult(NULL, 0);
    }
    return callback_->SetResult(buffer_.Data(), buffer_.Size());
  }

  ult::SimpleBuffer buffer_;
  IAsyncHttpBufferEvent* callback_;
};

class AsyncHttpFile : public base::AsyncHttpBase {

public:

  HRESULT Request(const std::wstring& url, const std::wstring& file, IAsyncHttpFileEvent* callback) {
    file_path_ = file;
    callback_ = callback;
    std::wstring file_folder(ult::GetUpperDirectory(file));
    ult::MakeSureFolderExist(file_folder);
    if (!file_.Create(file, true)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    return InitRequest(url);
  }

private:

  HRESULT OnContentLength(DWORD length) {
    return callback_->SetTotal(length);
  }

  HRESULT OnReadComplete(const void* info, DWORD length) {
    DWORD written;
    file_.Write(info, length, &written);
    return callback_->SetCompleted(file_.GetSize());
  }

  HRESULT OnResponseComplete(HRESULT hr) {
    if (FAILED(hr)) {
      return callback_->SetResult(NULL);
    }
    return callback_->SetResult(file_path_.c_str());
  }

  ult::File file_;
  std::wstring file_path_;
  IAsyncHttpFileEvent* callback_;
};

} //namespace ult

#endif