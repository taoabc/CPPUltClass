/* http·â×°Àà
** author
**   taoabc@gmail.com */
#ifndef ULT_HTTP_H_
#define ULT_HTTP_H_

#include <string>
#include <vector>
#include <Wininet.h>

#pragma comment(lib, "Wininet.lib")

namespace ult {

class HttpRequest {

public:

  HttpRequest(void) {

  }

  ~HttpRequest() {
    if (hopen_ != NULL) {
      InternetCloseHandle(hopen_);
    }
    if (hopenurl_ != NULL) {
      InternetCloseHandle(hopenurl_);
    }
  }

private:

  HINTERNET hopen_;
  HINTERNET hopenurl_;

};

inline bool HttpRequest(const std::wstring& url, std::string* content) {
  HINTERNET hopen = NULL;
  HINTERNET hopenurl = NULL;
  DWORD dwsize = 0;
  DWORD dwdownloaded = 0;
  char* buf = NULL;
  bool result = false;

  hopen = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG,
    NULL, NULL, 0);
  if (hopen != NULL) {
    DWORD len = 1;
    wchar_t tmp[1];
    //almost every time, this function will fail and we get the length we need
    InternetCanonicalizeUrl(url.c_str(), tmp, &len, ICU_BROWSER_MODE);
    wchar_t* canonical_url = new wchar_t[len];
    InternetCanonicalizeUrl(url.c_str(), canonical_url, &len, ICU_BROWSER_MODE);
    hopenurl = InternetOpenUrl(hopen, canonical_url, NULL, 0,
      INTERNET_FLAG_NO_UI | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    delete[] canonical_url;
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

} // namespace ult

#endif