/*
** Êı×Ö¼ÆËã
** author
**   taoabc@gmail.com
*/
#ifndef ULT_NUMBER_H_
#define ULT_NUMBER_H_

#include <ctime>
#include <vector>

namespace ult {
namespace number {

inline void SwapInteger(int* x, int *y) {
  if (*x != *y) {
    *x = *x ^ *y;
    *y = *x ^ *y;
    *x = *x ^ *y;
  }
}

inline int GetRandomInteger(int min_number, int max_number) {
  static bool seeded = false;
  if (!seeded) {
    srand((unsigned int)time(NULL));
    seeded = true;
  }
  if (min_number > max_number) {
    SwapInteger(&min_number, &max_number);
  }
  return (double)rand() / RAND_MAX * (max_number - min_number) + min_number;
}

inline void DisorderInteger(int begin_number, int end_number, std::vector<int>* vec) {
  if (begin_number > end_number) {
    SwapInteger(&begin_number, &end_number);
  }
  for (int i = begin_number; i <= end_number; ++i) {
    vec->push_back(i);
  }

  int vec_size = vec->size();
  int r;
  for (int i = 0; i < vec_size; ++i) {
    r = GetRandomInteger(0, vec_size - 1);
    SwapInteger(&(*vec)[i], &(*vec)[r]);
  }
}

} //namespace number

using namespace number;

} //namespace ult

#endif