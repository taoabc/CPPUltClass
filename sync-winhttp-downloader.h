/*
** Í¬²½ÏÂÔØÆ÷
** winhttp
** author
**   taoabc@gmail.com
*/
#ifndef ULT_SYNCWINHTTPDOWNLOADER_H_
#define ULT_SYNCWINHTTPDOWNLOADER_H_

#include "./sync-winhttp-request.h"
#include "./simple-buffer.h"
#include "./file-io.h"
#include "./file-dir.h"

#include <string>

namespace ult {

namespace detail {

namespace base {

class SyncHttpBase : public SyncWinHttpRequest {

public:

  SyncHttpBase(void) {

  }

  ~SyncHttpBase(void) {
    Close();
  }

  void Close(void) {
    SyncWinHttpRequest::Close();
    connection_.Close();
    session_.Close();
  }

protected:

  HRESULT InitRequest(const std::wstring& url) {
    RETURN_IF_FAILED(session_.Initialize());
    URL_COMPONENTS uc;
    UltWinHttpCrackUrl(url.c_str(), &uc);
    std::wstring host_name(uc.lpszHostName, uc.dwHostNameLength);
    RETURN_IF_FAILED(connection_.Initialize(session_.GetHandle(), host_name.c_str(), uc.nPort));
    RETURN_IF_FAILED(Initialize(connection_.GetHandle(), L"GET", uc.lpszUrlPath, uc.nScheme));
    RETURN_IF_FAILED(SendRequest(NULL, 0, NULL, 0, 0));
    return S_OK;
  }

private:

  ult::WinHttpConnection connection_;
  ult::WinHttpSession session_;
};

} //namespace base

class SyncHttpString : public base::SyncHttpBase {

public:

  HRESULT operator()(const std::wstring& url, std::string* s) {
    RETURN_IF_FAILED(InitRequest(url));
    s_ = s;
    RETURN_IF_FAILED(RecieveResponse());
    if (content_length_ != 0 && content_length_ != s->length()) {
      return S_FALSE;
    }
    return S_OK;
  }

private:

  HRESULT OnContentLength(DWORD length) {
    content_length_ = length;
    return S_OK;
  }

  HRESULT OnReadComplete(const void* info, DWORD length) {
    s_->append((char*)info, length);
    return S_OK;
  }

  std::string* s_;
  DWORD content_length_;
};

class SyncHttpFile : public base::SyncHttpBase {

public:

  HRESULT operator()(const std::wstring& url, const std::wstring& file) {
    RETURN_IF_FAILED(InitRequest(url));
    std::wstring file_folder(ult::GetUpperDirectory(file));
    if (!ult::MakeSureFolderExist(file_folder)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    ult::File down_file;
    if (!down_file.Create(file, true)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    file_ = &down_file;
    RETURN_IF_FAILED(RecieveResponse());
    if (content_length_ != 0 && content_length_ != down_file.GetSize()) {
      return S_FALSE;
    }
    return S_OK;
  }

private:

  HRESULT OnContentLength(DWORD length) {
    content_length_ = length;
    return S_OK;
  }

  HRESULT OnReadComplete(const void* info, DWORD length) {
    DWORD written;
    if (!file_->Write(info, length, &written)) {
      return E_FAIL;
    }
    return S_OK;
  }

  DWORD content_length_;
  ult::File* file_;
};

} //detail

inline HRESULT HttpDownloadString(const std::wstring& url, std::string* s) {
  return detail::SyncHttpString()(url, s);
}

inline HRESULT HttpDownloadFile(const std::wstring& url, const std::wstring& file) {
  return detail::SyncHttpFile()(url, file);
}

} //namespace ult

#endif