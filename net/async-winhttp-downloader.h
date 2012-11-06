/*
** 异步Winhttp下载
** 使用async-winhttp-request使用支撑层
** author
**   taoabc@gmail.com
*/

#ifndef ULT_NET_ASYNCWINHTTPDOWNLOADER_H_
#define ULT_NET_ASYNCWINHTTPDOWNLOADER_H_

#include "./winhttp-downloader-status.h"
#include "./async-winhttp-request.h"
#include "../simple-buffer.h"
#include "../file-io.h"
#include "../file-dir.h"

#include <string>

namespace ult {

struct IAsyncWinHttpEvent {
  virtual void StringHandle(
      unsigned id,
      int status,
      unsigned downloaded,
      unsigned total_size,
      void* buffer,
      unsigned len) = 0;
  virtual void FileHandle(
      unsigned id,
      int status,
      unsigned downloaded,
      unsigned total_size,
      const std::wstring& file_path) = 0;
};

class AsyncWinHttpDownloader : public ult::AsyncWinHttpRequest {

public:

  AsyncWinHttpDownloader(void) {
  }

  ~AsyncWinHttpDownloader(void) {
  }

  int DownloadString(unsigned id, const std::wstring& url, ult::IAsyncWinHttpEvent* callback,
      bool self_destroy = false) {
    Reset();
    id_ = id;
    callback_ = callback;
    self_destroy_ = self_destroy;
    dltype_ = kString;
    return InitRequest(url);
  }

  int DownloadFile(unsigned id, const std::wstring& url, const std::wstring& file_path,
      ult::IAsyncWinHttpEvent* callback, bool self_destroy = false) {
    Reset();
    id_ = id;
    file_path_ = file_path;
    callback_ = callback;
    self_destroy_ = self_destroy;
    dltype_ = kFile;
    std::wstring file_folder;
    ult::GetUpperPath(file_path, &file_folder);
    ult::MakeSureFolderExist(file_folder);
    if (!file_.Create(file_path_, true)) {
      return ult::HttpStatus::kCreateFileError;
    }
    return InitRequest(url);
  }

private:

  HRESULT OnSendRequestComplete(void) {
    return RecieveResponse();
  }

  HRESULT OnContentLength(DWORD length) {
    content_length_ = length;
    return S_OK;
  }

  HRESULT OnReadComplete(const void* info, DWORD length) {
    if (dltype_ == kString) {
      buffer_.Append(info, length);
      CallStringHandle(id_, ult::HttpStatus::kDownloading, buffer_.Size(),
          content_length_, NULL, 0);
    } else if (dltype_ == kFile) {
      DWORD written;
      file_.Write(info, length, &written);
      CallFileHandle(id_, ult::HttpStatus::kDownloading, (unsigned)file_.GetSize(),
        content_length_, file_path_);
    }
    return S_OK;
  }

  HRESULT OnResponseComplete(HRESULT hr) {
    if (SUCCEEDED(hr)) {
      if (dltype_ == kString) {
        CallStringHandle(id_, ult::HttpStatus::kSuccess, buffer_.Size(),
            content_length_, buffer_.Detach(), buffer_.Size());
      } else if (dltype_ == kFile) {
        CallFileHandle(id_, ult::HttpStatus::kSuccess, (unsigned)file_.GetSize(),
            content_length_, file_path_);
      }
    } else {
      if (dltype_ == kString) {
        CallStringHandle(id_, ult::HttpStatus::kUnknownError,
            buffer_.Size(), content_length_, NULL, 0);
      } else if (dltype_ == kFile) {
        CallFileHandle(id_, ult::HttpStatus::kUnknownError, (unsigned)file_.GetSize(),
            content_length_, NULL);
      }
    }
    if (dltype_ == kFile) {
      file_.Close();
    }
    if (self_destroy_) {
      delete this;
    }
    return S_OK;
  }

  int InitRequest(const std::wstring& url) {
    HRESULT hr = session_.Initialize(true);
    if (FAILED(hr)) {
      return ult::HttpStatus::kUnknownError;
    }
    URL_COMPONENTS uc;
    ult::UltWinHttpCrackUrl(url.c_str(), &uc);
    std::wstring host_name(uc.lpszHostName, uc.dwHostNameLength);
    hr = connection_.Initialize(session_.GetHandle(), host_name.c_str(), uc.nPort);
    if (FAILED(hr)) {
      return ult::HttpStatus::kConnectError;
    }
    hr = Initialize(connection_.GetHandle(), L"GET", uc.lpszUrlPath, uc.nScheme);
    if (FAILED(hr)) {
      return ult::HttpStatus::kOpenRequestError;
    }
    SendRequest(NULL, 0, NULL, 0, 0);
    return ult::HttpStatus::kSuccess;
  }

  void CallStringHandle(unsigned id, int status, unsigned downloaded,
      unsigned total_size, void* buffer, unsigned len) {
    if (callback_ != NULL) {
      callback_->StringHandle(id, status, downloaded, total_size, buffer, len);
    }
  }

  void CallFileHandle(unsigned id, int status, unsigned downloaded,
      unsigned total_size, const std::wstring& file_path) {
    if (callback_ != NULL) {
      callback_->FileHandle(id, status, downloaded, total_size, file_path);
    }
  }

  void Reset(void) {
    //HINTERNET
    session_.Close();
    connection_.Close();
    this->Close();
    //member
    content_length_ = 0;
    file_.Close();
    buffer_.Free();
  }

  ult::WinHttpSession session_;
  ult::WinHttpConnection connection_;

  DWORD content_length_;
  ult::IAsyncWinHttpEvent* callback_;
  ult::SimpleBuffer buffer_;
  ult::File file_;
  std::wstring file_path_;
  unsigned id_;
  enum {
    kString,
    kFile
  } dltype_;
  bool self_destroy_;
}; // class AsyncWinhttpDownloader
} //namespace ult

#endif