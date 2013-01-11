/*
** 文件内存映射封装
** author
**   taoabc@gmail.com
*/
#ifndef ULT_FILE_FILEMAP_H_
#define ULT_FILE_FILEMAP_H_

#include <windows.h>
#include <cassert>
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
    file_ = ::CreateFile(filename.c_str(), dwaccess, FILE_SHARE_READ, NULL,
      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_ == INVALID_HANDLE_VALUE) {
      return false;
    }
    return true;
  }

  bool CreateMapping(DWORD flprotect = PAGE_READONLY, ULONGLONG maximum_size = 0) {
    if (0 == this->GetFileSize()) {
      return false;
    }
    file_mapping_ = ::CreateFileMapping(file_, NULL, flprotect, maximum_size >> 32, maximum_size & 0xffffffff, NULL);
    if (file_mapping_ == NULL) {
      return false;
    }
    return true;
  }
  
  void* MapView(ULONGLONG offset, SIZE_T map_bytes, DWORD dwaccess = FILE_MAP_READ) {
    UnmapView();
    DWORD glt = GetAllocationGranularity();
    ULONGLONG real_offset = GranularityFloor(offset, glt);
    DWORD distance = static_cast<DWORD>(offset - real_offset);
    SIZE_T real_map_bytes = map_bytes + distance;
    pfile_view_ = ::MapViewOfFile(file_mapping_, dwaccess, real_offset >> 32, real_offset & 0xffffffff, real_map_bytes);
    return static_cast<void*>((unsigned char*)pfile_view_ + distance);
  }

  void UnmapView(void) {
    if (pfile_view_ != NULL) {
      ::UnmapViewOfFile(pfile_view_);
      pfile_view_ = NULL;
    }
  }
  
  void Close(void) {
    UnmapView();
    CloseMapping();
    CloseFile();
  }

  ULONGLONG GetFileSize(void) {
    DWORD size_high;
    DWORD size_low = ::GetFileSize(file_, &size_high);
    if (size_low == INVALID_FILE_SIZE && GetLastError != NO_ERROR) {
      return 0;
    }
    return (((ULONGLONG)size_high) << 32) + size_low;
  }

private:

  DWORD GetAllocationGranularity(void) {
    static DWORD glt = 0;
    if (glt == 0) {
      SYSTEM_INFO sys_info;
      ::GetSystemInfo(&sys_info);
      glt = sys_info.dwAllocationGranularity;
    }
    return glt;
  }

  ULONGLONG GranularityCeil(ULONGLONG number, ULONGLONG granularity) {
    assert(granularity != 0);
    while (number % granularity != 0) {
      ++number;
    }
    return number;
  }

  ULONGLONG GranularityFloor(ULONGLONG number, ULONGLONG granularity) {
    assert(granularity != 0);
    while (number % granularity != 0) {
      --number;
    }
    return number;
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

  HANDLE file_;
  HANDLE file_mapping_;
  PVOID pfile_view_;
}; //class FileMap
} //namespace ult

#endif