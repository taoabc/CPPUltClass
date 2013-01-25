/*
** 句柄管理
** 使用范型模板编程
** author
**   taoabc@gmail.com
*/
#ifndef ULT_BASE_HANDLEBASE_H_
#define ULT_BASE_HANDLEBASE_H_

namespace ult {

template <typename T>
class HandleBase {

public:

  HandleBase(void) :
      handle_(NULL) {
  }

  virtual ~HandleBase(void) {
  };

  bool Attach(T handle) {
    Close();
    handle_ = handle;
    return handle_ != NULL;
  }

  T Detach(void) {
    T h = handle_;
    handle_ = NULL;
    return h;
  }

  T GetHandle(void) const {
    return handle_;
  }

  virtual void Close(void) = 0;

protected:

  T handle_;

}; //class HandleBase

} //namespace ult

#endif