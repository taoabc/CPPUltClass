/*
** Êı×Ö¼ÆËã
** author
**   taoabc@gmail.com
*/
#ifndef ULT_NUMBER_H_
#define ULT_NUMBER_H_

#include <ctime>
#include <vector>
#include <cstdlib>
#include <algorithm>

namespace ult {

//get random from [min_number, max_number)
inline int GetRandomInteger(int min_number, int max_number) {
  static bool seeded = false;
  if (!seeded) {
    std::srand((unsigned int)time(NULL));
    seeded = true;
  }
  if (min_number > max_number) {
    std::swap(min_number, max_number);
  }
  return (int)((double)std::rand() / (RAND_MAX + 1) * (max_number - min_number) + min_number);
}

//dis order number from [begin_number, end_number)
inline void DisorderInteger(int begin_number, int end_number, std::vector<int>* vec) {
  if (begin_number > end_number) {
    std::swap(begin_number, end_number);
  }
  for (int i = begin_number; i < end_number; ++i) {
    vec->push_back(i);
  }

  int vec_size = vec->size();
  int r;
  for (int i = 0; i < vec_size; ++i) {
    r = GetRandomInteger(0, vec_size);
    std::swap((*vec)[i], (*vec)[r]);
  }
}

namespace number {

inline void DecreaseVals(unsigned __int64* numberator, unsigned __int64* denominator) {
  static const unsigned __int64 kMax = (unsigned __int64)1 << 31;
  while (*numberator > kMax) {
    *numberator >>= 1;
    *denominator >>= 2;
  }
}

}

//to calculate numberator1*numberator2 / denominator
//this function solve really big number multiply
inline unsigned __int64 UIntMultDiv(unsigned __int64 numberator1,
                                    unsigned __int64 numberator2,
                                    unsigned __int64 denominator) {
  number::DecreaseVals(&numberator1, &denominator);
  number::DecreaseVals(&numberator2, &denominator);
  if (denominator == 0) {
    denominator = 1;
  }
  return numberator1 * numberator2 / denominator;
}

} //namespace ult

#endif