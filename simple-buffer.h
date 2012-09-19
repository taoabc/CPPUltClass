/*
** 简单自增长内存管理
** author
**   taoabc@gmail.com
*/
#ifndef ULT_SIMPLEBUFFER_H_
#define ULT_SIMPLEBUFFER_H_

#include <cstdlib>

namespace ult {

class SimpleBuffer {

public:

  SimpleBuffer(void) {
    InitMember();
  }

  SimpleBuffer(size_t size) {
    InitMember();
    Grow(size);
  }

  ~SimpleBuffer(void) {
    if (!manual_free_) {
      Free();
    }
  }

  void Free(void) {
    if (capacity_ > kSmallBufferSize_ && buffer_ != NULL) {
      std::free(buffer_);
      buffer_ = NULL;
    }
  }

  bool Append(const void* data, size_t len) {
    size_t after_append = data_size_ + len;
    //if length to append is too big
    if (after_append < data_size_ || after_append < len) {
      return false;
    }
    //if the size after append is greater than capacity
    if (data_size_ + len > capacity_) {
      if (!Grow(CalcNewCapacity(after_append))) {
        return false;
      }
    }
    std::memcpy((char*)buffer_ + data_size_, data, len);
    data_size_ += len;
    return true;
  }

  void* Data(void) const {
    return buffer_;
  }

  size_t Size(void) const {
    return data_size_;
  }

  size_t Capacity(void) const {
    return capacity_;
  }

  void SetManualFree(void) {
    manual_free_ = true;
  }

private:

  void InitMember(void) {
    data_size_ = 0;
    manual_free_ = false;
    buffer_ = small_buffer_;
    capacity_ = kSmallBufferSize_;
  }

  bool Grow(size_t new_size) {
    //if use small buffer
    void* new_buffer = NULL;
    if (capacity_ <= kSmallBufferSize_) {
      new_buffer = malloc(new_size);
      if (new_buffer != NULL) {
        memcpy(new_buffer, buffer_, data_size_);
      }
    //if use heap, need realloc
    } else {
      new_buffer = realloc(buffer_, new_size);
    }
    if (new_buffer == NULL) {
      return false;
    }
    buffer_ = new_buffer;
    capacity_ = new_size;
    return true;
  }

  size_t CalcNewCapacity(size_t new_size) {
    //check whether need grow
    if (new_size <= capacity_) {
      return 0;
    }
    size_t new_capacity = capacity_;
    while (new_capacity < new_size) {
      //use double space if step less then maximun step
      new_capacity += min(new_capacity, kMaxStep_);
    }
    return new_capacity;
  }
  
  static const size_t kSmallBufferSize_ = 24;
  static const size_t kMaxStep_ = 8 * 1024;

  char small_buffer_[kSmallBufferSize_];
  void* buffer_;
  size_t capacity_;
  size_t data_size_;
  bool manual_free_;
}; //class SimpleBuffer
} //namespace ult

#endif