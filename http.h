/* 
** http封装类
** 使用微软库WinINet
** author
**   taoabc@gmail.com
*/
#ifndef ULT_HTTP_H_
#define ULT_HTTP_H_

#include <string>
#include <windows.h>
#include <Wininet.h>

#pragma comment(lib, "Wininet.lib")

namespace ult {
  
typedef void (*PHttpStringHandle)(
    int status,
    int progress,
    const std::string& content);
typedef void (*PHttpFileHandle)(
    int status,
    int progress,
    const std::wstring& filename);

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
      handle_opened_(false) {
  }
  ~Http(void) {
    CloseHandles();
  }

  bool DownloadString(const std::wstring& url, PHttpStringHandle HttpStringHandle) {
    OpenHandles(url);
    ReadData(NULL);
    DWORD status;
    DWORD size;
    QueryInfoNumber(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &size);
    return false;
  }

  bool DownloadFile(const std::wstring& url, PHttpFileHandle HttpfileHandle) {
    return false;
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

  bool QueryInfoNumber(DWORD flags, DWORD* num, DWORD* size) {
    if (!(flags | HTTP_QUERY_FLAG_NUMBER)) {
      return false;
    }
    *size = sizeof (*num);
    return 0 != HttpQueryInfo(hopenurl_, flags, num, size, NULL);
  }

  bool QueryInfoString(DWORD flags, void** buffer, DWORD* len) {
    if ((flags | HTTP_QUERY_FLAG_NUMBER) ||
        (flags | HTTP_QUERY_FLAG_REQUEST_HEADERS) ||
        (flags | HTTP_QUERY_FLAG_SYSTEMTIME)) {
      return false;
    }
    bool result = true;
    void* buf = NULL;
    DWORD size = 0;
    while (true) {
      if(0 == HttpQueryInfo(hopenurl_, flags, (LPVOID)buf, &size, NULL)) {
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
      *buffer = buf;
      *len = size;
    }
    return result;
  }

  bool ReadData(void (*Http::ContentHandle)(void* buffer, DWORD len)) {
    DWORD dwsize = 0;
    DWORD dwread = 0;
    char* buf = NULL;
    while (true) {
      if (0 == InternetQueryDataAvailable(hopenurl_, &dwsize, 0, 0)) {
        break;
      }
      if (dwsize <= 0) {
        break;
      }
      buf = new char[dwsize];
      if (InternetReadFile(hopenurl_, buf, dwsize, &dwread)) {
        if (ContentHandle != NULL) {
          ContentHandle(buf, dwread);
        }
      }
      delete[] buf;
    }
    return true;
  }

  //private variable
  HINTERNET hopen_;
  HINTERNET hopenurl_;
  bool handle_opened_;

};

namespace http {

inline bool InitInternetHandle(const std::wstring& url,
                               HINTERNET* phopen,
                               HINTERNET* phopenurl) {
  bool init_result = false;
  *phopen = NULL;
  *phopenurl = NULL;

  HINTERNET hopen = NULL;
  HINTERNET hopenurl = NULL;

  hopen = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
  if (hopen != NULL) {
    *phopen = hopen;
    DWORD len =5;
    wchar_t ch;
    wchar_t *buf = NULL;
    InternetCanonicalizeUrl(url.c_str(), &ch, &len, ICU_BROWSER_MODE);
    buf = new wchar_t[len];
    InternetCanonicalizeUrl(url.c_str(), buf, &len, ICU_BROWSER_MODE);
    hopenurl = InternetOpenUrl(hopen, buf, NULL, 0,
      INTERNET_FLAG_NO_UI | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    delete[] buf;
  }

  if (hopenurl != NULL) {
    *phopenurl = hopenurl;
    init_result = true;
  }

  return init_result;
}

} //namespace http

inline bool HttpDownloadString(const std::wstring& url, std::string* content) {
  HINTERNET hopen = NULL;
  HINTERNET hopenurl = NULL;
  DWORD dwsize = 0;
  DWORD dwdownloaded = 0;
  char* buf = NULL;
  bool result = false;

  http::InitInternetHandle(url, &hopen, &hopenurl);
  void* buffer = NULL;
  DWORD buf_size = 0;
  while (true) {
    DWORD status;
    DWORD l = sizeof (status);
    HttpQueryInfo(hopenurl, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
      &status, &l, NULL);
    if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND) {
      // Code to handle the case where the header isn't available.
      //return true;
      break;
    } else {
      // Check for an insufficient buffer.
      if (GetLastError()==ERROR_INSUFFICIENT_BUFFER) {
        // Allocate the necessary buffer.
        buffer = new char[buf_size];

        // Retry the call.
        continue;
      } else {
        // Error handling code.
        break;
      }
    }
  } //while

  if (buffer != NULL) {
    delete[] buffer;
  }

  if (hopenurl != NULL) {
    content->clear();
    do {
      dwsize = 0;
      if (!InternetQueryDataAvailable(hopenurl, &dwsize, 0, 0)) {
        break;
      }
      buf = new char[dwsize+1];
      memset(buf, 0, dwsize+1);
      if (InternetReadFile(hopenurl, buf, dwsize, &dwdownloaded)) {
        content->append(buf, dwdownloaded);
        result = true;
      }
      delete[] buf;
    } while (dwsize > 0);
  }
  if (hopen != NULL) {
    InternetCloseHandle(hopen);
  }
  if (hopenurl != NULL) {
    InternetCloseHandle(hopenurl);
  }
  return result;
}

inline bool HttpDownloadFile(const std::wstring& url, const std::wstring& file_name) {
  HINTERNET hopen = NULL;
  HINTERNET hopenurl = NULL;
  HANDLE hfile = NULL;
  DWORD dwsize = 0;
  DWORD dwdownloaded = 0;
  DWORD dwwrited = 0;
  char* buf = NULL;
  bool result = false;

  http::InitInternetHandle(url, &hopen, &hopenurl);

  if (hopenurl != NULL) {
    hfile = CreateFile(file_name.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    do {
      if (hfile == INVALID_HANDLE_VALUE) {
        break;
      }
      dwsize = 0;
      if (!InternetQueryDataAvailable(hopenurl, &dwsize, 0, 0)) {
        break;
      }
      buf = new char[dwsize+1];
      memset(buf, 0, dwsize+1);
      if (InternetReadFile(hopenurl, buf, dwsize, &dwdownloaded)) {
        if (0 == WriteFile(hfile, buf, dwdownloaded, &dwwrited, NULL)) {
          result = false;
          break;
        }
        result = true;
      }
      delete[] buf;
    } while (dwsize > 0);
  }
  if (hopen != NULL) {
    InternetCloseHandle(hopen);
  }
  if (hopenurl != NULL) {
    InternetCloseHandle(hopenurl);
  }
  if (hfile != NULL) {
    CloseHandle(hfile);
  }
  return result;
}

} // namespace ult

#endif