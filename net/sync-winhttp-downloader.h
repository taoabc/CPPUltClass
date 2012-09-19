/*
** Í¬²½ÏÂÔØÆ÷
** winhttp
** author
**   taoabc@gmail.com
*/
#ifndef ULT_NET_SYNCWINHTTPDOWNLOADER_H_
#define ULT_NET_SYNCWINHTTPDOWNLOADER_H_

#include "./sync-winhttp-request.h"
#include "../simple-buffer.h"
#include "../file-io.h"

#include <string>

namespace ult {

namespace HttpStatus {
enum {
  kContentUncomplete   = -7,
  kWriteFileError      = -6,
  kCreateFileError     = -5,
  kDownloadError       = -4,
  kSendRequestError    = -3,
  kConnectError        = -2,
  kUnknownError        = -1,
  kSuccess             = 0,
  kConnecting          = 1,
  kReceiveing          = 2,
  kDownloading         = 3,
};
}

class SyncWinHttpDownloader : public SyncWinHttpRequest {

public:

  SyncWinHttpDownloader(void) :
      content_length_(0) {
  }

  ~SyncWinHttpDownloader(void) {
  }

  int DownloadString(const wchar_t* url, ult::SimpleBuffer* sbuffer) {
    Reset();
    int ret = InitRequest(url);
    if (ret != 0) {
      return ret;
    }
    sbuffer_ = sbuffer;
    HRESULT hr = RecieveResponse();
    if (FAILED(hr)) {
      return HttpStatus::kDownloadError;
    }
    if (content_length_ != 0 && content_length_ != sbuffer_->Size()) {
      return HttpStatus::kContentUncomplete;
    }
    return HttpStatus::kSuccess;
  }

  int DownloadFile(const wchar_t* url, const wchar_t* file) {
    Reset();
    int ret = InitRequest(url);
    if (ret != 0) {
      return ret;
    }
    ult::File down_file;
    if (!down_file.Create(file, true)) {
      return HttpStatus::kCreateFileError;
    }
    file_ = &down_file;
    HRESULT hr = RecieveResponse();
    if (FAILED(hr)) {
      return HttpStatus::kDownloadError;
    }
    if (content_length_ != 0 && content_length_ != file_->GetSize()) {
      return HttpStatus::kContentUncomplete;
    }
    return HttpStatus::kSuccess;
  }

private:

  HRESULT OnContentLength(DWORD length) {
    content_length_ = length;
    return S_OK;
  }

  HRESULT OnReadComplete(const void* info, DWORD length) {
    if (sbuffer_ != NULL) {
      if (!sbuffer_->Append(info, length)) {
        return E_FAIL;
      }
    }
    if (file_ != NULL) {
      DWORD written;
      if (!file_->Write(info, length, &written)) {
        return E_FAIL;
      }
    }
    return S_OK;
  }

  int InitRequest(const wchar_t* url) {
    HRESULT hr = session_.Initialize();
    if (FAILED(hr)) {
      return HttpStatus::kUnknownError;
    }
    URL_COMPONENTS uc;
    UltWinHttpCrackUrl(url, &uc);
    std::wstring host_name(uc.lpszHostName, uc.dwHostNameLength);
    hr = connection_.Initialize(session_.GetHandle(), host_name.c_str(), uc.nPort);
    if (FAILED(hr)) {
      return HttpStatus::kConnectError;
    }
    hr = Initialize(connection_.GetHandle(), L"GET", uc.lpszUrlPath, uc.nScheme);
    if (FAILED(hr)) {
      return HttpStatus::kSendRequestError;
    }
    SendRequest(NULL, 0, NULL, 0, 0);
    return HttpStatus::kSuccess;
  }

  void Reset(void) {
    sbuffer_ = NULL;
    file_ = NULL;
    content_length_ = 0;
    session_.CloseHandle();
    connection_.CloseHandle();
    CloseHandle();
  }

  ult::WinHttpConnection connection_;
  ult::WinHttpSession session_;

  DWORD content_length_;

  ult::SimpleBuffer* sbuffer_;
  ult::File* file_;
}; //class SyncWinHttpDownloader
} //namespace ult

#endif