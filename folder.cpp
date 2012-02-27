#include "StdAfx.h"
#include "folder.h"
#include <Windows.h>

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
			i += wcslen(drive) + 1;
			drive += i;
		}
	}
	delete[] buf;

	*size_in_mb = static_cast<int>(maxfree / 1024 / 1024);
	return disk;
}
} //namespace Ult