# InfiArray

Implementation of unbounded array in C++11.

Array can be indexed with any positive and negative number.

It's pretty memory efficient since it does not allocate memory before it is really needed. Because whole memory is initialized to zero, there is no need to allocate anything during reading operations. To make sure that no memory will be alocated, use `get()` function instead of array subscript operator `[]`.

Structure implements both const and nonconst iterator. Const iterator can be used for non-allocating read and it's by far the most efficient way. Non-const iterator should also be faster then array access operator (`[]`). Iterator doesn't have to search memory map for each new access and because of that they should be used for iterating (which is obvious).
