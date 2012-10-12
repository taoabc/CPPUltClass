/*
** Í¬²½ÏÂÔØÆ÷
** winhttp
** author
**   taoabc@gmail.com
*/
#ifndef ULT_NET_SYNCWINHTTPDOWNLOADER_H_
#define ULT_NET_SYNCWINHTTPDOWNLOADER_H_

#include "./winhttp-downloader-status.h"
#include "./sync-winhttp-request.h"
#include "../simple-buffer.h"
#include "../file-io.h"
#include "../file-dir.h"

#include <string>

namespace ult {

class SyncWinHttpDownloader : public ult::SyncWinHttpRequest {

public:

  SyncWinHttpDownloader(void) {
  }

  ~SyncWinHttpDownloader(void) {
  }

  int DownloadString(const std::wstring& url, ult::SimpleBuffer* sbuffer, unsigned connect_timeo = 0) {
    Reset();
    connect_timeo_ = connect_timeo;
    int ret = InitRequest(url);
    if (ret != 0) {
      return ret;
    }
    buffer_ = sbuffer;
    HRESULT hr = RecieveResponse();
    if (FAILED(hr)) {
      return HttpStatus::kDownloadError;
    }
    if (content_length_ != 0 && content_length_ != buffer_->Size()) {
      return HttpStatus::kContentUncomplete;
    }
    return HttpStatus::kSuccess;
  }

  int DownloadFile(const std::wstring& url, const std::wstring& file, unsigned connect_timeo = 0) {
    Reset();
    connect_timeo_ = connect_timeo;
    int ret = InitRequest(url);
    if (ret != 0) {
      return ret;
    }
    std::wstring file_folder;
    ult::GetUpperPath(file, &file_folder);
    ult::MakeSureFolderExist(file_folder);
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

  void Close(void) {
    session_.Close();
    connection_.Close();
    ult::SyncWinHttpRequest::Close();
  }

private:

  HRESULT OnContentLength(DWORD length) {
    content_length_ = length;
    return S_OK;
  }

  HRESULT OnReadComplete(const void* info, DWORD length) {
    if (buffer_ != NULL) {
      if (!buffer_->Append(info, length)) {
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

  int InitRequest(const std::wstring& url) {
    HRESULT hr = session_.Initialize();
    if (FAILED(hr)) {
      return HttpStatus::kUnknownError;
    }
    URL_COMPONENTS uc;
    UltWinHttpCrackUrl(url.c_str(), &uc);
    std::wstring host_name(uc.lpszHostName, uc.dwHostNameLength);
    if (connect_timeo_ > 0) {
      session_.SetOption(WINHTTP_OPTION_CONNECT_TIMEOUT, &connect_timeo_, sizeof (connect_timeo_));
    }
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
    buffer_ = NULL;
    file_ = NULL;
    content_length_ = 0;
    session_.Close();
    connection_.Close();
    this->Close();
  }

  ult::WinHttpConnection connection_;
  ult::WinHttpSession session_;

  DWORD content_length_;
  unsigned connect_timeo_;

  ult::SimpleBuffer* buffer_;
  ult::File* file_;
}; //class SyncWinHttpDownloader
} //namespace ult

#endif