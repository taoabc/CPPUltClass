/*
** 提供文件及磁盘操作
** author
**   taoabc@gmail.com
*/
#ifndef ULT_FILE_DIR_H_
#define ULT_FILE_DIR_H_

#include <string>
#include <windows.h>
#include <shlobj.h>

namespace ult {
namespace filedir {

void SplitToPureNameAnddExtension(const wchar_t* fullname,
                                  std::wstring* purename,
                                  std::wstring* extension) {
  std::wstring fullname_string(fullname);
  int pos = fullname_string.rfind(L".");
  if (pos == std::wstring::npos) {
    purename->assign(fullname_string);
    extension->clear();
  } else {
    purename->assign(fullname_string.substr(0, pos));
    extension->assign(fullname_string.substr(pos+1));
  }
}

void SplitToPathPrefixAndFileName(const wchar_t* fullpath,
                                  const wchar_t* pathseparator,
                                  std::wstring* pathprefix,
                                  std::wstring* filename) {
  std::wstring fullpath_string(fullpath);
  int separator_len = wcslen(pathseparator);
  int pos = fullpath_string.rfind(pathseparator);
  if (pos == std::wstring::npos) {
    pathprefix->clear();
    filename->assign(fullpath_string);
  } else {
    pathprefix->assign(fullpath_string.substr(0, pos+separator_len));
    filename->assign(fullpath_string.substr(pos+separator_len));
  }
}

void NormalizeDirPathPrefix(std::wstring* dirpath) {
  if (dirpath->empty()) {
    return;
  }
  if (dirpath->rfind(L"\\") != dirpath->length()-1) {
    dirpath->append(L"\\");
  }
}

void GetMaxFreeSpaceDrive(std::wstring* drive,
                          unsigned __int64* freesize) {
  DWORD buf_len = ::GetLogicalDriveStrings(0, NULL);
  wchar_t* buf = new wchar_t [buf_len];

  ULARGE_INTEGER freespace;
  ULONGLONG maxfree = 0;

  if (0 != ::GetLogicalDriveStrings(buf_len, buf)) {
    wchar_t* drive_tmp = buf;
    DWORD i = 0;
    while (i <= buf_len) {
      if (DRIVE_FIXED == ::GetDriveType(drive_tmp)) {
        ::GetDiskFreeSpaceEx(drive_tmp, &freespace, NULL, NULL);
        ULONGLONG t = freespace.QuadPart;
        if (t > maxfree) {
          maxfree = t;
          drive->assign(drive_tmp);
        }
      }
      i += static_cast<DWORD>(wcslen(drive_tmp)) + 1;
      drive_tmp += i;
    }
  }
  delete[] buf;
  *freesize = maxfree;
}

void GetAppDataDirectory(std::wstring* directory) {
  wchar_t buf[MAX_PATH];
  ::SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buf);
  directory->assign(buf);
  delete[] buf;
}
  std::wstring GetModulePath(void);
  bool IsFileExist(const std::wstring& file);
  bool MakeSureFolderExist(const std::wstring& folder_path);
} //namespace filedir

using namespace filedir;

} //namespace ult

#endif // ULT_FILE_DIR_H_