#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

#include "infi_array.h"

#define _TESTING_CHUCK 10000000

#define _START_TIME \
  { __start = clock(); }
#define _STOP_TIME(__str)                                                      \
  {                                                                            \
    std::cout << std::setprecision(6)                                          \
              << (float)(clock() - __start) / CLOCKS_PER_SEC << "s  " << __str \
              << std::endl;                                                    \
  }

int main(int argc, char **argv) {
  int32_t j;
  clock_t __start;

  InfiArray infi;

  std::cout << "Running test with chuck of size " << _TESTING_CHUCK
            << " elements.\n"
            << std::endl;

  std::cout << "InfiArray" << std::endl;

  _START_TIME
  for (int i = 0; i < _TESTING_CHUCK; i++) j = infi.get(i);
  _STOP_TIME("[InfiArray] reading from empty array with .get()")

  {
    InfiArray::const_iterator bit = infi.get_const_iterator(0);
    InfiArray::const_iterator eit = infi.get_const_iterator(_TESTING_CHUCK);

    _START_TIME
    for (auto it = bit; it != eit; it++) j = *it;
    _STOP_TIME("[InfiArray] reading from empty array with const_iterator")
  }

  {
    InfiArray::iterator bit = infi.get_iterator(0);
    InfiArray::iterator eit = infi.get_iterator(_TESTING_CHUCK);

    _START_TIME
    for (auto it = bit; it != eit; it++) j = *it;
    _STOP_TIME(
        "[InfiArray] reading from empty array with non const iterator "
        "(allocating)")
  }

  infi.clear();

  _START_TIME
  for (int i = 0; i < _TESTING_CHUCK; i++) j = infi[i];
  _STOP_TIME(
      "[InfiArray] reading from empty array with [] operator (allocating)")

  _START_TIME
  for (int i = 0; i < _TESTING_CHUCK; i++) j = infi.get(i);
  _STOP_TIME("[InfiArray] reading from non empty array with .get()")

  {
    InfiArray::const_iterator bit = infi.get_const_iterator(0);
    InfiArray::const_iterator eit = infi.get_const_iterator(_TESTING_CHUCK);

    _START_TIME
    for (auto it = bit; it != eit; it++) j = *it;
    _STOP_TIME("[InfiArray] reading from non empty array with const_iterator")
  }

  infi.clear();

  _START_TIME
  for (int i = 0; i < _TESTING_CHUCK; i++) infi[i] = 1;
  _STOP_TIME("[InfiArray] writing into empty array with [] operator")

  _START_TIME
  for (int i = 0; i < _TESTING_CHUCK; i++) infi[i] = 1;
  _STOP_TIME("[InfiArray] writing into non empty array with [] operator")

  infi.clear();

  {
    InfiArray::iterator bit = infi.get_iterator(0);
    InfiArray::iterator eit = infi.get_iterator(_TESTING_CHUCK);

    _START_TIME
    for (auto it = bit; it != eit; it++) (*it) = 1;
    _STOP_TIME("[InfiArray] writing into empty array with iterator")
  }

  {
    InfiArray::iterator bit = infi.get_iterator(0);
    InfiArray::iterator eit = infi.get_iterator(_TESTING_CHUCK);

    _START_TIME
    for (auto it = bit; it != eit; it++) (*it) = 1;
    _STOP_TIME("[InfiArray] writing into non empty array with iterator")
  }

  std::cout << "\nstd::vector" << std::endl;

  {
    std::vector<int32_t> vect;
    vect.reserve(_TESTING_CHUCK);

    _START_TIME
    for (int i = 0; i < _TESTING_CHUCK; i++) vect[i] = 1;
    _STOP_TIME(
        "[std::vector] writing into precallocated vector with [] operator")
  }

  {
    std::vector<int32_t> vect;
    vect.reserve(_TESTING_CHUCK);

    _START_TIME
    for (int i = 0; i < _TESTING_CHUCK; i++) vect.push_back(1);
    _STOP_TIME(
        "[std::vector] writing into precallocated vector with push_back()")
  }

  {
    std::vector<int32_t> vect;

    _START_TIME
    for (int i = 0; i < _TESTING_CHUCK; i++) vect.push_back(1);
    _STOP_TIME("[std::vector] writing into vector with push_back()")

    {
      std::vector<int32_t>::const_iterator bit = vect.begin();
      std::vector<int32_t>::const_iterator eit = vect.end();

      _START_TIME
      for (auto it = bit; it != eit; it++) j = (*it);

      _STOP_TIME("[std::vector] reading from vector with const_iterator")
    }

    _START_TIME
    for (int i = 0; i < _TESTING_CHUCK; i++) j = vect[i];
    _STOP_TIME("[std::vector] reading from vector with [] operator")
  }

  return EXIT_SUCCESS;
}
