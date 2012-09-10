/* 
** http封装类
** 使用微软库WinINet
** author
**   taoabc@gmail.com
*/
#ifndef ULT_HTTP_H_
#define ULT_HTTP_H_

#include "number.h"
#include "file-io.h"
#include <string>
#include <windows.h>
#include <Wininet.h>

#pragma comment(lib, "Wininet.lib")

namespace ult {

namespace HttpStatus {
  enum {
    kSuccess           = 0,
    kConnecting        = 1,
    kDownloading       = 2,
    kConnectFailure    = 3,
    kDownloadFailure   = 4,
    kCreateFileFailure = 5,
    kWriteFileFailure  = 6,
  };
}
  
typedef void (*PHttpStringHandle)(
    int status,
    int progress,
    const std::string& content);
typedef void (*PHttpFileHandle)(
    int status,
    int progress,
    const std::wstring& file_path);

class Http {

public:

  static bool CanonicalizeUrl(const std::wstring& src, std::wstring* dest) {
    wchar_t ch;
    DWORD len = 1;
    bool ret = (0 != InternetCanonicalizeUrl(src.c_str(), &ch, &len, ICU_BROWSER_MODE));
    if (!ret) {
      wchar_t* buf = new wchar_t[len];
      ret = (0 != InternetCanonicalizeUrl(src.c_str(), buf, &len, ICU_BROWSER_MODE));
      dest->assign(buf, len);
      delete[] buf;
    }
    return ret;
  }

  Http(void) :
      handle_opened_(false),
      HttpStringHandle_(NULL),
      HttpFileHandle_(NULL) {
  }
  ~Http(void) {
    CloseHandles();
  }

  bool DownloadString(const std::wstring& url, PHttpStringHandle HttpStringHandle) {
    HttpStringHandle_ = HttpStringHandle;
    CallStringHandle(HttpStatus::kConnecting, -1, "");
    OpenHandles(url);
    DWORD status;
    DWORD contentlen;
    QueryInfoNumber(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status);
    if (status >= 400) {
      CallStringHandle(HttpStatus::kConnectFailure, -1, "");
      return false;
    }
    if (!QueryInfoNumber(HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &contentlen)) {
      contentlen = 0;
    }
    DWORD dwsize = 0;
    DWORD dwread = 0;
    char* buf = NULL;
    std::string str;
    int progress;
    while (true) {
      if (0 == InternetQueryDataAvailable(hopenurl_, &dwsize, 0, 0)) {
        break;
      }
      if (dwsize <= 0) {
        break;
      }
      buf = new char[dwsize];
      if (InternetReadFile(hopenurl_, buf, dwsize, &dwread)) {
        str.append(buf, dwread);
        DWORD dl = str.length();
        if (contentlen > 0) {
          progress = static_cast<int>(ult::UIntMultDiv(str.length(), 100, contentlen));
        } else {
          progress = 0;
        }
        CallStringHandle(HttpStatus::kDownloading, progress, "");
      }
      delete[] buf;
    } //while
    if (contentlen > 0) {
      if (str.length() == contentlen) {
        CallStringHandle(HttpStatus::kSuccess, progress, str);
      } else {
        CallStringHandle(HttpStatus::kDownloadFailure, progress, str);
        return false;
      }
    } else {
      CallStringHandle(HttpStatus::kSuccess, 100, str);
    }
    return true;
  }

  bool DownloadFile(const std::wstring& url, const std::wstring& file_path, PHttpFileHandle HttpFileHandle) {
    HttpFileHandle_ = HttpFileHandle;
    CallFileHandle(HttpStatus::kConnecting, -1, L"");
    OpenHandles(url);
    DWORD status;
    DWORD contentlen;
    QueryInfoNumber(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status);
    if (status >= 400) {
      CallFileHandle(HttpStatus::kConnectFailure, -1, L"");
      return false;
    } 
    ult::File down_file;
    if (!down_file.Create(file_path, true)) {
      CallFileHandle(HttpStatus::kCreateFileFailure, -1, file_path);
      return false;
    }
    if (!QueryInfoNumber(HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &contentlen)) {
      contentlen = 0;
    }
    DWORD dwsize = 0;
    DWORD dwread = 0;
    DWORD readtotal = 0;
    DWORD dwwrite = 0;
    char* buf = NULL;
    int progress = -1;
    bool result = true;
    while (true) {
      if (0 == InternetQueryDataAvailable(hopenurl_, &dwsize, 0, 0)) {
        break;
      }
      if (dwsize <= 0) {
        break;
      }
      buf = new char[dwsize];
      if (InternetReadFile(hopenurl_, buf, dwsize, &dwread)) {
        if (!down_file.Write(buf, dwread, &dwwrite)) {
          CallFileHandle(HttpStatus::kWriteFileFailure, progress, file_path);
          return false;
        }
        if (contentlen > 0) {
          progress = static_cast<int>(ult::UIntMultDiv(down_file.GetSize(), 100, contentlen));
        } else {
          progress = 0;
        }
        CallFileHandle(HttpStatus::kDownloading, progress, file_path);
      }
      delete[] buf;
    } //while
    if (contentlen > 0) {
      if (down_file.GetSize() == contentlen) {
        CallFileHandle(HttpStatus::kSuccess, progress, file_path);
      } else {
        CallFileHandle(HttpStatus::kDownloadFailure, progress, file_path);
        return false;
      }
    } else {
      CallFileHandle(HttpStatus::kSuccess, 100, file_path);
    }
    return true;
  }

private:

  //private functions
  bool OpenHandles(const std::wstring& url) {
    hopen_ = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hopen_ != NULL) {
      std::wstring curl;
      CanonicalizeUrl(url, &curl);
      hopenurl_ = InternetOpenUrl(hopen_, curl.c_str(), NULL, 0,
          INTERNET_FLAG_NO_UI | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    }

    if (hopenurl_ != NULL) {
      handle_opened_ = true;
    }

    return handle_opened_;
  }

  bool CloseHandles(void) {
    bool ret1 = true;
    bool ret2 = true;
    if (hopen_ != NULL) {
      ret1 = (0 != InternetCloseHandle(hopen_));
    }
    if (hopenurl_ != NULL) {
      ret2 = (0 != InternetCloseHandle(hopenurl_));
    }
    handle_opened_ = false;
    return ret1 && ret2;
  }

  bool QueryInfoNumber(DWORD flags, DWORD* num) {
    if (!(flags & HTTP_QUERY_FLAG_NUMBER)) {
      return false;
    }
    DWORD size;
    return 0 != HttpQueryInfo(hopenurl_, flags, num, &size, NULL);
  }

  bool QueryInfoString(DWORD flags, std::wstring* str) {
    if ((flags & HTTP_QUERY_FLAG_NUMBER) ||
        (flags & HTTP_QUERY_FLAG_REQUEST_HEADERS) ||
        (flags & HTTP_QUERY_FLAG_SYSTEMTIME)) {
      return false;
    }
    bool result = true;
    void* buf = NULL;
    DWORD size = 0;
    while (true) {
      if(0 != HttpQueryInfo(hopenurl_, flags, (LPVOID)buf, &size, NULL)) {
        break;
      } else {
        if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND) {
          // Code to handle the case where the header isn't available.
          break;
        } else {
          // Check for an insufficient buffer.
          if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            // Allocate the necessary buffer.
            buf = new char[size];
            // Retry the call.
            continue;
          } else {
            // Error handling code.
            if (buf != NULL) {
              delete[] buf;
            }
            result = false;
            break;
          }
        }
      } //if query fail
    } //while

    if (result) {
      str->assign((wchar_t*)buf, size / sizeof (wchar_t));
    }
    return result;
  }

  void CallStringHandle(int status, int progress, const std::string& content) {
    if (HttpStringHandle_ != NULL) {
      HttpStringHandle_(status, progress, content);
    }
  }

  void CallFileHandle(int status, int progress, const std::wstring& file_path) {
    if (HttpFileHandle_ != NULL) {
      HttpFileHandle_(status, progress, file_path);
    }
  }

  //private variable
  HINTERNET hopen_;
  HINTERNET hopenurl_;
  bool handle_opened_;
  PHttpStringHandle HttpStringHandle_;
  PHttpFileHandle HttpFileHandle_;

}; // class Http

} // namespace ult

#endif