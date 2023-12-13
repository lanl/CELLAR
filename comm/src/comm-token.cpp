/**
 * @file comm-token.cpp
 *
 * @brief Implementations of Token communication pattern
 * @date 2018-05-22
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Matching header include
#include <comm-token.hpp>

// STL Includes
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <utility>

// Internal Includes
#include <comm-patterns.hpp>
#include <error.hpp>
#include <utility-addressing.hpp>
#include <utility-algorithm.hpp>
#include <utility-diagnostic.hpp>

// Local Includes
#include <comm-patterns.hpp>

using namespace eap::comm;

using eap::global_index_t;
using eap::local_index_t;
using eap::utility::NonNegativeInteger;
using eap::utility::nullint;
using eap::utility::OptionalInteger;
using mpi::rank_t;
using mpi::tag_t;
using std::move;
using std::ptrdiff_t;
using std::size_t;
using std::uint32_t;
using std::uint64_t;
using std::vector;

// Explicit instantiations
TOKEN_INSTANTIATE_TOKEN_GS_UNIT(double);
TOKEN_INSTANTIATE_TOKEN_GS_UNIT(float);

TOKEN_INSTANTIATE_TOKEN_GS_V(double, Kokkos::LayoutLeft);
TOKEN_INSTANTIATE_TOKEN_GS_V(double, Kokkos::LayoutRight);
TOKEN_INSTANTIATE_TOKEN_GS_V(double, Kokkos::LayoutStride);

TOKEN_INSTANTIATE_TOKEN_GS_V(float, Kokkos::LayoutLeft);
TOKEN_INSTANTIATE_TOKEN_GS_V(float, Kokkos::LayoutRight);
TOKEN_INSTANTIATE_TOKEN_GS_V(float, Kokkos::LayoutStride);

namespace {
struct AwayCountAndSize {
    size_t count;
    size_t size;
};

AwayCountAndSize NumAwayAndSize(rank_t mype, vector<int32_t> const &counts) {
    size_t count = 0;
    size_t size = 0;

    for (rank_t pe = 0; pe < (rank_t)counts.size(); pe++) {
        if (pe != mype && counts[pe] > 0) {
            count++;
            size += counts[pe];
        }
    }

    return AwayCountAndSize{count, size};
}
} // namespace

void internal::BuildGlobalBase(mpi::Comm comm,
                               uint32_t num_local_cell,
                               vector<uint32_t> &num_cells,
                               vector<uint64_t> &bases) {

    EE_PRELUDE

    EE_CHECK(num_cells.resize(comm.size()),
             "Could not reserve space in num_cells for " << comm.size() << " elements");
    EE_CHECK(bases.resize(comm.size()),
             "Could not reserve space in bases for " << comm.size() << " elements");

    comm.all_gather(num_local_cell, std::span<uint32_t>(num_cells));

    Kokkos::
        View<uint32_t const *, Kokkos::LayoutLeft, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>
            num_cells_view(num_cells.data(), num_cells.size());
    Kokkos::View<uint64_t *, Kokkos::LayoutLeft, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>
        bases_view(bases.data(), bases.size());

    eap::utility::ExclusiveSumScanHost(num_cells_view, bases_view);
}

internal::PeCellAddress internal::PeAndLocalAddress(vector<global_index_t> const &rank_bases,
                                                    global_index_t global_address) {
    auto it = std::lower_bound(rank_bases.begin(), rank_bases.end(), global_address);

    if (it == rank_bases.end() || *it > global_address) {
        it--;
    }

    auto rank = static_cast<rank_t>(it - rank_bases.begin());

    while (rank != (rank_t)rank_bases.size() - 1 && rank_bases[rank] == rank_bases[rank + 1]) {
        rank++;
    }

    assert(it >= rank_bases.begin());

    return PeCellAddress{rank, static_cast<local_index_t>(global_address - *it)};
}

internal::CopyFromTo
internal::BuildCopyInfo(rank_t mype,
                        std::span<FortranLocalIndex const> home_addresses,
                        std::span<utility::NonNegativeInteger<mpi::rank_t> const> away_pe,
                        std::span<OptionalFortranLocalIndex const> away_address) {
    EE_PRELUDE

    assert(home_addresses.size() == away_address.size());
    assert(away_pe.size() == away_address.size());

    vector<size_t> copy_from, copy_to, zero;

    try {
        for (ptrdiff_t i = 0; i < away_pe.size(); i++) {
            auto const away = away_address[i];
            auto const home = home_addresses[i];

            if (!away) {
                zero.push_back(home);
            } else if (mype == *away_pe[i]) {
                copy_from.push_back(*away);
                copy_to.push_back(home);
            }
        }
    } catch (...) {
        EE_RAISE("The away addresses array was too large (size "
                 << away_pe.size() << "). Could not allocate enough memory for this Token");
    }

    return {move(copy_from), move(copy_to), move(zero)};
}

size_t internal::RecvScratchArraySize(uint32_t desired_max_gs_recv_size,
                                      uint32_t unit_size,
                                      uint32_t row_size,
                                      vector<Segment> const &segments) {
    auto total_recv_count = std::accumulate(
        segments.begin(),
        segments.end(),
        size_t(0),
        [](size_t sum, internal::Segment const &segment) { return sum + segment.length; });

    size_t max_recv_size =
        desired_max_gs_recv_size / unit_size + ((desired_max_gs_recv_size % unit_size) != 0);

    // increase the max_recv_size to the max size of any individual segment
    for (auto &segment : segments) {
        max_recv_size = std::max(max_recv_size, row_size * segment.length);
    }

    return std::min(total_recv_count * row_size, max_recv_size);
}

internal::RecvRequestCompletionStateMachine::RecvRequestCompletionStateMachine(
    std::vector<mpi::UniqueRequest> &recv_requests, bool require_default_order)
    : recv_requests_(recv_requests), require_default_order_(require_default_order) {
    assert(std::none_of(recv_requests_.begin(),
                        recv_requests_.end(),
                        [](mpi::UniqueRequest const &request) { return request.is_null(); }));
}

bool internal::RecvRequestCompletionStateMachine::get_next_requests(std::vector<int> &completed) {
    if (num_completed_ == (int)recv_requests_.size()) return false;

    completed.clear();

    if (require_default_order_) {
        while (completed.empty()) {
            mpi::wait_some_into(recv_requests_, requests_completed_);

            for (; num_completed_ != (int)recv_requests_.size() &&
                   recv_requests_[num_completed_].is_null();
                 num_completed_++) {
                completed.push_back(num_completed_);
            }

            requests_completed_.clear();
        }
    } else {
        mpi::wait_some_into(recv_requests_, completed);
        num_completed_ += completed.size();
    }

    return true;
}

TokenBuilder TokenBuilder::FromComm(mpi::Comm comm) { return TokenBuilder{comm}; }

void TokenBuilder::SetNumCells(uint32_t num_local_cell) {
    internal::BuildGlobalBase(comm_, num_local_cell, num_cells_, bases_);

    // num_cells_ not used for any more than pre-allocated memory
    num_cells_.clear();
}

void TokenBuilder::SetCellBases(std::span<uint64_t const> bases) {
    EE_PRELUDE

    EE_ASSERT_EQ(comm_.size(), bases.size());

    // Don't need to clear the array - it will only be written to.
    bases_.resize(comm_.size());
    std::copy(bases.begin(), bases.end(), bases_.begin());
}

void TokenBuilder::UseRmaAllToAll(RmaAllToAll<std::int32_t> *rma) {
    if (rma->count() != 1) {
        std::cerr << comm_.rank()
                  << ": TokenBuilder::UseRmaAllToAll: Only RmaAllToAll with count 1 can be "
                     "used by TokenBuilder. You passed an RmaAllToAll with a count of "
                  << rma->count() << "." << std::endl;
    }

    rma_ = rma;
}

void TokenBuilder::RequireRankOrderRequestCompletion(bool require_rank_order_completion) {
    require_rank_order_completion_ = require_rank_order_completion;
}

void TokenBuilder::SetToPes(std::vector<int> &&to_pes) {
    auto from_pes = rma_ ? rma_->AllToAll(to_pes) : comm_.all_to_all(to_pes);
    SetToAndFromPes(std::move(to_pes), std::move(from_pes));
}

void TokenBuilder::SetToAndFromPes(std::vector<int> &&to_pes, std::vector<int> &&from_pes) {
    if (to_pes.size() != (size_t)comm_.size() || from_pes.size() != (size_t)comm_.size()) {
        std::cerr << comm_.rank() << ": to_pes and from_pes must be of size MPI_Comm_size"
                  << std::endl;
        comm_.abort(EXIT_FAILURE);
    }

    to_pes_ = std::move(to_pes);
    from_pes_ = std::move(from_pes);
}

void TokenBuilder::PesAndAddresses(std::span<OptionalFortranGlobalIndex const> away_globals,
                                   std::span<utility::NonNegativeInteger<mpi::rank_t>> pes,
                                   std::span<OptionalFortranLocalIndex> addresses) const {
    assert(away_globals.size() == pes.size());
    assert(away_globals.size() == addresses.size());

    for (ptrdiff_t i = 0; i < away_globals.size(); i++) {
        if (away_globals[i]) {
            auto const away = internal::PeAndLocalAddress(bases_, *away_globals[i]);

            pes[i] = away.rank;
            addresses[i] = away.local_address;
        } else {
            pes[i] = nullint;
            addresses[i] = nullint;
        }
    }
}

void TokenBuilder::PesAndAddresses(std::span<OptionalFortranGlobalIndex const> away_globals,
                                   vector<NonNegativeInteger<mpi::rank_t>> &pes,
                                   vector<OptionalFortranLocalIndex> &addresses) const {
    EE_PRELUDE

    try {
        pes.resize(away_globals.size());
        addresses.resize(away_globals.size());
    } catch (...) {
        EE_RAISE("away_globals.size() (" << away_globals.size()
                                         << ") too large - cannot allocate buffers");
    }

    PesAndAddresses(std::span<OptionalFortranGlobalIndex const>(away_globals),
                    std::span<NonNegativeInteger<rank_t>>(pes),
                    std::span<OptionalFortranLocalIndex>(addresses));
}

void TokenBuilder::FlagPes(std::span<OptionalFortranGlobalIndex const> away_globals,
                           std::span<int> pe_flags) const {
    assert(pe_flags.size() == comm_.size());
    MARK_UNUSED(pe_flags.size());

    for (ptrdiff_t i = 0; i < away_globals.size(); i++) {
        if (away_globals[i]) {
            auto const away = internal::PeAndLocalAddress(bases_, *away_globals[i]);

            // set the flag
            pe_flags[away.rank] = 1;
        }
    }
}

Token TokenBuilder::BuildGlobal(std::span<FortranLocalIndex const> home_addresses,
                                std::span<OptionalFortranGlobalIndex const> away_global) {
    EAP_COMM_TIME_FUNCTION("eap::comm::TokenBuilder::BuildGlobal");

    EE_DIAG_PRE

    EE_ASSERT(!bases_.empty(),
              "Tried to build a global token before calling TokenBuilder::SetNumCells or "
              "TokenBuilder::SetAllNumCells.");

    vector<NonNegativeInteger<rank_t>> away_pe;
    vector<OptionalFortranLocalIndex> away_address;
    PesAndAddresses(away_global, away_pe, away_address);

    return BuildLocal(home_addresses, away_pe, away_address);

    EE_DIAG_POST
}

Token TokenBuilder::BuildLocal(std::span<FortranLocalIndex const> home_addresses,
                               std::span<NonNegativeInteger<mpi::rank_t> const> away_pe,
                               std::span<OptionalFortranLocalIndex const> away_address) {
    EAP_COMM_TIME_FUNCTION("eap::comm::TokenBuilder::BuildLocal");

    EE_DIAG_PRE

    EE_ASSERT_EQ(away_address.size(), away_pe.size());
    EE_ASSERT_EQ(home_addresses.size(),
                 away_address.size(),
                 "The home_addresses array must be a mapping of the away_address to the "
                 "local index. Therefore they must be the same size.");

    auto const mype = comm_.rank();

    vector<int32_t> count_move_to(comm_.size(), 0);
    for (ptrdiff_t i = 0; i < away_pe.size(); i++) {
        if (away_address[i]) {
            count_move_to[*away_pe[i]] += 1;
        }
    }

    count_move_to[mype] = 0;

    auto copy_info = internal::BuildCopyInfo(mype, home_addresses, away_pe, away_address);

    auto const count_get_from =
        (to_pes_.empty()) ? (rma_ ? rma_->AllToAll(count_move_to) : comm_.all_to_all(count_move_to))
                          : SomeToSome(comm_, count_move_to, to_pes_, from_pes_);

    auto const home_counts = NumAwayAndSize(mype, count_move_to);

    vector<internal::Segment> home_segments;
    EE_CHECK(home_segments.reserve(home_counts.count),
             "Could not reserve space in home_segments for " << home_counts.count << " Segments");

    {
        size_t low = 0;
        for (rank_t pe = 0; pe < (rank_t)count_move_to.size(); pe++) {
            if (pe != mype && count_move_to[pe] > 0) {
                home_segments.push_back({pe, low, static_cast<size_t>(count_move_to[pe])});
                low += count_move_to[pe];
            }
        }
    }

    auto const away_counts = NumAwayAndSize(mype, count_get_from);

    vector<internal::Segment> away_segments;
    EE_CHECK(away_segments.reserve(away_counts.count),
             "Could not reserve space in away_segments for " << away_counts.count << " Segments");

    {
        size_t low = 0;
        for (rank_t pe = 0; pe < (rank_t)count_get_from.size(); pe++) {
            if (pe != mype && count_get_from[pe] > 0) {
                away_segments.push_back({pe, low, static_cast<size_t>(count_get_from[pe])});
                low += count_get_from[pe];
            }
        }
    }

    // global_index contains the list of away-local addresses this rank needs from each other
    // rank. It is subdivided by home_segments - the home_segment for rank 2 indicates which slice
    // of the global_index array contains the indices this rank needs from it.
    auto global_index =
        EE_CHECK(vector<local_index_t>(home_counts.size, 0),
                 "Could not allocate global_index with " << home_counts.size << " addresses");

    // home_index contains the list of indices in an intermediate buffer (output from a Get() call,
    // input to a Put() call) that received data is mapped to. e.g. Data received from
    // away_address[i] is mapped to output[home_index[i]]. Essentially it maps the data from the
    // global address space into a flat buffer.
    auto home_index =
        EE_CHECK(vector<local_index_t>(home_counts.size),
                 "Could not allocate home_index with " << home_counts.size << " addresses");

    {
        auto away_low = EE_CHECK(vector<size_t>(comm_.size(), 0),
                                 "Could not allocate away_low with " << comm_.size() << " indices");
        for (auto &segment : home_segments) {
            away_low[segment.rank] = segment.begin;
        }

        for (ptrdiff_t i = 0; i < away_pe.size(); i++) {
            if (away_address[i] && *away_pe[i] != mype) {
                auto const low = away_low[*away_pe[i]]++;
                global_index[low] = *away_address[i];
                home_index[low] = home_addresses[i];
            }
        }
    }

    // away_index contains the list of local addresses that other ranks need from this rank.
    // It is subdivided by away_segments.
    auto away_index =
        EE_CHECK(vector<local_index_t>(away_counts.size),
                 "Could not allocate away_index with " << away_counts.size << " addresses");

    {
        vector<mpi::UniqueRequest> requests;
        EE_CHECK(requests.reserve(home_segments.size() + away_segments.size()),
                 "Could not allocate requests with "
                     << (home_segments.size() + away_segments.size()) << " Requests");

        for (auto &segment : home_segments) {
            assert(segment.rank != mype);
            requests.push_back(comm_.immediate_send(
                &global_index[segment.begin], segment.length, segment.rank, BUILD_GLOBAL_TAG));
        }

        for (auto &segment : away_segments) {
            assert(segment.rank != mype);
            requests.push_back(comm_.immediate_recv(
                &away_index[segment.begin], segment.length, segment.rank, BUILD_GLOBAL_TAG));
        }

        mpi::wait_all(requests);
    }

    auto const max_home_addr = std::max_element(home_addresses.begin(), home_addresses.end());

    auto const minimum_gather_size = max_home_addr == home_addresses.end() ? 0 : *max_home_addr + 1;

    auto const max_local_away_addr =
        std::max_element(copy_info.copy_from.begin(), copy_info.copy_from.end());

    auto const max_remote_away_addr = std::max_element(away_index.begin(), away_index.end());

    size_t minimum_scatter_size = 0;
    if (max_local_away_addr != copy_info.copy_from.end()) {
        minimum_scatter_size = *max_local_away_addr + 1;
    }

    if (max_remote_away_addr != away_index.end()) {
        minimum_scatter_size =
            std::max(minimum_scatter_size, static_cast<size_t>(*max_remote_away_addr + 1));
    }

    std::vector<int> send_procs;
    std::vector<int> recv_procs;

    for(size_t idx = 0; idx < away_segments.size(); idx++)
        recv_procs.push_back(away_segments[idx].rank);

    for(size_t idx = 0; idx < home_segments.size(); idx++)
        send_procs.push_back(home_segments[idx].rank);

    MPI_Comm new_comm;
    MPI_Dist_graph_create_adjacent(comm_, recv_procs.size(), recv_procs.data(), MPI_UNWEIGHTED,
                                   send_procs.size(), send_procs.data(), MPI_UNWEIGHTED,
                                   MPI_INFO_NULL, false, &new_comm);
    MPI_Comm new_comm_flipped;
    MPI_Dist_graph_create_adjacent(comm_, send_procs.size(), send_procs.data(), MPI_UNWEIGHTED,
                                   recv_procs.size(), recv_procs.data(), MPI_UNWEIGHTED,
                                   MPI_INFO_NULL, false, &new_comm_flipped);

    return Token(comm_,
                 new_comm,
                 new_comm_flipped,
                 minimum_gather_size,
                 minimum_scatter_size,
                 move(copy_info.zero),
                 move(copy_info.copy_from),
                 move(copy_info.copy_to),
                 move(home_segments),
                 move(home_index),
                 move(away_segments),
                 move(away_index),
                 has_target_max_gs_receive_size_,
                 target_max_gs_receive_size_,
                 require_rank_order_completion_);

    EE_DIAG_POST
}

Token::Token(mpi::Comm comm,
             MPI_Comm new_comm,
             MPI_Comm new_comm_flipped,
             size_t minimum_gather_size,
             size_t minimum_scatter_size,
             vector<size_t> &&zero,
             vector<size_t> &&copy_from_info,
             vector<size_t> &&copy_to_info,
             vector<internal::Segment> &&home_segments,
             vector<local_index_t> &&home_index,
             vector<internal::Segment> &&away_segments,
             vector<local_index_t> &&away_index,
             bool has_target_max_gs_receive_size,
             std::uint32_t target_max_gs_receive_size,
             bool require_rank_order_completion)
    : comm_(std::move(comm)),
      topo_comm(mpi::UniqueComm::from_handle(new_comm)),
      topo_comm_flipped(mpi::UniqueComm::from_handle(new_comm_flipped)),
      minimum_gather_size_(minimum_gather_size),
      minimum_scatter_size_(minimum_scatter_size),
      zero_(move(zero)),
      copy_from_info_(move(copy_from_info)),
      copy_to_info_(move(copy_to_info)),
      home_segments_(move(home_segments)),
      home_index_(move(home_index)),
      away_segments_(move(away_segments)),
      away_index_(move(away_index)),
      has_target_max_gs_receive_size_(has_target_max_gs_receive_size),
      target_max_gs_receive_size_(target_max_gs_receive_size),
      require_rank_order_completion_(require_rank_order_completion) {}

void Token::FillHomeArrays(std::span<mpi::rank_t> ranks,
                           std::span<eap::utility::FortranIndex<local_index_t>> los,
                           std::span<local_index_t> lengths,
                           std::span<eap::utility::FortranIndex<local_index_t>> indices) const {
    EE_DIAG_PRE

    EE_ASSERT((size_t)ranks.size() >= home_segments_.size());
    EE_ASSERT((size_t)los.size() >= home_segments_.size());
    EE_ASSERT((size_t)lengths.size() >= home_segments_.size());

    for (size_t i = 0; i < home_segments_.size(); i++) {
        ranks[i] = home_segments_[i].rank;
        los[i] = static_cast<local_index_t>(home_segments_[i].begin);
        lengths[i] = static_cast<local_index_t>(home_segments_[i].length);
    }

    EE_ASSERT((size_t)indices.size() >= home_index_.size());

    std::copy(home_index_.begin(), home_index_.end(), indices.begin());

    EE_DIAG_POST
}
