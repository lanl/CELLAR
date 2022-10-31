/**
 * @file mesh-kidmom.cpp
 *
 * @brief Implements KidMom data structure for Mesh
 * @date 2019-02-15
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <mesh-kidmom.hpp>

// STL Includes
#include <limits>
#include <numeric>
#include <utility>

// Third Party Includes
#include <Kokkos_ScatterView.hpp>
#include <mpi/mpi.hpp>

// Internal Includes
#include <comm-patterns.hpp>
#include <utility-algorithm.hpp>
#include <utility-linear_view.hpp>

// Local Includes
#include <mesh-recon_move.hpp>

using namespace eap;
using namespace eap::mesh;

using eap::utility::nullint;

// ---- KidMom Public Routines ----

KidMom::KidMom()
    : cell_mother_("KidMom::cell_mother", 0),
      cell_daughter_("KidMom::cell_daughter", 0),
      ltop_("KidMom::ltop", 0),
      all_top_("KidMom::all_top", 0),
      lpoint_("KidMom::lpoint", 0),
      lopack_("KidMom::lopack", 0) {}

void KidMom::ResizeLocal(local_index_t num_cells, local_index_t newsize) {
    EE_PRELUDE

    EE_ASSERT(newsize >= num_cells);

    Kokkos::resize(cell_mother_, newsize);
    Kokkos::resize(cell_daughter_, newsize);
    Kokkos::resize(ltop_, newsize);
    lpoint_.resize(newsize);
}

void KidMom::Initialize(KidMomOptions const &options) { options_ = options; }

void KidMom::InitializeLevel1() {
    Kokkos::deep_copy(cell_mother_, nullint);
    Kokkos::deep_copy(cell_daughter_, nullint);
}

std::pair<local_index_t, global_index_t> KidMom::BuildTop(Cells const &cells) {
    local_index_t num_top = 0;
    for (local_index_t l = 0; l < cells.num_local_cells(); l++) {
        if (IsTop(cells, l)) {
            ltop_(num_top) = l;
            num_top++;
        }
    }

    return std::make_pair(num_top,
                          mpi::Comm::world().all_reduce(mpi::sum(), (global_index_t)num_top));
}

local_index_t KidMom::SetupAllTop(Cells const &cells, local_index_t num_top) {
    using Kokkos::subview;
    using std::make_pair;

    Kokkos::resize(all_top_, cells.num_local_cells_with_clones());

    Kokkos::deep_copy(subview(all_top_, make_pair(local_index_t(0), num_top)),
                      subview(ltop_, make_pair(local_index_t(0), num_top)));

    for (local_index_t l = 0; l < cells.num_clone_cells(); l++) {
        all_top_(num_top + l) = cells.num_local_cells() + l;
    }

    return num_top + cells.num_clone_cells();
}

void KidMom::BuildPack(Cells const &cells,
                       MeshDualView<OptionalFortranLocalIndex *> cell_level_dual) {
    EE_DIAG_PRE

    EE_ASSERT(lpoint_.extent(0) >= cell_level_dual.extent(0));
    EE_ASSERT(!cell_level_dual.need_sync_device());

    auto cell_level = cell_level_dual.view_device();

    auto const max_levels = eap::utility::LocalGlobalMax(cell_level);
    num_pack_ = max_levels.local.value();
    max_num_pack_ = max_levels.global.value();
    num_levels_ = num_pack_ + 1;

    lopack_.resize(num_levels_ + 1);

    // Count the number of cells at each level
    {
        // Currently using Serial on Host, but speed up can be achieved using OpenMP at
        // >~100k cells. Consider supporting later.
        using ExecSpace =
            std::conditional<std::is_same<Kokkos::DefaultHostExecutionSpace,
                                          decltype(cell_level)::execution_space>::value,
                             Kokkos::Serial,
                             Kokkos::DefaultExecutionSpace>::type;

        Kokkos::View<local_index_t *, ExecSpace> level_counts(
            "eap::mesh::Kidmom::BuildPack::level_counts", num_levels_);
        {
            auto level_counts_scatter =
                Kokkos::Experimental::create_scatter_view<Kokkos::Experimental::ScatterSum,
                                                          void,
                                                          void,
                                                          local_index_t *,
                                                          ExecSpace>(level_counts);

            // Count the number of cells at each level
            Kokkos::parallel_for(
                "eap::mesh::KidMom::BuildPack::count_levels",
                Kokkos::RangePolicy<ExecSpace>(0, cells.num_local_cells()),
                KOKKOS_LAMBDA(int const i) {
                    auto level_counts = level_counts_scatter.access();
                    if (cell_level(i)) {
                        level_counts(local_index_t(*cell_level(i))) += 1;
                    }
                });

            Kokkos::Experimental::contribute(level_counts, level_counts_scatter);
        }

        lopack_.sync_device();

        eap::utility::InclusiveExclusiveSumScan<decltype(level_counts)::const_type,
                                                decltype(lopack_)::t_dev,
                                                ExecSpace>(level_counts, lopack_.view_device());

        lopack_.modify_device();
    }

    // Fill out lpoint_
    {
        Kokkos::View<local_index_t *, eap::HostMemorySpace> current_lo(
            "eap::mesh::Kidmom::BuildPack::current_lo", num_levels_);

        lopack_.sync_host();

        Kokkos::deep_copy(
            current_lo,
            Kokkos::subview(lopack_, Kokkos::make_pair(0, current_lo.extent_int(0))).view_host());

        lpoint_.clear_sync_state();

        {
            auto lpoint = lpoint_.view_host();

            for (local_index_t i = 0; i < cell_level.extent(0); i++) {
                if (cell_level(i)) {
                    lpoint(current_lo(local_index_t(*cell_level(i)))++) = i;
                }
            }
        }

        lpoint_.modify_host();
    }

    EE_DIAG_POST
}

void KidMom::KidMomBuild(Cells &cells) {
    EE_DIAG_PRE

    // Immediately free old tokens - not needed anymore
    kid_token_.clear();

    // Ensure that cells global arrays are up-to-date
    cells.UpdateGlobalBase();

    // Ensures that the information on what level cells are at is up to date
    SyncCellsAtLevelHost();

    // copy token_builder
    auto token_builder = cells.token_builder();

    if (options_.kid_mom_use_s2s) {
        token_builder.SetToPes(KidMomBuildToPes(token_builder));
    }

    std::vector<FortranLocalIndex> home_addresses;
    std::vector<OptionalFortranGlobalIndex> away_globals;

    // Last Level doesn't have any mothers
    for (local_index_t level = 1; level < num_levels_ - 1; level++) {
        home_addresses.clear();
        away_globals.clear();

        for (local_index_t const kid : CellsAtLevelHost(level)) {
            home_addresses.push_back(kid);
            away_globals.push_back(cell_mother_(kid));
        }

        kid_token_.push_back(token_builder.BuildGlobal(home_addresses, away_globals));
    }

    EE_DIAG_POST
}

void KidMom::MomKidBuild(Cells &cells) {
    EE_DIAG_PRE

    // Immediately free old tokens - not needed anymore
    mom_token_.clear();

    // Ensure that cells global arrays are up-to-date
    cells.UpdateGlobalBase();

    // Ensures that the information on what level cells are at is up to date
    SyncCellsAtLevelHost();

    // copy token_builder
    auto token_builder = cells.token_builder();

    if (options_.mom_kid_use_s2s) {
        token_builder.SetToPes(MomKidBuildToPes(token_builder));
    }

    std::vector<FortranLocalIndex> home_addresses;
    std::vector<OptionalFortranGlobalIndex> away_globals;

    // Last Level doesn't have any mothers
    for (local_index_t level = 1; level < num_levels_ - 1; level++) {
        home_addresses.clear();
        away_globals.clear();

        for (local_index_t const mom : CellsAtLevelHost(level)) {
            if (OptionalFortranGlobalIndex const kid = cell_daughter_(mom)) {
                home_addresses.push_back(mom);
                away_globals.push_back(*kid);
            }
        }

        mom_token_.push_back(token_builder.BuildGlobal(home_addresses, away_globals));
    }

    EE_DIAG_POST
}

void KidMom::MomKidFree() { mom_token_.clear(); }

void KidMom::MomKidsBuild(Cells &cells, int num_dims) {
    EE_DIAG_PRE

    auto const num_kids = 1 << num_dims;

    // Last Level doesn't have any mothers
    auto const num_mother_levels = num_levels_ - 1;

    mom_kids_token_.clear();

    cells.UpdateGlobalBase();

    // copy token_builder
    auto token_builder = cells.token_builder();

    if (options_.mom_kids_use_s2s) {
        token_builder.SetToPes(MomKidsBuildToPes(token_builder, num_dims));
    }

    std::vector<FortranLocalIndex> home_addresses;
    std::vector<OptionalFortranGlobalIndex> away_globals;

    // reverse decrement of levels
    auto level = num_mother_levels;
    while (level-- > 0) {
        for (auto i = 0; i < num_kids; i++) {
            home_addresses.clear();
            away_globals.clear();

            // Maps mom to each of its children
            for (local_index_t const mom : CellsAtLevelHost(level)) {
                if (OptionalFortranGlobalIndex const kid = cell_daughter_(mom)) {
                    home_addresses.push_back(mom);
                    away_globals.push_back(OptionalFortranGlobalIndex(*kid + i));
                }
            }

            mom_kids_token_.push_back(token_builder.BuildGlobal(home_addresses, away_globals));
        }
    }

    EE_DIAG_POST
}

void KidMom::ResetMothersAndDaughters(Cells const &cells) {
    using eap::comm::TokenOperation;

    // Last Level doesn't have any mothers
    auto const num_mother_levels = num_levels_ - 1;

    {
        // reverse decrement of levels
        auto level = num_mother_levels;
        while (level-- > 0) {
            // explicitly use global_index_t as buffer type since cell_address and cell_daughter are
            // not compatible
            mom_token_[level]
                .Get<std::decay_t<decltype(cells.cell_address())>,
                     decltype(cell_daughter_),
                     global_index_t>(TokenOperation::Copy, cells.cell_address(), cell_daughter_);
        }
    }

    {
        for (local_index_t level = 0; level < num_mother_levels; level++) {
            // explicitly use global_index_t as buffer type since cell_address and cell_mother are
            // not compatible
            kid_token_[level]
                .Get<std::decay_t<decltype(cells.cell_address())>,
                     decltype(cell_mother_),
                     global_index_t>(TokenOperation::Copy, cells.cell_address(), cell_mother_);
        }
    }
}

void KidMom::ResetMothersAndDaughtersNew(Cells const &cells, local_index_t new_cells) {
    using Kokkos::deep_copy;
    using Kokkos::make_pair;
    using Kokkos::subview;

    deep_copy(subview(cell_mother_, make_pair(cells.num_local_cells(), new_cells)), nullint);
    deep_copy(subview(cell_daughter_, make_pair(cells.num_local_cells(), new_cells)), nullint);
}

void KidMom::ReconMove(nonstd::span<FortranLocalIndex const> send_start,
                       nonstd::span<local_index_t const> send_length,
                       nonstd::span<FortranLocalIndex const> recv_start,
                       nonstd::span<local_index_t const> recv_length) {
    EE_DIAG_PRE

    ReconMovePattern pattern(send_start, send_length, recv_start, recv_length);
    pattern.Move(cell_mother_);
    pattern.Move(cell_daughter_);
    pattern.Move(ltop_);
    {
        lpoint_.sync_host();
        auto lpoint = lpoint_.view_host();
        pattern.Move(lpoint);
        lpoint_.modify_host();
    }

    EE_DIAG_POST
}

void KidMom::SyncCellsAtLevelHost() {
    lopack_.sync_host();
    lpoint_.sync_host();
}

// ---- KidMom Private Routines ----

void KidMom::CheckLoHiPackAndLevels(Cells const &cells,
                                    MeshDualView<OptionalFortranLocalIndex *> cell_level_dual) {
    EE_DIAG_PRE

    lopack_.sync_host();
    cell_level_dual.sync_host();

    auto const lopack = lopack_.view_host();
    auto const cell_level = cell_level_dual.view_host();

    EE_ASSERT_EQ(0, lopack(0));

    EE_ASSERT_EQ(
        cells.num_local_cells(), lopack(num_levels_), "num_levels_ (=" << num_levels_ << ")");

    local_index_t expected_cells_count = 0;
    for (local_index_t l = 0; l < cells.num_local_cells(); l++) {
        if (cell_level(l)) {
            expected_cells_count++;
        }
    }

    local_index_t found_cells_count = 0;
    for (local_index_t level = 0; level < num_levels_; level++) {
        for (local_index_t const l : CellsAtLevelHost(level)) {
            found_cells_count++;
            EE_ASSERT_EQ(level, cell_level(l), "lpoint and cell_level not in-sync");
        }
    }

    EE_ASSERT_EQ(expected_cells_count, found_cells_count);

    EE_DIAG_POST_MSG("Internal state of Levels is inconsistent")
}

bool KidMom::IsTop(Cells const &cells, local_index_t cell) const {
    return cells.IsActive(cell) && !cell_daughter_(cell);
}

std::vector<int> KidMom::KidMomBuildToPes(eap::comm::TokenBuilder const &token_builder) const {
    std::vector<int> to_pes(mpi::Comm::world().size());
    std::vector<OptionalFortranGlobalIndex> away_globals;

    // Last Level doesn't have any mothers
    for (local_index_t level = 1; level < num_levels_ - 1; level++) {
        away_globals.clear();

        for (local_index_t const kid : CellsAtLevelHost(level)) {
            away_globals.push_back(cell_mother_(kid));
        }

        token_builder.FlagPes(away_globals, to_pes);
    }

    return to_pes;
}

std::vector<int> KidMom::MomKidBuildToPes(eap::comm::TokenBuilder const &token_builder) const {
    std::vector<int> to_pes(mpi::Comm::world().size());
    std::vector<OptionalFortranGlobalIndex> away_globals;

    // Last Level doesn't have any mothers
    for (local_index_t level = 1; level < num_levels_ - 1; level++) {
        away_globals.clear();

        for (local_index_t const mom : CellsAtLevelHost(level)) {
            if (OptionalFortranGlobalIndex const kid = cell_daughter_(mom)) {
                away_globals.push_back(*kid);
            }
        }

        token_builder.FlagPes(away_globals, to_pes);
    }

    return to_pes;
}

std::vector<int> KidMom::MomKidsBuildToPes(eap::comm::TokenBuilder const &token_builder,
                                           int num_dims) const {
    auto const num_kids = 1 << num_dims;

    // Last Level doesn't have any mothers
    auto const num_mother_levels_ = num_levels_ - 1;

    std::vector<int> to_pes(mpi::Comm::world().size());
    std::vector<OptionalFortranGlobalIndex> away_globals;

    // reverse decrement of levels
    auto level = num_mother_levels_;
    while (level-- > 0) {
        for (auto i = 0; i < num_kids; i++) {
            away_globals.clear();

            // Maps mom to each of its children
            for (local_index_t const mom : CellsAtLevelHost(level)) {
                if (OptionalFortranGlobalIndex const kid = cell_daughter_(mom)) {
                    away_globals.push_back(OptionalFortranGlobalIndex(*kid + i));
                }
            }

            token_builder.FlagPes(away_globals, to_pes);
        }
    }

    return to_pes;
}
