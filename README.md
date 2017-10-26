# InfiArray

Implementation of unbounded array in C++11.

Array can be indexed with any positive and negative number.

It's pretty memory efficient since it does not allocate memory before it is really needed. Because whole memory is initialized to zero, there is no need to allocate anything during reading operations. To make sure that no memory will be alocated, use `get()` function instead of array subscript operator `[]`.
