/*
** 简单自增长自释放缓冲区管理
** author
**   taoabc@gmail.com
*/
#ifndef ULT_SIMPLEBUFFER_H_
#define ULT_SIMPLEBUFFER_H_

#include <cstdlib>
#include <cstring>

namespace ult {

class SimpleBuffer {

public:

  static void Free(void* buffer) {
    if (buffer != NULL) {
      std::free(buffer);
    }
  }

  SimpleBuffer(void) {
    InitMember();
  }

  SimpleBuffer(size_t size) {
    InitMember();
    Grow(size);
  }

  ~SimpleBuffer(void) {
    FreeBuffer();
  }

  void Free(void) {
    FreeBuffer();
    InitMember();
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

  bool Reserve(size_t len) {
    return Grow(len);
  }

  void* Detach(void) {
    EnsureAtHeap();
    void* p = buffer_;
    InitMember();
    return p;
  }

  size_t Size(void) const {
    return data_size_;
  }

  size_t Capacity(void) const {
    return capacity_;
  }

private:

  void InitMember(void) {
    buffer_ = small_buffer_;
    data_size_ = 0;
    capacity_ = kSmallBufferSize_;
  }

  void FreeBuffer(void) {
    if (buffer_ != small_buffer_) {
      std::free(buffer_);
    }
  }

  //grow to new_size
  bool Grow(size_t new_size) {
    //no need to grow
    if (new_size <= capacity_) {
      return true;
    }
    void* new_buffer = NULL;
    //if use small buffer
    if (capacity_ <= kSmallBufferSize_) {
      new_buffer = std::malloc(new_size);
      if (new_buffer != NULL) {
       std::memcpy(new_buffer, buffer_, data_size_);
      }
    //if use heap, need realloc
    } else {
      new_buffer = std::realloc(buffer_, new_size);
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
      //use double space if step less then maximum step
      new_capacity += min(new_capacity, kMaxStep_);
    }
    return new_capacity;
  }

  bool EnsureAtHeap(void) {
    //move stack buffer to heap
    if (capacity_ <= kSmallBufferSize_) {
      if (!Grow(kSmallBufferSize_ + 1)) {
        return false;
      }
    }
    return true;
  }
  
  static const size_t kSmallBufferSize_ = 24;
  static const size_t kMaxStep_ = 8 * 1024;

  char small_buffer_[kSmallBufferSize_];
  void* buffer_;
  size_t capacity_;
  size_t data_size_;
}; //class SimpleBuffer
} //namespace ult

#endif