/*
** �ṩ�ļ������̲���
** author
**   taoabc@gmail.com
*/
#ifndef ULT_FILE_H_
#define ULT_FILE_H_

#include <string>

namespace ult
{
	std::wstring GetMaxFreeSapceDisk(unsigned int* size_in_mb);
	std::wstring GetAppDataPath(void);
}

#endif // ULT_FILE_H_