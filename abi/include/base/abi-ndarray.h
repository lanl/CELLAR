#ifndef ABI_NDARRAY_H
#define ABI_NDARRAY_H

#include <stddef.h>

#define ABI_NDARRAY_MAX_RANK 8

typedef struct _abi_nd_array_t {
    size_t rank;
    size_t dims[ABI_NDARRAY_MAX_RANK];
    size_t strides[ABI_NDARRAY_MAX_RANK];
    void *data;
} abi_ndarray_t;

#endif // ABI_NDARRAY_H