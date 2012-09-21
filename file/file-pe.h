/*
** peœ‡πÿ
** author
**   taoabc@gmail.com
*/
#ifndef ULT_FILE_FILEPE_H_
#define ULT_FILE_FILEPE_H_

#include "file-io.h"

namespace ult{

class FilePE {

public:

  FilePE(void) {
    memset(&dos_header_, 0, sizeof (dos_header_));
    memset(&nt_headers_, 0, sizeof (nt_headers_));
  }

  ~FilePE(void) {
    file_.Close();
  }

  bool Open(const std::wstring& filename) {
    file_.Close();
    if (!file_.Open(filename)) {
      return false;
    }
    DWORD dwread;
    if (!file_.Read(&dos_header_, sizeof(IMAGE_DOS_HEADER), &dwread)) {
      return false;
    }
    unsigned __int64 new_position;
    if (!file_.Seek(dos_header_.e_lfanew, &new_position)) {
      return false;
    }
    if (!file_.Read(&nt_headers_, sizeof(IMAGE_NT_HEADERS), &dwread)) {
      return false;
    }
    file_.SeekToBegin();
    return true;
  }
  
  bool Close(void) {
    return file_.Close();
  }

  bool IsValidPE(void) {
    if (dos_header_.e_magic != IMAGE_DOS_SIGNATURE ||
        nt_headers_.Signature != IMAGE_NT_SIGNATURE) {
      return false;
    }
    return true;
  }

  DWORD GetSignSize(void) {
    if (!IsValidPE()) {
      return 0;
    }
    return nt_headers_.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
  }

private:

  File file_;

  IMAGE_DOS_HEADER dos_header_;
  IMAGE_NT_HEADERS nt_headers_;

};

}//namespace ult

#endif