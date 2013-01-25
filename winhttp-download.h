/*
** 同步GET方式下载
** winhttp
** author
**   taoabc@gmail.com
*/
#ifndef ULT_WINHTTPDOWNLOAD_H_
#define ULT_WINHTTPDOWNLOAD_H_

#include "./winhttp-request.h"
#include "./file-io.h"
#include "./file-dir.h"

#include <string>

namespace ult {

namespace detail {

struct HttpDownloadString {
  HRESULT operator()(const std::wstring& url, std::string* s) {
    WinHttpRequest req;
    RETURN_IF_FAILED(req.Request(url));
    LPVOID buffer;
    DWORD readed;
    while (SUCCEEDED(req.ReadData(&buffer, &readed)) && readed != 0) {
      s->append((char*)buffer, readed);
    }
    return S_OK;
  }
};

struct HttpDownloadFile {
  HRESULT operator()(const std::wstring& url, const std::wstring& file) {
    WinHttpRequest req;
    RETURN_IF_FAILED(req.Request(url));
    LPVOID buffer;
    DWORD readed;
    std::wstring file_folder(ult::GetUpperDirectory(file));
    if (!ult::MakeSureFolderExist(file_folder)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    ult::File down_file;
    if (!down_file.Create(file, true)) {
      return HRESULT_FROM_WIN32(::GetLastError());
    }
    DWORD written;
    while (SUCCEEDED(req.ReadData(&buffer, &readed)) && readed != 0) {
      down_file.Write(buffer, readed, &written);
    }
    return S_OK;
  }
};

} //namespace detail

inline HRESULT HttpDownloadString(const std::wstring& url, std::string* s) {
  return detail::HttpDownloadString()(url, s);
}

inline HRESULT HttpDownloadFile(const std::wstring& url, const std::wstring& file) {
  return detail::HttpDownloadFile()(url, file);
}

} //namespace ult

#endif