#include "StdAfx.h"
#include "ult-file.h"
#include <Windows.h>
#include <shlobj.h>

namespace ult {
std::wstring GetMaxFreeSapceDisk( unsigned int* size_in_mb )
{
	DWORD buf_len = GetLogicalDriveStrings(0, NULL);
	wchar_t* buf = new wchar_t [buf_len];

	ULARGE_INTEGER freespace;
	ULONGLONG maxfree = 0;

	std::wstring disk;
	
	if (0 != GetLogicalDriveStrings(buf_len, buf)) {
		wchar_t* drive = buf;
		DWORD i = 0;
		while (i <= buf_len) {
			if (DRIVE_FIXED == GetDriveType(drive)) {
				GetDiskFreeSpaceEx(drive, &freespace, NULL, NULL);
				ULONGLONG t = freespace.QuadPart;
				if (t > maxfree) {
					maxfree = t;
					disk.assign(drive);
				}
			}
			i += static_cast<DWORD>(wcslen(drive)) + 1;
			drive += i;
		}
	}
	delete[] buf;

	*size_in_mb = static_cast<int>(maxfree / 1024 / 1024);
	return disk;
}

std::wstring GetAppDataPath( void )
{
	wchar_t buf[MAX_PATH];
	::SHGetSpecialFolderPath(NULL, buf, CSIDL_APPDATA, false);
	std::wstring result(buf);
	return result;
}

std::wstring GetFileUpperFolder( const std::wstring& file_name )
{
  int pos = file_name.rfind(L"\\");
  return file_name.substr(0, pos + 1);
}

std::wstring GetFileTitleName( const std::wstring& file_name )
{
  std::wstring name = GetFileNameWithoutPath(file_name);
  int pos = name.rfind(L".");
  std::wstring title_name(name);
  if (0 < pos) {
    title_name = name.substr(0, pos);
  }
  return title_name;
}

std::wstring GetFileExtensionName( const std::wstring& file_name )
{
  std::wstring name = GetFileNameWithoutPath(file_name);
  int pos = name.rfind(L".");
  std::wstring extension(L"");
  if (0 < pos) {
    extension = name.substr(pos + 1, name.length() - pos - 1);
  }
  return extension;
}

std::wstring GetFileNameWithoutPath( const std::wstring& file_name )
{
  int pos = file_name.rfind(L"\\");
  int len = file_name.length();
  std::wstring name(L"");
  if (1 < pos && len > pos + 1) {
    name = file_name.substr(pos + 1, len - pos -1);
  }
  return name;
}

std::wstring GetModulePath( void )
{
  wchar_t buf[MAX_PATH];
  GetModuleFileName(NULL, buf, MAX_PATH);
  std::wstring path(buf);
  int pos = path.rfind(L"\\");
  return path.substr(0, pos + 1);
}

bool MakeSureFolderExist( const std::wstring& folder_path )
{
  bool ret = false;
  if (0 == CreateDirectory(folder_path.c_str(), NULL)) {
    if (ERROR_ALREADY_EXISTS == GetLastError()) {
      ret = true;
    }
  } else {
    ret = true;
  }
  return ret;
}

bool IsFileExist( const std::wstring& file )
{
  if (-1 != _waccess(file.c_str(), 0)) {
    return true;
  } else {
    return false;
  }
}

} //namespace Ult