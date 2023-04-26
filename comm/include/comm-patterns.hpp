/**
 * @file comm-patterns.hpp
 *
 * @brief EAP Communication patterns
 * @date 2018-05-22
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_COMM_PATTERNS_HPP_
#define EAP_COMM_PATTERNS_HPP_

// STL includes
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <type_traits>

// External includes
#include <Kokkos_Core.hpp>
#include <mpi/mpi.hpp>
#include <span>

// Internal package includes
#include <error-macros.hpp>
#include <utility-addressing.hpp>
#include <utility-format.hpp>
#include <utility-memory.hpp>

// Package includes
#include "comm-internal-typestr.hpp"
#include "comm-reserved_tags.hpp"
#include "comm-timer.hpp"

#define COMM_PATTERNS_INTERNAL_CHECK_IS_COMM_SIZE(arg, comm)                                       \
    {                                                                                              \
        if (arg.size() != comm.size()) {                                                           \
            std::stringstream ss;                                                                  \
            ss << "Argument error: " #arg " was size " << arg.size()                               \
               << ", but must be the same size as comm.size() = " << comm.size();                  \
            throw std::invalid_argument(ss.str());                                                 \
        }                                                                                          \
    }

namespace eap {
namespace comm {
template <typename T, typename = std::enable_if_t<mpi::is_datatype_v<T>>>
class RmaAllToAll {
  public:
    using value_type = T;

    RmaAllToAll(mpi::Comm comm, int count_ = 1)
        : comm_(comm),
          count_(count_),
          win_(mpi::UniqueWin<T>::allocate(comm, comm.size() * count_)) {
        std::fill(win_.base(), win_.base() + this->comm_.size() * count(), 0);
        this->win_.lock_all(mpi::WinLockAssertFlags::NoCheck);
        barrier_ = this->comm_.immediate_barrier();
    }

    RmaAllToAll(RmaAllToAll const &) = delete;
    RmaAllToAll &operator=(RmaAllToAll const &) = delete;

    RmaAllToAll(RmaAllToAll &&) = default;
    RmaAllToAll &operator=(RmaAllToAll &&) = default;

    ~RmaAllToAll() {
        if (barrier_) {
            barrier_.wait();
        }

        if (win_) {
            win_.unlock_all();
        }
    }

    int count() const { return count_; }

    template <typename KLayout>
    void AllToAll(Kokkos::View<T const *, KLayout, eap::HostMemorySpace> const &send,
                  Kokkos::View<T *, KLayout, eap::HostMemorySpace> &recv) {
        using eap::utility::StringJoinView;
        EE_DIAG_PRE

        EAP_COMM_TIME_FUNCTION("eap::comm::RmaAllToAll<" +
                               std::string(internal::type_to_str<T>::name()) + ">::AllToAll");

        EE_ASSERT(send.size() >= (size_t)(comm_.size() * count()),
                  "send_count (" << send.size()
                                 << ") must be at least as large as Comm_size * count ("
                                 << comm_.size() * count() << ")");

        EE_ASSERT(recv.size() >= (size_t)(comm_.size() * count()),
                  "recv_count (" << recv.size()
                                 << ") must be at least as large as Comm_size * count ("
                                 << comm_.size() * count() << ")");

        barrier_.wait();

        for (mpi::rank_t pe = 0; pe < comm_.size(); pe++) {
            bool all_zero = true;
            for (auto i = pe * count(); i < (pe + 1) * count(); i++) {
                if (send(i) != 0) {
                    all_zero = false;
                    break;
                }
            }

            if (!all_zero) {
                for (auto i = 0; i < count(); i++) {
                    win_.put(&send(pe * count() + i), 1, pe, comm_.rank() * count() + i);
                }
            }
        }

        win_.flush_all();

        comm_.barrier();

        auto const base = win_.base();

        for (auto i = 0; i < comm_.size() * count(); i++) {
            recv(i) = base[i];
            base[i] = 0;
        }

        barrier_ = comm_.immediate_barrier();

        EE_DIAG_POST_MSG("send = [" << StringJoinView(send, ", ") << "], recv = ["
                                    << StringJoinView(recv, ", ") << "]")
    }

    void AllToAll(T const send[], size_t send_count, T recv[], size_t recv_count) {
        Kokkos::View<T const *, Kokkos::LayoutRight, eap::HostMemorySpace> send_view =
            Kokkos::View<T const *, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>(send,
                                                                                   send_count);

        Kokkos::View<T *, Kokkos::LayoutRight, eap::HostMemorySpace> recv_view =
            Kokkos::View<T *, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>(recv, recv_count);

        AllToAll(send_view, recv_view);
    }

    std::vector<T> AllToAll(std::vector<T> const &send) {
        std::vector<T> recv(comm_.size() * count());
        AllToAll(send.data(), send.size(), recv.data(), recv.size());
        return recv;
    }

  private:
    mpi::Comm comm_;
    int count_;
    mpi::UniqueWin<T> win_;
    mpi::UniqueRequest barrier_;
};

template <typename T, typename = std::enable_if_t<mpi::is_datatype_v<T>>>
std::vector<T> SomeToSome(mpi::Comm &comm,
                          std::vector<T> const &send_array,
                          std::vector<int> const &to_pes,
                          std::vector<int> const &from_pes) {
    using eap::utility::StringJoin;

    EE_DIAG_PRE

    EAP_COMM_TIME_FUNCTION("eap::comm::SomeToSome<" +
                           std::string(internal::type_to_str<T>::name()) + ">");

// HACK: CUDA messes this up prior to passing to gcc - disable for now.
#ifndef __NVCC__
    EE_ASSERT_EQ((size_t)comm.size(), send_array.size());
    EE_ASSERT_EQ((size_t)comm.size(), to_pes.size());
    EE_ASSERT_EQ((size_t)comm.size(), from_pes.size());
#endif

    std::vector<T> recv_array(comm.size());

    {
        size_t const num_sends =
            std::count_if(to_pes.begin(), to_pes.end(), [](int b) { return b != 0; });
        size_t const num_recvs =
            std::count_if(from_pes.begin(), from_pes.end(), [](int b) { return b != 0; });

        std::vector<mpi::UniqueRequest> requests;
        requests.reserve(num_sends + num_recvs);

        for (mpi::rank_t rank = 0; rank < comm.size(); rank++) {
            if (from_pes[rank]) {
                requests.push_back(comm.immediate_recv(recv_array[rank], rank, SOME_TO_SOME_TAG));
            }
        }

        for (mpi::rank_t rank = 0; rank < comm.size(); rank++) {
            if (to_pes[rank]) {
                requests.push_back(comm.immediate_send(send_array[rank], rank, SOME_TO_SOME_TAG));
            }
        }

        mpi::wait_all(requests);
    }

    return recv_array;

    EE_DIAG_POST_MSG("send_array = [" << StringJoin(send_array, ", ") << "], to_pes = ["
                                      << StringJoin(to_pes, ", ") << "], from_pes = ["
                                      << StringJoin(from_pes, ", ") << "]")
}

/**
 * @brief
 * An implementation of MPI_Alltoallv - sends varying slices of a buffer to many ranks.
 *
 * `send_data` is paritioned by `send_start` and `send_length`, where `send_start[i]` is the offset
 * of the data to send to rank `i` and `send_length[i]` is the length of the buffer that should be
 * send to rank `i`.
 *
 * `send_data` is paritioned by `send_start` and `send_length`, where `recv_start[i]` is the offset
 * of the buffer that receives the data from rank `i` and `recv_length[i]` is the length of the
 * buffer.
 *
 * @tparam T An MPI-compatible Datatype
 * @param comm Communicator to use.
 * @param send_start Must be of length `comm.size()`.
 * @param send_length Must be of length `comm.size()`.
 * @param send_data Must be large enough to contain all buffers described by `send_start` and
 * `send_length`
 * @param recv_start Must be of length `comm.size()`.
 * @param recv_length Must be of length `comm.size()`.
 * @param recv_data Must be large enough to contain all buffers described by `recv_start` and
 * `recv_length`
 * @param indexing Indicates what indexing method that `send_start` and `recv_start` use
 */
template <typename T, typename = std::enable_if_t<mpi::is_datatype_v<T>>>
void Move(mpi::Comm comm,
          std::span<FortranLocalIndex const> send_start,
          std::span<local_index_t const> send_length,
          std::span<T const> send_data,
          std::span<FortranLocalIndex const> recv_start,
          std::span<local_index_t const> recv_length,
          std::span<T> recv_data) {
    using eap::utility::StringJoin;
    using mpi::rank_t;

    EE_DIAG_PRE

    EAP_COMM_TIME_FUNCTION("eap::comm::Move");

    // Validate inputs
    // HACK: CUDA messes this up prior to passing to gcc - disable for now.
#ifndef __NVCC__
    EE_ASSERT_EQ(comm.size(), send_start.size());
    EE_ASSERT_EQ(comm.size(), send_length.size());
    EE_ASSERT_EQ(comm.size(), recv_start.size());
    EE_ASSERT_EQ(comm.size(), recv_length.size());
#endif

    EE_ASSERT(!(*send_data.begin() < *recv_data.end() && *recv_data.begin() < *send_data.end()),
              "recv_data must not overlap with send_data");

    rank_t const my_pe = comm.rank();

// HACK: CUDA messes this up prior to passing to gcc - disable for now.
#ifndef __NVCC__
    EE_ASSERT_EQ(
        recv_length[my_pe], send_length[my_pe], "Send did not equal receive for rank " << my_pe);
#endif

    // Count how many requests we'll issue
    size_t const num_sends =
        std::count_if(send_length.begin(), send_length.end(), [](auto len) { return len > 0; });

    size_t const num_recvs =
        std::count_if(recv_length.begin(), recv_length.end(), [](auto len) { return len > 0; });

    // Reserve the amount of memory needed for requests up front
    std::vector<mpi::UniqueRequest> requests;
    requests.reserve(num_sends + num_recvs);

    // Issue receives
    for (rank_t pe = 0; pe < comm.size(); pe++) {
        if (pe != my_pe && recv_length[pe] > 0) {
            requests.push_back(
                comm.immediate_recv(&recv_data[recv_start[pe]], recv_length[pe], pe, MOVE_TAG));
        }
    }

    // Issue sends
    for (rank_t pe = 0; pe < comm.size(); pe++) {
        if (pe != my_pe && send_length[pe] > 0) {
            requests.push_back(
                comm.immediate_send(&send_data[send_start[pe]], send_length[pe], pe, MOVE_TAG));
        }
    }

    // Copy local buffers
    if (send_length[my_pe] > 0) {
        std::span<T const> const send_sub =
            send_data.subspan(send_start[my_pe], send_length[my_pe]);

        std::span<T> const recv_sub = recv_data.subspan(recv_start[my_pe], recv_length[my_pe]);

        std::copy(send_sub.begin(), send_sub.end(), recv_sub.begin());
    }

    // Synchronize
    mpi::wait_all(requests);

    EE_DIAG_POST_MSG("send_start = [" << StringJoin(send_start, ", ") << "], send_length = ["
                                      << StringJoin(send_length, ", ") << "], send_data = ["
                                      << StringJoin(send_data, ", ") << "], recv_start = ["
                                      << StringJoin(recv_start, ", ") << "], recv_length = ["
                                      << StringJoin(recv_length, ", ") << "], recv_data = ["
                                      << StringJoin(recv_data, ", ") << "]")
}
} // namespace comm
} // namespace eap

#endif // EAP_COMM_PATTERNS_HPP_