/*
** 句柄管理
** 使用范型模板编程
** author
**   taoabc@gmail.com
*/
#ifndef ULT_BASE_HANDLEBASE_H_
#define ULT_BASE_HANDLEBASE_H_

namespace ult {

template <typename Type>
class HandleBase {

public:

  HandleBase(void) :
      handle_(NULL) {
  }

  virtual ~HandleBase(void) {
  };

  bool Attach(Type handle) {
    Close();
    handle_ = handle;
    return handle_ != NULL;
  }

  Type Detach(void) {
    Type h = handle_;
    handle_ = NULL;
    return h;
  }

  Type GetHandle(void) const {
    return handle_;
  }

  virtual void Close(void) = 0;

protected:

  Type handle_;

}; //class HandleBase

} //namespace ult

#endif