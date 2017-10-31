#ifndef _INFI_ARRAY_H_
#define _INFI_ARRAY_H_

#define _BLOCK_SIZE 2048

#define _LOWER_MASK (_BLOCK_SIZE - 1)
#define _UPPER_MASK (~_LOWER_MASK)

#define __GET_BLOCK_KEY(x) (x & _UPPER_MASK)
#define __GET_BLOCK_IDX(x) (x & _LOWER_MASK)

#include <cstdio>
#include <cstring>
#include <unordered_map>

class InfiArray {
 public:
  typedef int32_t value_type;

  typedef size_t size_type;
  typedef size_t difference_type;

  typedef value_type* pointer;
  typedef value_type& reference;

  typedef std::unordered_map<size_type, value_type*> infi_map;

  /**  Iterator forward declarations  **/
  class iterator;
  class const_iterator;

  template <bool _ConstIterator = true>
  class _base_iterator {
   public:
    typedef InfiArray::pointer pointer;
    typedef InfiArray::reference reference;
    typedef InfiArray::value_type value_type;
    typedef InfiArray::difference_type difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;

    typedef typename std::conditional<_ConstIterator, const InfiArray*,
                                      InfiArray*>::type _InfiType;
    typedef typename std::conditional<_ConstIterator, const value_type*,
                                      value_type*>::type _InfiArrayBlock;
    typedef typename std::conditional<_ConstIterator, const_iterator,
                                      iterator>::type _IteratorType;

    _base_iterator() {}

    _base_iterator(const _IteratorType& __it)
        : _key(__it._key),
          _index(__it._index),
          _block(__it._block),
          _array(__it._array) {}

    _base_iterator(_InfiType __a, size_type __n) : _array(__a) {
      _key = __GET_BLOCK_KEY(__n);
      _index = __GET_BLOCK_IDX(__n);
      _block = _array->_get_block(_key);
    }

    /**
     *  @brief  %_IteratorType assignment operator.
     *  @param  x  A %_IteratorType.
     */
    _IteratorType& operator=(const _IteratorType& __it) {
      _key = __it._key;
      _index = __it._index;
      _block = __it._block;
      _array = __it._array;
      return *this;
    }

    /**
     *  @brief  Prefix iterator increment operator
     *
     *  This moves iterator one memory cell forward prior to its evaluation.
     */
    _IteratorType& operator++() {
      _forward();
      return static_cast<_IteratorType&>(*this);
    }

    /**
     *  @brief  Prefix iterator decrement operator
     *
     *  This moves iterator one memory cell backward prior to its evaluation.
     */
    _IteratorType& operator--() {
      _backward();
      return static_cast<_IteratorType&>(*this);
    }

    /**
     *  @brief  Postfix iterator increment operator
     *
     *  This moves iterator one memory cell forward after its evaluation.
     */
    _IteratorType operator++(int) {
      _IteratorType __old(static_cast<_IteratorType&>(*this));
      _forward();
      return __old;
    }

    /**
     *  @brief  Postfix iterator decrement operator
     *
     *  This moves iterator one memory cell backward after its evaluation.
     */
    _IteratorType operator--(int) {
      _IteratorType __old(static_cast<_IteratorType&>(*this));
      _backward();
      return __old;
    }

    /**
     *  @brief  Iterator equality comparison.
     *  @param  it  A %_IteratorType.
     *  @return  True iff both iterators point on same memory cell.
    */
    bool operator==(const _IteratorType& __it) const {
      return (_key == __it._key) && (_index == __it._index);
    }

    /**
     *  @brief  Iterator inequality comparison.
     *  @param  it  A %_IteratorType.
     *  @return  False iff both iterators point on same memory cell.
    */
    bool operator!=(const _IteratorType& __it) const {
      return (_key != __it._key) || (_index != __it._index);
    }

   protected:
    /**  Move iterator fotward to next index.  */
    void _forward() {
      if (++_index >= _BLOCK_SIZE) {
        _index = 0;
        _key += _BLOCK_SIZE;
        _block = _array->_get_block(_key);
      }
    }

    /**  Move iterator back to previous index.  */
    void _backward() {
      if (_index == 0) {
        _index = _BLOCK_SIZE;
        _key -= _BLOCK_SIZE;
        _block = _array->_get_block(_key);
      }
      _index--;
    }

    size_type _key;
    size_type _index;

    _InfiType _array;
    _InfiArrayBlock _block;
    friend class InfiArray;
  };

  class const_iterator : public _base_iterator<true> {
   public:
    /**  Default constructor with no effect.  */
    const_iterator() : _base_iterator() {}

    /**
     *  @brief  %const_iterator copy constructor.
     *  @param  it  A %const_iterator.
     */
    const_iterator(const const_iterator& __it) : _base_iterator(__it) {}

    /**
     *  @brief  %const_iterator constructor.
     *  @param  m  Pointer to InfiArray memory map.
     *  @param  n  Index of iterator pointer.
     */
    const_iterator(const InfiArray* __a, size_type __n)
        : _base_iterator(__a, __n) {}

    /**  Const value access operator.  **/
    value_type operator*() const {
      if (_block == nullptr) return 0;
      return _block[_index];
    }
  };

  class iterator : public _base_iterator<false> {
   public:
    /**  Default constructor with no effect.  */
    iterator() : _base_iterator() {}

    /**
     *  @brief  %iterator copy constructor.
     *  @param  it  A %iterator.
     */
    iterator(const iterator& __it) : _base_iterator(__it) {}

    /**
     *  @brief  %iterator constructor.
     *  @param  m  Pointer to InfiArray memory map.
     *  @param  n  Index of iterator pointer.
     */
    iterator(InfiArray* __a, size_type __n) : _base_iterator(__a, __n) {}

    /**  Non-const reference access operator.  **/
    reference operator*() {
      if (_block == nullptr) _block = _array->_alloc_block(_key);
      return _block[_index];
    }
  };

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
    for (auto __b : _memory) delete[] __b.second;
    _memory.clear();
  }

  /**
   *  Returns a read/write iterator that points to the requested
   *  element in the %InfiArray. Iteration is done in ordinary element order.
   */
  iterator get_iterator(size_type __n) { return iterator(this, __n); }

  /**
   *  Returns a read-only (constant) iterator that points to the
   *  requested element in the %InfiArray. Iteration is done in ordinary
   *  element order.
   */
  const_iterator get_const_iterator(size_type __n) const {
    return const_iterator(this, __n);
  }

  /**
   *  @brief  Swaps data with another %InfiArray.
   *  @param  x  An %InfiArray.
   *
   *  This exchanges the elements between two InfiArrays in constant time.
   *  (Only using swap function of map objects.
   */
  void swap(InfiArray& __x) { _memory.swap(__x._memory); }

  /**
   *  Deallocates all zeroed out blocks. This can by pretty time consuming
   *  as the program needs to check all allocated blocks for nonzero elements.
   */
  void block_shrink() {
    for (auto it = _memory.begin(); it != _memory.end();) {
      if (!_test_block((*it).second)) {
        delete[](*it).second;
        _memory.erase(it++);
      } else {
        it++;
      }
    }
  }

  /**  Returns the number of memory blocks in the %InfiArray.  */
  size_type block_count() const { return _memory.size(); }

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
  value_type operator[](size_type __n) const { return this->get(__n); }

  /**
   *  @brief  Subscript access to the data contained in the %InfiArray
   *  @param n The index of the element for which data should be accessed.
   *  @return  Read/write reference to data.
   *
   *  This operator allows for easy, array-style, data access.
   *  If requested block does not exists, it will be allocated.
   */
  reference operator[](size_type __n) {
    if (!_block_existence(__n)) _alloc_block(__n);
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
    if (!_block_existence(__n)) return 0;
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
    auto __i = _memory.find(__key);

    if (__i == _memory.end()) return nullptr;
    return (*__i).second;
  }

  /**
   *  @brief  Allocate new memory block at given index.
   *  @param  key  An index of requested memory.
   *
   *  A key can be any value for given memory block as it is rouded down
   *  to correct memory block address.
  */
  value_type* _alloc_block(size_type __key) {
    if (_block_existence(__key)) throw;
    value_type* __block = new value_type[_BLOCK_SIZE]();
    _memory[__GET_BLOCK_KEY(__key)] = __block;
    return __block;
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
    if (__block == nullptr) return false;

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

  infi_map _memory;
};

#endif /* _INFI_ARRAY_H_ */
