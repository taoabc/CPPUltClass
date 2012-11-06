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
#include "../file-io.h"

namespace ult {

class AsyncWinHttpUploader : public ult::AsyncWinHttpRequest {

public:

  AsyncWinHttpUploader(void) :
      buffer1_(NULL),
      buffer2_(NULL),
      buffer_ready_(1) {
    InitializeCriticalSection(&cs_);
  }

  ~AsyncWinHttpUploader(void) {
    DeleteCriticalSection(&cs_);
  }

  int Init(void) {
    if (buffer1_ == NULL) {
      buffer1_ = new char[kBufferSize_];
    }
    if (buffer2_ == NULL) {
      buffer2_ = new char[kBufferSize_];
    }
    return Reset();
  }
  
  int Reset(void) {
    sendfield_.clear();
    //close
    file_.Close();
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
    header = L"Content-Type: multipart/form-data; boundary=" + wboundary_;
    
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

  int PostFileToUrl(const std::wstring& url, const std::wstring& file, const std::wstring& field = L"file") {
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
    file_.Open(file);
    unsigned __int64 file_size = file_.GetSize();
    if (file_size > 3.5 * 1024 * 1024 * 1024) {
      return ult::HttpStatus::kUnknownError;
    }
    size_t l1 = sendfield_.length();
    size_t l2 = post_begin_.length();
    size_t l3 = post_end_.length();
    DWORD total_size = (DWORD)file_size + sendfield_.length() + post_begin_.length() + post_end_.length();
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

  HRESULT OnWriteDataComplete(void) {
    switch (flag_) {
    case StepFlag::SendBegin:
      WriteData(post_begin_.c_str(), post_begin_.length());
      flag_ = StepFlag::SendContent;
      //read file to buffer
      FileToBuffer();
      break;
    case StepFlag::SendContent:
      {
        void* buffer;
        DWORD len;
        GetBuffer(&buffer, &len);
        if (len != kBufferSize_) {
          int a = 0;
        }
        if (len > 0) {
          WriteData(buffer, len);
        // just send data over
        } else if (len == 0) {  
          file_.Close();
          WriteData(post_end_.c_str(), post_end_.length());
          flag_ = StepFlag::Over;
        }
        if (len == kBufferSize_) {
          flag_ = StepFlag::SendContent;
          FileToBuffer();
        } else if (len < kBufferSize_) {
          flag_ = StepFlag::SendEnd;
        }
      }
      break;
    case StepFlag::SendEnd:
      file_.Close();
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

  bool FileToBuffer(void) {
    bool ret = true;
    EnterCriticalSection(&cs_);
    if (buffer_ready_ == 1) {
      file_.Read(buffer2_, kBufferSize_, &readed2_);
      buffer_ready_ = 2;
    } else if (buffer_ready_ == 2) {
      file_.Read(buffer1_, kBufferSize_, &readed1_);
      buffer_ready_ = 1;
    } else {
      ret = false;
    }
    LeaveCriticalSection(&cs_);
    return ret;
  }

  bool GetBuffer(void** p, DWORD* len) {
    if (buffer_ready_ == 1) {
      *p = buffer1_;
      *len = readed1_;
    } else if (buffer_ready_ == 2) {
      *p = buffer2_;
      *len = readed2_;
    } else {
      return false;
    }
    return true;
  }

  ult::WinHttpSession session_;
  ult::WinHttpConnection connection_;

  std::string sendfield_;
  std::string post_begin_;
  std::string post_end_;
  ult::File file_;
  DWORD readed1_;
  DWORD readed2_;

  std::string aboundary_;
  std::wstring wboundary_;
  char* buffer1_;
  char* buffer2_;
  int buffer_ready_;

  enum class StepFlag {
    SendField,
    SendBegin,
    SendContent,
    SendEnd,
    Over,
  } flag_;
  CRITICAL_SECTION cs_;
  static const std::string kLineEnd_;
  static const size_t kBufferSize_;
}; //class AsyncWinHttpUploader

__declspec(selectany) const std::string AsyncWinHttpUploader::kLineEnd_ = "\r\n";
__declspec(selectany) const size_t AsyncWinHttpUploader::kBufferSize_ = 128 * 1024;
} //namespace ult

#endif