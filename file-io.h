/*
** ÎÄ¼þÀà
** author
**   taoabc@gmail.com
*/
#ifndef ULT_FILE_H_
#define ULT_FILE_H_

#include <string>
#include <windows.h>

namespace ult {
namespace file {

class File {

public:

File(void) : hfile_(NULL) {

}

~File(void) {
  if (hfile_ != NULL) {
    Close();
  }
}

bool IsOpened(void) {
  return hfile_ != NULL;
}

void Attach(HANDLE hfile) {
  Close();
  hfile_ = hfile;
}

HANDLE Detach(void) {
  HANDLE f = hfile_;
  hfile_ = NULL;
  return f;
}

bool Close(void) {
  if (hfile_ == NULL) {
    return true;
  }
  if (!::CloseHandle(hfile_)) {
    return false;
  }
  hfile_ = NULL;
  return true;
}

bool Open(const std::wstring& filename) {
  return OpenOperation(filename, GENERIC_READ);
}

bool Open(const std::wstring& filename, DWORD dwaccess) {
  return OpenOperation(filename, dwaccess);
}

bool OpenOperation(const std::wstring& filename, DWORD dwaccess) {
  hfile_ = ::CreateFile(filename.c_str(), dwaccess, FILE_SHARE_READ, NULL,
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hfile_ == INVALID_HANDLE_VALUE) {
    return false;
  }
  return true;
}

bool Create(const std::wstring& filename, bool create_always) {

}

unsigned __int64 GetPosition(void) {

}

unsigned __int64 GetLength(void) {

}

bool Seek(unsigned __int64 position, unsigned __int64 new_position) {

}

bool SeekToBegin(void) {

}

bool SeekToEnd(unsigned __int64* new_position) {

}

bool Read() {

}

bool Write() {

}

private:

  HANDLE hfile_;
};
} //namespace ult

using namespace file;

} //namespace ult

#endif