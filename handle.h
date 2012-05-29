/*
** 多句柄等待管理
** author
**   taoabc@gmail.com
*/

#include <windows.h>
#include <vector>

namespace ult {

class HandleWaiter {

public:

  HandleWaiter(void) : count_(0),
                       all_closed_(true) {
  }

  ~HandleWaiter() {
    CloseAll();
  }

  bool PushBack(HANDLE h) {
    if (MAXIMUM_WAIT_OBJECTS == count_) {
      return false;
    }
    harr_[count_++] = h;
    all_closed_ = false;
    return true;
  }

  bool WaitAll(int milliseconds) {
    DWORD wait_ret = WaitForMultipleObjects(count_, harr_, true, milliseconds);
    Clear();
    if (WAIT_TIMEOUT == wait_ret || WAIT_FAILED == wait_ret) {
      return false;
    }
    return true;
  }

  void CloseAll(void) {
    if (all_closed_) {
      return;
    }
    for (int i = 0; i < count_; ++i) {
      CloseHandle(harr_[i]);
    }
    all_closed_ = true;
  }

  void Clear(void) {
    CloseAll();
    count_ = 0;
  }

private:

  HANDLE harr_[MAXIMUM_WAIT_OBJECTS];
  bool all_closed_;
  int count_;
};

}
