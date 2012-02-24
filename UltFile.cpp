#include "StdAfx.h"
#include "UltFile.h"
#include <Windows.h>

CString Ult::GetMaxFreeSapceDisk( unsigned int* spaceInM )
{
	DWORD bufLen = GetLogicalDriveStrings(0, NULL);
	wchar_t* buf = new wchar_t[bufLen];

	ULARGE_INTEGER freespace;
	ULONGLONG maxFree = 0;

	CString disk;
	
	if (0 != GetLogicalDriveStrings(bufLen, buf)) {
		wchar_t* pDrive = buf;
		int i = 0;
		while (i <= bufLen) {
			if (DRIVE_FIXED == GetDriveType(pDrive)) {
				GetDiskFreeSpaceEx(pDrive, &freespace, NULL, NULL);
				ULONGLONG t = freespace.QuadPart;
				if (t > maxFree) {
					maxFree = t;
					disk.SetString(pDrive);
				}
			}
			i += wcslen(pDrive) + 1;
			pDrive += i;
		}
	}
	delete[] buf;

	*spaceInM = maxFree / 1024 / 1024;
	return disk;
}
