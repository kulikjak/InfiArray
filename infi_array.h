#ifndef _INFI_ARRAY_H_
#define _INFI_ARRAY_H_

#define _BLOCK_SIZE 2048

#define _LOWER_MASK (_BLOCK_SIZE - 1)
#define _UPPER_MASK (~_LOWER_MASK)

#define __GET_BLOCK_KEY(x) (x & _UPPER_MASK)

#include <cstring>
#include <map>


class InfiArray {
 public:
  typedef int32_t value_type;
  typedef size_t size_type;

  typedef value_type& reference;
  typedef const value_type& const_reference;

  /**  Default constructor creates no elements.  */
  InfiArray() {}

  /**
   *  @brief  %InfiArray copy constructor.
   *  @param  x  An %InfiArray.
   *
   *  The newly-created %InfiArray will have copies of all the elements of @a.
   *  Extra allocated memory in @a will be also copied as there is no fast way
   *  to check it (block_shrink is not too fast).
   */
  InfiArray(const InfiArray& __x) {
    for (auto __b : __x._memory) {
      _memory[__b.first] = new value_type[_BLOCK_SIZE]();
      memcpy(_memory[__b.first], __b.second, _BLOCK_SIZE * sizeof(value_type));
    }
  }

  /**  The dtor erases all allocated memory blocks.  */
  ~InfiArray() {
    for (auto __b : _memory) delete[] __b.second;
  }

  /**  Erases all the memory blocks.  */
  void clear() {
    for (auto __b : _memory)
      delete[] __b.second;
    _memory.clear();
  }

  /**
   *  @brief  Swaps data with another %InfiArray.
   *  @param  x  An %InfiArray.
   *
   *  This exchanges the elements between two InfiArrays in constant time.
   *  (Only using swap function of map objects.
   */
  void swap(InfiArray& __x) {
    _memory.swap(__x._memory);
  }

  /**
   *  Deallocates all zeroed out blocks. This can by pretty time consuming
   *  as the program needs to check all allocated blocks for nonzero elements.
   */
  void block_shrink() {
    for (auto it = _memory.begin(); it != _memory.end(); ) {
      if (!_test_block((*it).second)) {
        delete[] (*it).second;
        _memory.erase(it++);
      } else {
        it++;
      }
    }
  }

  /**  Returns the number of memory blocks in the %InfiArray.  */
  size_type block_count() const {
    return _memory.size();
  }

  /**
   *  @brief  %InfiArray assignment operator.
   *  @param  x  An %InfiArray.
   *
   *  The newly-created %InfiArray will have copies of all the elements of @a.
   *  Extra memory in @a will not be copied.
   */
  InfiArray& operator=(const InfiArray& __x) {
    this->clear();
    for (auto __b : __x._memory) {
      _memory[__b.first] = new value_type[_BLOCK_SIZE]();
      memcpy(_memory[__b.first], __b.second, _BLOCK_SIZE * sizeof(value_type));
    }
  }

  /**
   *  @brief  Subscript access to the data contained in the %InfiArray
   *  @param n The index of the element for which data should be accessed.
   *  @return  Value of the data.
   *
   *  This operator allows for easy, array-style, data access.
   */
  value_type operator[] (size_type __n) const {
    return this->get(__n);
  }

  /**
   *  @brief  Subscript access to the data contained in the %InfiArray
   *  @param n The index of the element for which data should be accessed.
   *  @return  Read/write reference to data.
   *
   *  This operator allows for easy, array-style, data access.
   *  If requested block does not exists, it will be allocated.
   */
  reference operator[] (size_type __n) {
    if (!_block_existence(__n))
      _alloc_block(__n);
    return _memory[__GET_BLOCK_KEY(__n)][(__n & _LOWER_MASK)];
  }

  /**
   *  @brief  Provides access to the data contained in the %InfiArray.
   *  @param n The index of the element for which data should be accessed.
   *  @return  Const value of the data.
   *
   *  This function provides for simple data access.
   *  There is no non const variation of this function. Because of that
   *  no memory block can be allocated while using this function.
   */
  const value_type get(size_type __n) const {
    if (!_block_existence(__n))
      return 0;
    return _memory.at(__GET_BLOCK_KEY(__n))[__n & _LOWER_MASK];
  }

 private:
  /**
   *  @brief  Get memory block at given index.
   *  @param  key  An index of requested memory.
   *
   *  A key can be any value for given memory block as it is rouded down
   *  to correct memory block address.
  */
  value_type* _get_block(size_type __key) const {
    __key = __GET_BLOCK_KEY(__key);
    auto __i = _memory.lower_bound(__key);

    if (__i == _memory.end() || _memory.key_comp()(__key, (*__i).first))
      return nullptr;
    return (*__i).second;
  }

  /**
   *  @brief  Allocate new memory block at given index.
   *  @param  key  An index of requested memory.
   *
   *  A key can be any value for given memory block as it is rouded down
   *  to correct memory block address.
  */
  void _alloc_block(size_type __key) {
    if (_block_existence(__key))
      throw;
    _memory[__GET_BLOCK_KEY(__key)] = new value_type[_BLOCK_SIZE]();
  }

  /**
   *  @brief  Deallocate memory block at given index.
   *  @param  key  An index of requested memory.
   *
   *  A key can be any value for given memory block as it is rouded down
   *  to correct memory block address.
  */
  void _dealloc_block(size_type __key) {
    value_type* __block = _get_block(__key);
    _memory.erase(__GET_BLOCK_KEY(__key));

    delete[] __block;
  }

  /**
   *  @brief  Test memory block for nonzero values.
   *  @param  block  An memory block.
   *  @return  True iff block contains nonzero elements.
  */
  bool _test_block(value_type* __block) const {
    if (__block == nullptr)
      return false;

    for (size_type __i = 0; __i < _BLOCK_SIZE; __i++) {
      if (__block[__i]) return true;
    }
    return false;
  }

  /**
   *  @brief  Test if memory block is allocated.
   *  @param  key  An index of requested memory.
   *  @return  True iff block exists.
   *
   *  A key can be any value for given memory block as it is rouded down
   *  to correct memory block address.
  */
  bool _block_existence(size_type __key) const {
    return _get_block(__key) != nullptr;
  }

  std::map<size_type, value_type*> _memory;
};


#endif  /* _INFI_ARRAY_H_ */




  /**
   *  @brief  Provides access to the data contained in the %InfiArray.
   *  @param n The index of the element for which data should be accessed.
   *  @return  Value of the data.
   *
   *  This function provides for simple data access.
   */
  //value_type at(size_type __n) const;

  /**
   *  @brief  Provides access to the data contained in the %InfiArray.
   *  @param n The index of the element for which data should be accessed.
   *  @return  Read/write reference to data.
   *
   *  This function provides for simple data access.
   *  If requested block does not exists, it will be allocated.
   */
  //reference at(size_type __n);

