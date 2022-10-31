/**
 * @file mesh-cells.cpp
 * @author Jonas Lippuner <jlippuner@lanl.gov>
 * @brief Implementation of Cells class
 * @date 2019-02-12
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <mesh-cells.hpp>

// Internal Includes
#include <totalview-viz.hpp>
#include <utility-algorithm.hpp>
#include <utility-linear_view.hpp>

using namespace eap;
using namespace eap::mesh;

// Define some routines for displaying some of the Views in Cells in TotalView
EAP_TOTALVIEW_DEFINE_DISPLAY_VIEW(MeshView<FortranLocalIndex *>)
EAP_TOTALVIEW_DEFINE_DISPLAY_VIEW(MeshView<FortranGlobalIndex *>)

Cells::Cells(eap::comm::TokenBuilder const &token_builder)
    : token_builder_(token_builder),
      num_global_cells_(0),
      num_local_cells_(0),
      max_num_local_cells_(0),
      num_local_cells_with_clones_(0),
      num_local_reserve_cells_(0),
      all_num_local_cells_("Cells::all_num_local_cells_", 0),
      global_base_address_("Cells::global_base_address_", 0),
      cell_address_("Cells::cell_address_", 0),
      cell_active_("Cells::cell_active_", 0),
      cell_center_("Cells::cell_center_", 0, 0),
      cell_position_("Cells::cell_position_", 0, 0),
      cell_half_("Cells::cell_half_", 0, 0),
      cell_half_lo_("Cells::cell_half_lo_", 0, 0),
      cell_half_hi_("Cells::cell_half_hi_", 0, 0),
      cell_volume_("Cells::cell_volume_", 0),
      global_state_consistent_(GlobalStateConsistency::Consistent),
      cell_arrays_allocated_(false) {
    auto const num_pe = mpi::Comm::world().size();

    Kokkos::resize(global_base_address_, num_pe + 1);
    Kokkos::deep_copy(global_base_address_, 0);

    Kokkos::resize(all_num_local_cells_, num_pe);
    Kokkos::deep_copy(all_num_local_cells_, 0);
}

void Cells::SetNumLocalCells(local_index_t num_local_cells) {
    EE_DIAG_PRE

    switch (global_state_consistent_) {
    case GlobalStateConsistency::Consistent:
        global_state_consistent_ = GlobalStateConsistency::NeedsUpdateGlobalBase;
        break;
    case GlobalStateConsistency::NeedsSetNumLocalCells:
        EE_ASSERT_EQ(all_num_local_cells_[mpi::Comm::world().rank()],
                     num_local_cells,
                     "Tried to SetNumLocalCells to a different value than received from "
                     "Cells::PrepareGlobalBase");
        UpdateGlobalAddress_(num_local_cells);
        global_state_consistent_ = GlobalStateConsistency::Consistent;
        break;
    case GlobalStateConsistency::NeedsUpdateGlobalBase:
        // No problem - just setting a new NumLocalCells
        break;
    }
    num_local_cells_ = num_local_cells;

    EE_DIAG_POST_MSG(EE_DBG(num_local_cells))
}

void Cells::UpdateGlobalBase() {
    EE_DIAG_PRE

    EE_ASSERT(
        global_state_consistent_ == GlobalStateConsistency::NeedsUpdateGlobalBase ||
            global_state_consistent_ == GlobalStateConsistency::Consistent,
        "Tried to call Cells::UpdateGlobalBase without first returning to a consistent state");

    if (global_state_consistent_ != GlobalStateConsistency::Consistent) {
        SetGlobalBase(num_local_cells_);
    }

    global_state_consistent_ = GlobalStateConsistency::Consistent;

    EE_DIAG_POST
}

void Cells::CheckGlobalBase() {
    UpdateGlobalBase();
    CheckGlobalAddress_();
}

void Cells::PrepareGlobalBase(local_index_t new_num_local_cells) {
    EE_DIAG_PRE

    EE_ASSERT(global_state_consistent_ == GlobalStateConsistency::Consistent,
              "Cells::PrepareGlobalBase is only valid from the consistent state");

    SetGlobalBase(new_num_local_cells);
    global_state_consistent_ = GlobalStateConsistency::NeedsSetNumLocalCells;

    EE_DIAG_POST_MSG(EE_DBG(new_num_local_cells))
}

void Cells::ResizeCellArrays(const size_t size, const size_t num_dims) {
    Kokkos::resize(cell_address_, size);
    Kokkos::resize(cell_active_, size);
    Kokkos::resize(cell_center_, size, num_dims);
    Kokkos::resize(cell_position_, size, num_dims);
    Kokkos::resize(cell_half_, size, num_dims);
    Kokkos::resize(cell_half_lo_, size, num_dims);
    Kokkos::resize(cell_half_hi_, size, num_dims);
    Kokkos::resize(cell_volume_, size);

    cell_arrays_allocated_ = true;
}

void Cells::SetGlobalBase(local_index_t num_local_cells) {
    auto comm = mpi::Comm::world();

    {
        // Ensure unit-stride
        auto all_num_local_cells = eap::utility::MakeLinearView(all_num_local_cells_);

        comm.all_gather(num_local_cells, all_num_local_cells.Span());

        // Syncs all_num_local_cells
    }

    utility::InclusiveExclusiveSumScanHost(all_num_local_cells_, global_base_address_);

    // Avoid another all_gather by passing the all_num_local_cells array directly to TokenBuilder
    {
        // Ensure unit-stride
        auto global_base_address = eap::utility::MakeLinearView(global_base_address_);

        // CellBases is just the exclusive scan portion of the array
        token_builder_.SetCellBases(global_base_address.Span().subspan(0, comm.size()));

        // Syncs global_base_address
    }

    UpdateGlobalAddress_(num_local_cells);
}

void Cells::UpdateGlobalAddress_(local_index_t num_local_cells) {
    EE_PRELUDE

    auto comm = mpi::Comm::world();

    EE_ASSERT(num_local_cells <= cell_address_.size(),
              "Cells::ResizeCellArrays must be called with a large enough size prior to calling "
              "Cells::UpdateGlobalAddress. "
                  << EE_DBG(num_local_cells) << ", " << EE_DBG(cell_address_.size()));

    {
        // "Copy" View handle to be transferred to Kokkos kernel
        auto cell_address = cell_address_;
        auto global_base_address = global_base_address_[comm.rank()];

        Kokkos::parallel_for(
            "Cells::SetGlobalBase::cell_address_",
            // Run wherever cell_address is located
            Kokkos::RangePolicy<decltype(cell_address)::execution_space>(0, num_local_cells),
            KOKKOS_LAMBDA(local_index_t l) { cell_address(l) = l + global_base_address; });
    }
}

void Cells::CheckGlobalAddress_() const {
    EE_PRELUDE

    auto comm = mpi::Comm::world();

    {
        // "Copy" View handle to be transferred to Kokkos kernel
        auto cell_address = cell_address_;
        auto global_base_address = global_base_address_[comm.rank()];

        local_index_t global_base_address_out_of_date;
        Kokkos::parallel_reduce(
            "Cells::CheckGlobalAddress",
            // Run wherever cell_address is located
            Kokkos::RangePolicy<decltype(cell_address)::execution_space>(0, num_local_cells_),
            KOKKOS_LAMBDA(local_index_t l, local_index_t & out_of_date) {
                out_of_date += (cell_address(l) != l + global_base_address);
            },
            global_base_address_out_of_date);

        EE_ASSERT(global_base_address_out_of_date == 0, "cell_address isn't up to date");
    }
}
