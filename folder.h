/*
** 提供文件及磁盘操作
** author
**   taoabc@gmail.com
*/
#ifndef ULT_FILE_H_
#define ULT_FILE_H_

#include <string>

namespace ult
{
	std::wstring GetMaxFreeSapceDisk(unsigned int* space_in_mb);
}

#endif // ULT_FILE_H_