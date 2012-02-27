#include "StdAfx.h"
#include "UltFile.h"
#include <Windows.h>

namespace Ult {
CString GetMaxFreeSapceDisk( unsigned int* space_in_mb )
{
	DWORD buf_len = GetLogicalDriveStrings(0, NULL);
	wchar_t* buf = new wchar_t[buf_len];

	ULARGE_INTEGER freespace;
	ULONGLONG maxfree = 0;

	CString disk;
	
	if (0 != GetLogicalDriveStrings(buf_len, buf)) {
		wchar_t* drive = buf;
		DWORD i = 0;
		while (i <= buf_len) {
			if (DRIVE_FIXED == GetDriveType(drive)) {
				GetDiskFreeSpaceEx(drive, &freespace, NULL, NULL);
				ULONGLONG t = freespace.QuadPart;
				if (t > maxfree) {
					maxfree = t;
					disk.SetString(drive);
				}
			}
			i += wcslen(drive) + 1;
			drive += i;
		}
	}
	delete[] buf;

	*space_in_mb = maxfree / 1024 / 1024;
	return disk;
}
} //namespace Ult