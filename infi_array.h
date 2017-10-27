#ifndef _INFI_ARRAY_H_
#define _INFI_ARRAY_H_

#define _BLOCK_SIZE 2048

#define _LOWER_MASK (_BLOCK_SIZE - 1)
#define _UPPER_MASK (~_LOWER_MASK)

#define __GET_BLOCK_KEY(x) (x & _UPPER_MASK)
#define __GET_BLOCK_IDX(x) (x & _LOWER_MASK)

#include <cstring>
#include <cstdio>
#include <unordered_map>

class InfiArray {
 public:
  typedef int32_t value_type;

  typedef size_t size_type;
  typedef size_t difference_type;

  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;

  typedef std::unordered_map<size_type, value_type*> infi_map;

  class const_iterator {
   public:
    typedef InfiArray::pointer pointer;
    typedef InfiArray::reference reference;
    typedef InfiArray::value_type value_type;
    typedef InfiArray::difference_type difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;

    /**  Default constructor with no effect.  */
    const_iterator() {}

    /**
     *  @brief  %const_iterator copy constructor.
     *  @param  x  A %const_iterator.
     */
    const_iterator(const const_iterator& __it)
        : _key(__it._key),
          _index(__it._index),
          _iter(__it._iter),
          _memory(__it._memory) {}

    /**
     *  @brief  %InfiArray constructor.
     *  @param  m  Pointer to InfiArray memory map.
     *  @param  n  Index of iterator pointer.
     */
    const_iterator(const infi_map* __m, size_type __n) : _memory(__m) {
      _key = __GET_BLOCK_KEY(__n);
      _index = __GET_BLOCK_IDX(__n);
      _iter = _memory->find(_key);
    }

    /**  Const value access operator.  */
    value_type operator*() const {
      if (_iter == _memory->end()) return 0;
      return (*_iter).second[_index];
    }

    /**
     *  @brief  Prefix iterator increment operator
     *
     *  This moves iterator one memory cell forward prior to its evaluation.
     */
    const_iterator& operator++() {
      _forward();
      return *this;
    }

    /**
     *  @brief  Postfix iterator increment operator
     *
     *  This moves iterator one memory cell forward after its evaluation.
     */
    const_iterator operator++(int) {
      const_iterator __old = *this;
      _forward();
      return __old;
    }

    /**
     *  @brief  Prefix iterator decrement operator
     *
     *  This moves iterator one memory cell backward prior to its evaluation.
     */
    const_iterator& operator--() {
      _backward();
      return *this;
    }

    /**
     *  @brief  Postfix iterator decrement operator
     *
     *  This moves iterator one memory cell backward after its evaluation.
     */
    const_iterator operator--(int) {
      const_iterator __old = *this;
      _backward();
      return __old;
    }

    /**
     *  @brief  %const_iterator assignment operator.
     *  @param  x  A %const_iterator.
     */
    const_iterator& operator=(const const_iterator& __it) {
      _key = __it._key;
      _index = __it._index;
      _iter = __it._iter;
      _memory = __it._memory;
      return *this;
    }

    /**
     *  @brief  Iterator equality comparison.
     *  @param  it  A %const_iterator.
     *  @return  True iff both iterators point on same memory cell.
    */
    bool operator==(const const_iterator& __it) const {
      return (_index == __it._index) && (_key == __it._key);
    }

    /**
     *  @brief  Iterator inequality comparison.
     *  @param  it  A %const_iterator.
     *  @return  False iff both iterators point on same memory cell.
    */
    bool operator!=(const const_iterator& __it) const {
      return (_index != __it._index) || (_key != __it._key);
    }

   private:
    /**  Move iterator fotward to next index.  */
    void _forward() {
      if (++_index >= _BLOCK_SIZE) {
        _index = 0;
        _key += _BLOCK_SIZE;
        _iter = _memory->find(_key);
      }
    }

    /**  Move iterator back to previous index.  */
    void _backward() {
      if (_index == 0) {
        _index = _BLOCK_SIZE;
        _key -= _BLOCK_SIZE;
        _iter = _memory->find(_key);
      }
      _index--;
    }

    size_type _key;
    size_type _index;

    infi_map::const_iterator _iter;
    const infi_map* _memory;
    friend class InfiArray;
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
   *  Returns a read-only (constant) iterator that points to the
   *  requested element in the %InfiArray. Iteration is done in ordinary
   *  element order.
   */
  const_iterator get_iterator(size_type __n) const {
    return const_iterator((&_memory), __n);
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
  void _alloc_block(size_type __key) {
    if (_block_existence(__key)) throw;
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
