/**
 * @file comm-ffi-internal-patterns.hpp
 *
 * @brief Supports the FFI layer to eap::comm patterns
 * @date 2019-01-15
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_COMM_FFI_INTEROP_INTERNAL_PATTERNS_HPP_
#define EAP_COMM_FFI_INTEROP_INTERNAL_PATTERNS_HPP_

// STL Includes
#include <iostream>

// Internal Includes
#include <comm-patterns.hpp>
#include <utility-kokkos.hpp>

// Local Includes
#include <comm-ffi-types.h>

namespace eap {
namespace comm {
namespace ffi {
class RmaAllToAllGeneric {
    mpi::Comm comm;
    eap_comm_datatype_t datatype_;
    void *rma_all_to_all = nullptr;

  public:
    RmaAllToAllGeneric(MPI_Comm comm, eap_comm_datatype_t datatype, int count);
    ~RmaAllToAllGeneric();

    eap_comm_datatype_t datatype() const { return datatype_; }

    void invoke(abi_ndarray_t const *input, abi_ndarray_t *output);

    template <typename Fn>
    void use_concrete(Fn kernel) {
        switch (datatype()) {
        case eap_comm_datatype_int32:
            kernel(reinterpret_cast<eap::comm::RmaAllToAll<int> *>(rma_all_to_all));
            break;
        default:
            std::cerr << comm.rank() << ": eap_comm_datatype_t " << datatype()
                      << " is not compatible with comm_rma_all_to_all." << std::endl;
            comm.abort(EXIT_FAILURE);
        }
    }

    eap::comm::RmaAllToAll<int> *unwrap_int() {
        if (datatype() != eap_comm_datatype_int32) {
            std::cerr << comm.rank()
                      << ": Tried to unwrap RmaAllToAllGeneric as an int, but it was datatype "
                      << datatype() << std::endl;
            comm.abort(EXIT_FAILURE);
        }
        return reinterpret_cast<eap::comm::RmaAllToAll<int> *>(rma_all_to_all);
    }
};
} // namespace ffi
} // namespace comm
} // namespace eap

#endif // EAP_COMM_FFI_INTEROP_INTERNAL_PATTERNS_HPP_