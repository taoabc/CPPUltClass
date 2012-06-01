/*
** 文件信息
** author
**   taoabc@gmail.com
*/

#ifndef ULT_FILE_INFO_H_
#define ULT_FILE_INFO_H_

#include "file-io.h"
#include <windows.h>

namespace ult {
namespace fileinfo {

class FileInfo {
  
public:

  
  bool IsPEFile(void) {
    if (hfile_ == NULL) {
      return false;
    }

  }

private:

  HANDLE hfile_;
};

}//namespace fileinfo  

using namespace fileinfo;

}
#endif