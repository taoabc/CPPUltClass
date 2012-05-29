/*
** 提供文件及磁盘操作
** author
**   taoabc@gmail.com
*/
#ifndef ULT_FILE_H_
#define ULT_FILE_H_

#include <string>

namespace ult {
  std::wstring GetFileNameWithoutPath(const std::wstring& file_name);
  std::wstring GetFileTitleName(const std::wstring& file_name);
  std::wstring GetFileExtensionName(const std::wstring& file_name);
  std::wstring GetFileUpperFolder(const std::wstring& file_name);
  std::wstring GetMaxFreeSapceDisk(unsigned int* size_in_mb);
	std::wstring GetAppDataPath(void);
  std::wstring GetModulePath(void);
  bool IsFileExist(const std::wstring& file);
  bool MakeSureFolderExist(const std::wstring& folder_path);
}

#endif // ULT_FILE_H_