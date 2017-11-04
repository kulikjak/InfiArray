#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>

#include "infi_array.h"

#define _VERBOSE true
#define _TEST(__str)                               \
  {                                                \
    if (_VERBOSE) std::cout << __str << std::endl; \
  }

int main(int argc, char **argv) {
  InfiArray infi;

  std::cout << "InfiArray tests" << std::endl;

  _TEST("Reading from empty array with get function")
  for (int i = -5; i < 5; i++) assert(infi.get(i) == 0);

  _TEST("Reading from empty array with [] operator (allocating)")
  for (int i = -5; i < 5; i++) assert(infi[i] == 0);

  _TEST("Clearing the array")
  assert(infi.block_count() == 2);
  infi.clear();
  assert(infi.block_count() == 0);

  _TEST("Writing into the empty array with [] operator")
  for (int i = -5; i < 5; i++) infi[i] = 1;

  _TEST("Reading from nonempty array with get function")
  for (int i = -5; i < 5; i++) assert(infi.get(i) == 1);
  for (int i = 5; i < 3000; i++) assert(infi.get(i) == 0);

  _TEST("Block shrink function")
  for (int i = -5; i < 5; i++) infi[i] = 0;
  infi.block_shrink();
  assert(infi.block_count() == 0);

  _TEST("Copy constructor")
  {
    for (int i = -5; i < 5; i++) infi[i] = 1;

    InfiArray second(infi);
    infi[3] = 2;
    second[3] = 3;

    for (int i = -5; i < 3; i++) assert(second[i] == 1);

    assert(infi[3] == 2);
    assert(second[3] == 3);

    second[3000] = 1;
    assert(infi.block_count() == 2);
    assert(second.block_count() == 3);

    _TEST("Copy assignment operator")
    InfiArray third = infi;

    third[3] = 1;

    for (int i = -5; i < 5; i++) assert(third[i] == 1);

    third[-3000] = 1;
    assert(infi[3] == 2);
    assert(third[3] == 1);

    assert(infi.block_count() == 2);
    assert(third.block_count() == 3);

    _TEST("Swap function")
    second.swap(third);

    assert(second.block_count() == 3);
    assert(third.block_count() == 3);

    assert(second[-3000] == 1);
    assert(third[3000] == 1);

    second.clear();
    InfiArray().swap(third);

    assert(second.block_count() == 0);
    assert(third.block_count() == 0);
  }

  infi.clear();

  _TEST("iterator test")

  {
    InfiArray::iterator bitx = infi.get_iterator(0);
    InfiArray::iterator eitx = infi.get_iterator(100);

    for (auto it = bitx; it != eitx; it++)
      *it = 1;

    InfiArray::const_iterator bit = infi.get_const_iterator(0);
    InfiArray::const_iterator eit = infi.get_const_iterator(100);

    for (auto it = bit; it != eit; it++)
      assert((*it) == 1);
  }

  return EXIT_SUCCESS;
}
