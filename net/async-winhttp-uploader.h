/*
** winhttp…œ¥´¿‡
** author
**   taoabc@gmail.com
*/

#ifndef ULT_NET_ASYNCWINHTTPUPLOADER_H_
#define ULT_NET_ASYNCWINHTTPUPLOADER_H_

#include "./winhttp-downloader-status.h"
#include "./async-winhttp-request.h"
#include "../string-operate.h"

namespace ult {

class AsyncWinHttpUploader : public ult::AsyncWinHttpRequest {

public:

  AsyncWinHttpUploader(void) {
    wboundary_ = L"----------";
    wboundary_ += ult::GetRandomString(L"", 30);
    aboundary_ = ult::UnicodeToAnsi(wboundary_);
  }

  ~AsyncWinHttpUploader(void) {
  }

  void Test(const std::wstring& url) {
    InitRequest(url);
    std::wstring header;
    header = L"Content-Type: multipart/form-data; boundary=" + wboundary_;
    
    SendRequest(header.c_str(), -1L, WINHTTP_NO_REQUEST_DATA, 0, tosend_.length(), true);
    //SendRequest(NULL, 0, NULL, 0, 0);
  }

private:

  HRESULT OnReadComplete(const void* info, DWORD length) {
    std::string content((char*)info, length);
    printf("%s\n", content.c_str());
    return S_OK;
  }

  HRESULT OnSendRequestComplete(void) {
    tosend_ = "--" + aboundary_ + kLineEnd_ + "Content-Disposition: form-data; name=\"file\"; filename=\"Capture.PNG\"" + kLineEnd_;
    tosend_ += "Content-Type: image/png" + kLineEnd_ + kLineEnd_;
    tosend_ += "asdfasdfas" + kLineEnd_;
    tosend_ += "--" + aboundary_ + "--" + kLineEnd_;
    WriteData(tosend_.c_str(), tosend_.length());
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
    hr = Initialize(connection_.GetHandle(), L"POST", uc.lpszUrlPath, uc.nScheme);
    if (FAILED(hr)) {
      return ult::HttpStatus::kOpenRequestError;
    }
    return ult::HttpStatus::kSuccess;
  }

  ult::WinHttpSession session_;
  ult::WinHttpConnection connection_;

  std::string tosend_;

  std::string aboundary_;
  std::wstring wboundary_;
  static const std::string kLineEnd_;
}; //class AsyncWinHttpUploader

__declspec(selectany) const std::string AsyncWinHttpUploader::kLineEnd_ = "\r\n";
} //namespace ult

#endif