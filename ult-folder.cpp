#include "StdAfx.h"
#include "ult-folder.h"
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

} //namespace Ult