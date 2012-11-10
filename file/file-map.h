/*
** 文件内存映射封装
** author
**   taoabc@gmail.com
*/
#ifndef ULT_FILE_FILEMAP_H_
#define uLT_FILE_FILEMAP_H_

#include <windows.h>
#include <string>

namespace ult {

class FileMap {

public:

  FileMap(void) :
      file_(NULL), 
      file_mapping_(NULL), 
      pfile_view_(NULL) {
  }

  ~FileMap(void) {
    Close();
  }

  bool Open(const std::wstring& filename, DWORD dwaccess = GENERIC_READ) {
    Close();
    file_ = ::CreateFile(filename.c_str(), dwaccess, FILE_SHARE_READ, NULL,
      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_ == INVALID_HANDLE_VALUE) {
      return false;
    }
    dwaccess_ = dwaccess;
    return true;
  }

  bool CreateMapping(void) {
    DWORD dwprotect;
    if (dwaccess_ == GENERIC_READ) {
      dwprotect = PAGE_READONLY;
      dwview_access_ = FILE_MAP_READ;
    } else if (dwaccess_ == (GENERIC_READ | GENERIC_WRITE)) {
      dwprotect = PAGE_READWRITE;
      dwview_access_ = FILE_MAP_WRITE;
    } else if (dwaccess_ == (GENERIC_READ| GENERIC_EXECUTE)) {
      dwprotect = PAGE_EXECUTE_READ;
      dwview_access_ = FILE_MAP_EXECUTE;
    } else if (dwaccess_ == (GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE)) {
      dwprotect = PAGE_EXECUTE_READWRITE;
      dwview_access_ = FILE_MAP_EXECUTE;
    }
    unsigned __int64 filesize = this->GetSize();
    if (filesize == 0) {
      return false;
    }
    UnmapView();
    CloseMapping();
    file_mapping_ = ::CreateFileMapping(file_, NULL, dwprotect, 0, 0, NULL);
    if (file_mapping_ == NULL) {
      return false;
    }
    return true;
  }
  
  void* MapView(UINT64 offset, SIZE_T map_bytes) {
    UnmapView();
    pfile_view_ = ::MapViewOfFile(file_mapping_, dwview_access_, offset >> 32, offset & 0xffffffff, map_bytes);
    return pfile_view_;
  }

  void UnmapView(LPVOID view) {
    if (view != NULL) {
      ::UnmapViewOfFile(view);
    }
  }
  
  void Close(void) {
    UnmapView();
    CloseMapping();
    CloseFile();
  }

  unsigned __int64 GetSize(void) {
    DWORD size_high;
    DWORD size_low = ::GetFileSize(file_, &size_high);
    if (size_low == INVALID_FILE_SIZE && GetLastError != NO_ERROR) {
      return 0;
    }
    return (((unsigned __int64)size_high) << 32) + size_low;
  }

private:

  void UnmapView(void) {
    if (pfile_view_ != NULL) {
      ::UnmapViewOfFile(pfile_view_);
      pfile_view_ = NULL;
    }
  }

  void CloseMapping(void) {
    if (file_mapping_ != NULL) {
      ::CloseHandle(file_mapping_);
      file_mapping_ = NULL;
    }
  }

  void CloseFile(void) {
    if (file_ != NULL) {
      ::CloseHandle(file_);
      file_ = NULL;
    }
  }

  DWORD dwaccess_;
  DWORD dwview_access_;
  HANDLE file_;
  HANDLE file_mapping_;
  PVOID pfile_view_;
}; //class FileMap
} //namespace ult
#endif