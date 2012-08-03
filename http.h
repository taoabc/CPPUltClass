/* 
** http·â×°Àà
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
    DWORD len = 1;
    wchar_t tmp[1];
    InternetCanonicalizeUrl(url.c_str(), tmp, &len, ICU_BROWSER_MODE);
    wchar_t* canonical_url = new wchar_t[len];
    InternetCanonicalizeUrl(url.c_str(), canonical_url, &len, ICU_BROWSER_MODE);
    hopenurl = InternetOpenUrl(hopen, canonical_url, NULL, 0,
      INTERNET_FLAG_NO_UI | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    delete[] canonical_url;
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