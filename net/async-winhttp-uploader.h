/*
** winhttp单文件上传类
** author
**   taoabc@gmail.com
*/

#ifndef ULT_NET_ASYNCWINHTTPUPLOADER_H_
#define ULT_NET_ASYNCWINHTTPUPLOADER_H_

#include "./winhttp-downloader-status.h"
#include "./async-winhttp-request.h"
#include "../string-operate.h"
#include "../file-dir.h"
#include "../file-map.h"

namespace ult {

struct IAsyncWinHttpUploaderEvent {
  virtual void SetTotal(DWORD total) = 0;
  virtual void SetCompleted(DWORD complated) = 0;
  virtual void SetStatus(DWORD status) = 0;
};

class AsyncWinHttpUploader : public ult::AsyncWinHttpRequest {

public:

  AsyncWinHttpUploader(void) {
  }

  ~AsyncWinHttpUploader(void) {
  }

  int Init(IAsyncWinHttpUploaderEvent* callback = NULL, DWORD chunk_size = 128 * 1024) {
    callback_ = callback;
    SetChunkSize(chunk_size);
    return Reset();
  }

  DWORD SetChunkSize(DWORD chunk_size) {
    DWORD glt = GetGranularity();
    chunk_size = chunk_size < 0x80000000 ? chunk_size : 0x80000000;
    chunk_size_ = (chunk_size + glt-1) - (chunk_size + glt-1) % glt;
    return chunk_size_;
  }
  
  int Reset(void) {
    sendfield_.clear();
    //close
    file_map_.Close();
    session_.Close();
    connection_.Close();
    this->Close();
    //init boundary
    wboundary_ = L"----------";
    wboundary_ += ult::GetRandomString(L"", 30);
    aboundary_ = ult::UnicodeToAnsi(wboundary_);
    //return
    return ult::HttpStatus::kSuccess;
  }

  void Test(const std::wstring& url) {
    InitRequest(url);
    std::wstring header;
    header = L"Content-Type: application/x-www-form-urlencoded";
    
    sendfield_ = "--" + aboundary_ + kLineEnd_ + "Content-Disposition: form-data; name=\"file\"; filename=\"Capture.PNG\"" + kLineEnd_;
    sendfield_ += "Content-Type: application/octet-stream" + kLineEnd_ + kLineEnd_;
    sendfield_ += "asdfasdfas" + kLineEnd_;
    sendfield_ += "--" + aboundary_ + "--" + kLineEnd_;
    SendRequest(header.c_str(), -1L, WINHTTP_NO_REQUEST_DATA, 0, sendfield_.length());
  }

  void AddField(const std::wstring& field, const char* data, DWORD len) {
    sendfield_ += "--" + aboundary_ + kLineEnd_ + "Content-Disposition: form-data; name=\"";
    sendfield_ += ult::UnicodeToAnsi(field) + "\"" + kLineEnd_ + kLineEnd_;
    sendfield_.append(data, len);
    sendfield_ += kLineEnd_;
  }

  int PostFileToUrl(const std::wstring& url, const std::wstring& file,
      DWORD start_position = 0, const std::wstring& field = L"file") {
    int ret = InitRequest(url);
    if (ret != ult::HttpStatus::kSuccess) {
      return ret;
    }
    std::wstring header = L"Content-Type: multipart/form-data; boundary=" + wboundary_;
    //biuld begin
    post_begin_ = "--" + aboundary_ + kLineEnd_ + "Content-Disposition: form-data; name=\"";
    post_begin_ += ult::UnicodeToAnsi(field) + "\"; filename=\"";
    std::wstring dir, name;
    ult::ToUpperpathAndFilename(file, L"\\", &dir, &name);
    post_begin_ += ult::UnicodeToAnsi(name) + "\"" + kLineEnd_ + "Content-Type: application/octet-stream" + kLineEnd_ + kLineEnd_;
    //biuld end
    post_end_ = kLineEnd_ + "--" + aboundary_ + "--" + kLineEnd_;
    //calculate file size
    file_map_.Open(file);
    unsigned __int64 file_size = file_map_.GetSize();
    if (file_size > 3.5 * 1024 * 1024 * 1024) {
      return ult::HttpStatus::kUnknownError;
    }
    file_size_ = (DWORD)file_size;
    if (start_position >= file_size_) {
      return ult::HttpStatus::kUnknownError;
    }
    DWORD file_size_tosend = file_size_ - start_position;
    file_cursor_ = start_position;
    SetCallbackTotal(file_size_);
    size_t l1 = sendfield_.length();
    size_t l2 = post_begin_.length();
    size_t l3 = post_end_.length();
    DWORD total_size = file_size_tosend + sendfield_.length() + post_begin_.length() + post_end_.length();
    SendRequest(header.c_str(), -1L, NULL, 0, total_size);
    return ult::HttpStatus::kSuccess;
  }

private:

  HRESULT OnReadComplete(const void* info, DWORD length) {
    std::string content((char*)info, length);
    printf("%s\n", content.c_str());
    return S_OK;
  }

  HRESULT OnSendRequestComplete(void) {
    WriteData(sendfield_.c_str(), sendfield_.length());
    flag_ = StepFlag::SendBegin;
    return S_OK;
  }

  HRESULT OnResponseComplete(HRESULT hr) {
    if (SUCCEEDED(hr)) {
      SetCallbackStatus(ult::HttpStatus::kSuccess);
    } else {
      SetCallbackStatus(ult::HttpStatus::kUnknownError);
    }
    return S_OK;
  }

  HRESULT OnWriteDataComplete(void) {
    switch (flag_) {
    case StepFlag::SendBegin:
      WriteData(post_begin_.c_str(), post_begin_.length());
      flag_ = StepFlag::SendContent;
      file_map_.CreateMapping();
      break;
    case StepFlag::SendContent:
      {
        if (file_cursor_ < file_size_) {
          SetCallbackCompleted(file_cursor_);
          DWORD left = file_size_ - file_cursor_;
          DWORD cursor_ahead = file_cursor_ % GetGranularity();
          DWORD map_size;
          if (left > chunk_size_) {
            map_size = chunk_size_;
          } else {
            //the last chunk of file
            map_size = left;
            flag_ = StepFlag::SendEnd;
          }
          map_size += cursor_ahead;
          LPVOID file_view = file_map_.MapView(file_cursor_-cursor_ahead, map_size);
          if (file_view == NULL) {
            SetCallbackStatus(ult::HttpStatus::kReadFileError);
          } else {
            DWORD useful_size = map_size - cursor_ahead;
            WriteData((char*)file_view+cursor_ahead, useful_size);
            file_cursor_ += useful_size;
          }
        }
      }
      break;
    case StepFlag::SendEnd:
      file_map_.Close();
      WriteData(post_end_.c_str(), post_end_.length());
      flag_ = StepFlag::Over;
      break;
    case StepFlag::Over:
      {
        HRESULT hr = RecieveResponse();
        if (FAILED(hr)) {
          return hr;
        }
      }
      break;
    break;
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
    hr = Initialize(connection_.GetHandle(), L"POST", uc.lpszUrlPath, uc.nScheme);
    if (FAILED(hr)) {
      return ult::HttpStatus::kOpenRequestError;
    }
    return ult::HttpStatus::kSuccess;
  }

  void SetCallbackTotal(DWORD total) {
    if (callback_ != NULL) {
      callback_->SetTotal(total);
    }
  }

  void SetCallbackCompleted(DWORD completed) {
    if (callback_ != NULL) {
      callback_->SetCompleted(completed);
    }
  }

  void SetCallbackStatus(DWORD s) {
    if (callback_ != NULL) {
      callback_->SetStatus(s);
    }
  }

  DWORD GetGranularity(void) {
    static DWORD glt = 0;
    if (glt == 0) {
      SYSTEM_INFO sys_info;
      ::GetSystemInfo(&sys_info);
      glt = sys_info.dwAllocationGranularity;
    }
    return glt;
  }

  ult::WinHttpSession session_;
  ult::WinHttpConnection connection_;

  std::string sendfield_;
  std::string post_begin_;
  std::string post_end_;
  ult::FileMap file_map_;

  std::string aboundary_;
  std::wstring wboundary_;
  DWORD file_size_;
  DWORD file_cursor_;

  enum class StepFlag {
    SendField,
    SendBegin,
    SendContent,
    SendEnd,
    Over,
  } flag_;
  DWORD chunk_size_;
  IAsyncWinHttpUploaderEvent* callback_;
  static const std::string kLineEnd_;
}; //class AsyncWinHttpUploader

__declspec(selectany) const std::string AsyncWinHttpUploader::kLineEnd_ = "\r\n";
} //namespace ult

#endif