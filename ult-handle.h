/*
** 多句柄等待管理
** author
**   taoabc@gmail.com
*/

#include <windows.h>
#include <vector>

namespace ult {

class HandleWaiter
{
public:
  ~HandleWaiter();

  void PushBack(HANDLE h);
  void WaitAll(int milliseconds);
private:
  std::vector<HANDLE*> array_vec_;
  int count_;
};

}
