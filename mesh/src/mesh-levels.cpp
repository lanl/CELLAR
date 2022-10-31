/**
 * @file mesh-levels.cpp
 *
 * @brief Implements Levels datastructure for the EAP Mesh
 * @date 2019-02-14
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Includes
#include <mesh-levels.hpp>

// STL Includes
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>

// Internal Includes
#include <error-macros.hpp>

// Local Includes
#include <mesh-dzn_compute.hpp>
#include <mesh-recon_move.hpp>

using namespace eap;
using namespace eap::mesh;
using namespace eap::utility::literals;

using Kokkos::deep_copy;
using Kokkos::make_pair;
using Kokkos::parallel_for;
using Kokkos::RangePolicy;
using Kokkos::subview;

Levels::Levels()
    : cell_level_("Levels::cell_level", 0),
      area_("Levels::area"),
      gdxx_("Levels::gdxx"),
      dxyzmn_("Levels::dxyzmn"),
      dtsize_("Levels::dtsize"),
      sides_("Levels::sides"),
      sizes_("Levels::sizes"),
      gsizes_("Levels::gsizes"),
      levelmx_("Levels::levelmx", 0),
      flag_("Levels::flag", 0),
      flag_tag_("Levels::flag_tag", 0),
      amr_tag_("Levels::amr_tag", 0),
      chunk_ids_("Levels::chunk_ids", 0) {
    CheckState({State::Constructed});
}

void Levels::ResizeLocal(local_index_t num_cells, local_index_t newsize) {
    EE_DIAG_PRE

    // Check valid states
    CheckState({State::Constructed, State::Reconstruction});

    EE_ASSERT(newsize >= num_cells, "newsize must be larger than num_cells");

    // Size isn't actually changing - don't need to do anything
    if (num_cells == num_cells_) {
        return;
    }

    // We allow resized_cell_info_ to be set already since it's possible that some places are
    // resizing the cell info multiple times during an initialization process.

    // Reset initialized_cell_info_ to indicate that the caller needs to Initialize the Cell Info
    // now.
    initialized_cell_info_ = false;

    num_cells_ = num_cells;

    Kokkos::resize(levelmx_, newsize);
    cell_level_.resize(newsize);
    Kokkos::resize(flag_, newsize);
    Kokkos::resize(flag_tag_, newsize);
    Kokkos::resize(amr_tag_, newsize);

    this->cell_level = cell_level_;
    this->flag = flag_;
    this->flag_tag = flag_tag_;

    kid_mom_.ResizeLocal(num_cells_, newsize);

    this->cell_daughter = kid_mom_.cell_daughter_;
    this->cell_mother = kid_mom_.cell_mother_;

    this->ltop = kid_mom_.ltop_;

    resized_cell_info_ = true;

    EE_DIAG_POST_MSG("this->num_cells_ = " << this->num_cells_ << ", num_cells = " << num_cells
                                           << ", newsize = " << newsize)
}

void Levels::Initialize(LevelOptions const &options) {
    using std::accumulate;
    using std::int8_t;
    using std::min;
    using std::pow;
    using std::sqrt;

    EE_DIAG_PRE

    // Check valid states
    CheckState({State::Constructed, State::Reconstruction});

    num_dims_ = options.num_dim;

    EE_ASSERT(num_dims_ >= 1 && num_dims_ <= 3, "Invalid value for num_dim (" << num_dims_ << ")");

    // Initialize base level
    sizes_(0, X_DIR) = options.cell_size[X_DIR];
    sizes_(0, Y_DIR) = options.cell_size[Y_DIR];
    sizes_(0, Z_DIR) = options.cell_size[Z_DIR];

    // Unused dimensions are set to cell_size of 1.0
    for (int8_t dir = num_dims_; dir < MAX_DIM; dir++) {
        sizes_(0, dir) = 1.0;
    }

    // Initialize a value representing distance of the CFL condition. It's the inverse of the
    // minimum of the cell sides, factoring in higher-dimension diagonals scaled down by the number
    // of dimensions.
    double dtsize_inv = sizes_(0, X_DIR);
    for (int8_t dir = 1; dir < num_dims_; dir++) {
        dtsize_inv = min(dtsize_inv, sizes_(0, dir));
    }

    // Factor in cell face diagonals
    if (num_dims_ >= 2) {
        for (int8_t dir_a = 0; dir_a < num_dims_ - 1; dir_a++) {
            for (int8_t dir_b = dir_a + 1; dir_b < num_dims_; dir_b++) {
                dtsize_inv = min(dtsize_inv,
                                 sqrt(sizes_(0, dir_a) * sizes_(0, dir_a) +
                                      sizes_(0, dir_b) * sizes_(0, dir_b)) /
                                     2.0);
            }
        }
    }

    // Factor in 3D cell diagonals
    if (num_dims_ >= 3) {
        double sum = 0;
        for (int8_t dir = 0; dir < num_dims_; dir++) {
            sum += sizes_(0, dir) * sizes_(0, dir);
        }
        dtsize_inv = min(dtsize_inv, sqrt(sum) / 3.0);
    }
    dtsize_(0) = 1.0 / dtsize_inv;

    // Initialize rest of levels
    for (size_t level = 1; level < MAX_LEVELS; level++) {
        for (int8_t dir = 0; dir < num_dims_; dir++) {
            sizes_(level, dir) = 0.5 * sizes_(level - 1, dir);
        }

        for (int8_t dir = num_dims_; dir < MAX_DIM; dir++) {
            sizes_(level, dir) = 1.0;
        }

        dtsize_(level) = 2.0 * dtsize_(level - 1);
    }

    // Pre-computes a bunch of variables associated with each level
    {
        // TODO: At some point when these Views are changed to DualViews, these should be grabbing
        // the device pointers
        //
        // Need to capture these members by name since otherwise KOKKOS_LAMBDA will capture them by
        // reference via `this`, which is illegal.
        auto area = area_;
        auto dxyzmn = dxyzmn_;
        auto gdxx = gdxx_;
        auto gsizes = gsizes_;
        auto sides = sides_;
        auto sizes = sizes_;

        Kokkos::parallel_for(
            "InitLevelData",
            Kokkos::RangePolicy<Kokkos::DefaultHostExecutionSpace>(0, MAX_LEVELS),
            KOKKOS_LAMBDA(int level) {
                dxyzmn(level) = sizes(level, X_DIR);
                for (int8_t dir = 1; dir < MAX_DIM; dir++) {
                    dxyzmn(level) = min(dxyzmn(level), sizes(level, dir));
                }

                area(level) = sizes(level, X_DIR) * sizes(level, Y_DIR) * sizes(level, Z_DIR);

                for (auto dir = 0; dir < MAX_DIM; dir++) {
                    sides(level, dir) = sizes(level, (dir + 1) % 3) * sizes(level, (dir + 2) % 3);
                    gsizes(level, dir) = std::sqrt(1.0 / 12.0) * sizes(level, dir);
                }

                gdxx(level) = std::sqrt(3.0 / 20.0) * sizes(level, X_DIR);
            });
    }

    kid_mom_.Initialize(options.kid_mom);

    initialized_level_info_ = true;
    TryTransitionToReady();

    EE_DIAG_POST
}

void Levels::InitializeLevel1() {
    EE_DIAG_PRE

    CheckState({State::Constructed, State::Reconstruction});

    EE_ASSERT(resized_cell_info_,
              "You must call Levels::ResizeLocal before Levels::InitializeLevel1");

    cell_level_.clear_sync_state();
    Kokkos::deep_copy(cell_level_.view_host(), 0_fiu);
    cell_level_.modify_host();

    kid_mom_.InitializeLevel1();

    initialized_cell_info_ = true;
    TryTransitionToReady();

    EE_DIAG_POST
}

void Levels::MarkAsInitialized() {
    EE_DIAG_PRE

    CheckState({State::Constructed, State::Reconstruction});

    EE_ASSERT(resized_cell_info_, "You must call Levels::ResizeLocal first");

    initialized_cell_info_ = true;
    TryTransitionToReady();

    EE_DIAG_POST
}

void Levels::BuildPack(Cells const &cells) {
    cell_level_.sync_device();
    kid_mom_.BuildPack(cells, cell_level_);
}

void Levels::BuildTop(Cells const &cells) {
    auto tops = kid_mom_.BuildTop(cells);

    num_top_ = tops.first;
    sum_num_top_ = tops.second;
}

void Levels::SetupAllTop(Cells const &cells) {
    all_num_top_ = kid_mom_.SetupAllTop(cells, num_top_);
    this->all_top = kid_mom_.all_top_;

    Levels::GeneratedChunks const generated_chunks = GenerateChunkIds(cells);

    chunk_count_ = generated_chunks.chunk_count;
    chunk_ids_ = std::move(generated_chunks.chunk_ids);
}

void Levels::ReconMove(local_index_t data_length,
                       nonstd::span<FortranLocalIndex const> send_start,
                       nonstd::span<local_index_t const> send_length,
                       nonstd::span<FortranLocalIndex const> recv_start,
                       nonstd::span<local_index_t const> recv_length) {
    using Kokkos::make_pair;
    using Kokkos::subview;

    EE_DIAG_PRE

    ReconMovePattern pattern(send_start, send_length, recv_start, recv_length);
    pattern.Move(subview(levelmx_, make_pair((local_index_t)0, data_length)));
    pattern.Move(subview(flag_, make_pair((local_index_t)0, data_length)));
    pattern.Move(subview(flag_tag_, make_pair((local_index_t)0, data_length)));
    pattern.Move(subview(amr_tag_, make_pair((local_index_t)0, data_length)));
    {
        cell_level_.sync_host();
        auto cell_level = cell_level_.view_host();
        pattern.Move(subview(cell_level, make_pair((local_index_t)0, data_length)));
        cell_level_.modify_host();
    }

    kid_mom_.ReconMove(send_start, send_length, recv_start, recv_length);

    EE_DIAG_POST
}

void Levels::ReconPack(local_index_t nlow,
                       local_index_t nhigh,
                       local_diff_t nstep,
                       MeshView<FortranLocalIndex const *> move_from,
                       MeshView<FortranLocalIndex const *> move_to) {
    EE_PRELUDE

    EE_ASSERT(nstep == 1 || nstep == -1, EE_DBG(nstep));

    cell_level_.sync_host();
    auto const cell_level = cell_level_.view_host();
    auto const levelmx = levelmx_;
    auto const flag = flag_;
    auto const flag_tag = flag_tag_;
    auto const amr_tag = amr_tag_;
    auto const cell_mother = kid_mom_.cell_mother_;
    auto const cell_daughter = kid_mom_.cell_daughter_;

    auto const range =
        nstep == 1 ? Kokkos::RangePolicy<Kokkos::DefaultHostExecutionSpace>(nlow, nhigh + 1)
                   : Kokkos::RangePolicy<Kokkos::DefaultHostExecutionSpace>(nhigh, nlow + 1);

    Kokkos::parallel_for(
        "Levels::ReconPack", range, KOKKOS_LAMBDA(local_index_t const i) {
            local_index_t const from_l = move_from(i);
            local_index_t const to_l = move_to(i);

            cell_level(to_l) = cell_level(from_l);
            levelmx(to_l) = levelmx(from_l);
            flag(to_l) = flag(from_l);
            flag_tag(to_l) = flag_tag(from_l);
            amr_tag(to_l) = amr_tag(from_l);

            cell_mother(to_l) = cell_mother(from_l);
            cell_daughter(to_l) = cell_daughter(from_l);
        });

    cell_level_.modify_host();
}

void Levels::ReconDivChild(Cells const &cells,
                           local_index_t level,
                           local_index_t parentl,
                           local_index_t l) {
    flag_(l) = 0;
    flag_tag_(l) = 0;
    amr_tag_(l) = flag_tag_(parentl);
    levelmx_(l) = 0;

    cell_level_.sync_host();

    {
        auto cell_level = cell_level_.view_host();
        cell_level(l) = level + 1;
    }

    cell_level_.modify_host();

    kid_mom_.ReconDivChild(l, cells.cell_address()(parentl));
}

void Levels::SetFlagsScalar(local_index_t low, local_index_t high, int32_t value) {
    deep_copy(subview(flag_, make_pair(low, high)), value);
    deep_copy(subview(flag_tag_, make_pair(low, high)), value);
}

void Levels::SetFlagsAll(int32_t value) { Kokkos::deep_copy(flag_, value); }

void Levels::SetFlagsScalarFromList(MeshView<FortranLocalIndex const *> list, int32_t value) {
    auto const flag = flag_;

    Kokkos::parallel_for(
        "Levels::SetFlagsScalarFromList", list.extent(0), KOKKOS_LAMBDA(local_index_t const i) {
            local_index_t const l = list(i);
            flag(l) = value;
        });
}

void Levels::SetFlagsScalarAndTagsFromList(MeshView<FortranLocalIndex const *> list,
                                           int32_t value,
                                           MeshView<int32_t const *> tags) {
    auto const flag = flag_;
    auto const flag_tag = flag_tag_;

    Kokkos::parallel_for(
        "Levels::SetFlagsScalarAndTagsFromList",
        list.extent(0),
        KOKKOS_LAMBDA(local_index_t const i) {
            local_index_t const l = list(i);
            flag(l) = value;
            flag_tag(l) = tags(l);
        });
}

void Levels::SetFlagsFromTuples(MeshView<Kokkos::pair<FortranLocalIndex, int32_t> const *> tups) {
    auto const flag = flag_;

    Kokkos::parallel_for(
        "Levels::SetFlagsFromTuples", tups.extent(0), KOKKOS_LAMBDA(local_index_t const i) {
            flag(local_index_t(tups(i).first)) = tups(i).second;
        });
}

void Levels::SetFlagTagsFromTuples(
    MeshView<Kokkos::pair<FortranLocalIndex, int32_t> const *> tups) {
    auto const flag_tag = flag_tag_;

    Kokkos::parallel_for(
        "Levels::SetFlagTagsFromTuples", tups.extent(0), KOKKOS_LAMBDA(local_index_t const i) {
            flag_tag(local_index_t(tups(i).first)) = tups(i).second;
        });
}

void Levels::SetFlagsFromTuplesKeyAndScalar(
    MeshView<Kokkos::pair<FortranLocalIndex, int32_t> const *> tups, int32_t value) {
    auto const flag = flag_;

    Kokkos::parallel_for(
        "Levels::SetFlagsFromTuplesKeyAndScalar",
        tups.extent(0),
        KOKKOS_LAMBDA(local_index_t const i) { flag(local_index_t(tups(i).first)) = value; });
}

void Levels::SetFlagTagsFromTuplesKeyAndScalar(
    MeshView<Kokkos::pair<FortranLocalIndex, int32_t> const *> tups, int32_t value) {
    auto const flag_tag = flag_tag_;

    Kokkos::parallel_for(
        "Levels::SetFlagTagsFromTuplesKeyAndScalar",
        tups.extent(0),
        KOKKOS_LAMBDA(local_index_t const i) { flag_tag(local_index_t(tups(i).first)) = value; });
}

void Levels::ApplyDZNs(DefinedZoneResolutionInfo const &dzn,
                       std::uint8_t numdim,
                       Cells const &cells,
                       double time,
                       Kode kode) {
    eap::mesh::ProcessCellsInDZNFilterKode(
        dzn, numdim, cells, time, kode, 0, cells.num_local_cells_with_clones(), levelmx_, levelmx_);
}

void Levels::SetLevelMaxFromTuples(
    MeshView<Kokkos::pair<FortranLocalIndex, local_index_t> const *> tups) {
    auto const levelmx = levelmx_;

    parallel_for(
        "Levels::SetLevelMaxFromTuples",
        RangePolicy<>(size_t(0), tups.extent(0)),
        KOKKOS_LAMBDA(local_index_t const i) {
            levelmx((local_index_t)tups(i).first) = tups(i).second;
        });
}

// ---- Level Private Routines ----

local_index_t Levels::CountChunks(Cells const &cells) const {
    if (all_num_top_ <= 0) {
        return 0;
    }

    auto const &ltop = kid_mom_.ltop();

    local_index_t run_count = 0;

    {
        local_index_t lastl = ltop(0);
        local_index_t run_length = 1;
        for (local_index_t i = 1; i < num_top_; i++) {
            if (ltop(i) == lastl + 1 && run_length < ELEM_MAX) {
                run_length++;
            } else {
                run_count++;

                run_length = 1;
            }

            lastl = ltop(i);
        }
    }

    run_count++;

    if (cells.num_clone_cells() > 0) {
        local_index_t const diff = cells.num_clone_cells();
        local_index_t const quot = diff / ELEM_MAX;

        run_count += quot;
        if (quot * ELEM_MAX < diff) {
            run_count++;
        }
    }

    return run_count;
}

std::vector<std::array<local_index_t, 2>> Levels::PopulateChunks(Cells const &cells) const {
    using std::min;

    if (all_num_top_ <= 0) {
        return {};
    }

    local_index_t const num_chunks = CountChunks(cells);

    std::vector<std::array<local_index_t, 2>> chunks;
    chunks.reserve(num_chunks);

    auto const &ltop = kid_mom_.ltop();

    local_index_t lastl = ltop(0);
    local_index_t run_length = 1;

    local_index_t chunk_start = lastl;
    for (local_index_t i = 1; i < num_top_; i++) {
        if (ltop(i) != lastl + 1 || run_length == ELEM_MAX) {
            chunks.push_back({chunk_start, lastl});
            chunk_start = ltop(i);

            run_length = 1;
        } else {
            run_length++;
        }

        lastl = ltop(i);
    }
    chunks.push_back({chunk_start, lastl});

    for (local_index_t ci = cells.num_local_cells() + 1; ci < cells.num_local_cells_with_clones();
         ci++) {
        chunks.push_back(
            {ci, min(local_index_t(ci + ELEM_MAX - 1), cells.num_local_cells_with_clones())});
    }

    return chunks;
}

Levels::GeneratedChunks Levels::GenerateChunkIds(Cells const &cells) const {
    if (cells.num_local_cells() <= 0) {
        return {0, MeshView<utility::FortranIndex<local_index_t> *>("Levels::chunk_ids", 0)};
    }

    MeshView<utility::FortranIndex<local_index_t> *> chunk_ids("Levels::chunk_ids",
                                                               cells.num_local_cells_with_clones());

    auto chunks = PopulateChunks(cells);

    for (local_index_t ci = 0; ci < chunks.size(); ci++) {
        for (local_index_t l = chunks[ci][0]; l <= chunks[ci][1]; l++) {
            chunk_ids(l) = ci;
        }
    }

    return GeneratedChunks{local_index_t(chunks.size()), chunk_ids};
}

template <typename Collection>
std::string Levels::StatesString(Collection const &collection) {
    std::stringstream ss;
    auto first = true;
    for (auto state : collection) {
        if (!first) ss << ", ";
        ss << StateString(state);
        first = false;
    }
    return ss.str();
}

void Levels::CheckState(std::initializer_list<State> valid_states) const {
    EE_PRELUDE

    EE_ASSERT(std::find(valid_states.begin(), valid_states.end(), state_) != valid_states.end(),
              "Invalid call for current Levels state. Current State: "
                  << StateString(state_) << ", Valid States: [" << StatesString(valid_states)
                  << "]");
}

void Levels::StateTransition(std::initializer_list<State> from, State to) {
    EE_PRELUDE

    EE_ASSERT(std::find(from.begin(), from.end(), state_) != from.end(),
              "Invalid Levels state transition. Current state: "
                  << StateString(state_) << ", Target State: " << StateString(to)
                  << ", Valid Start States: [" << StatesString(from) << "]");

    state_ = to;
}

void Levels::TryTransitionToReady() {
    CheckState({State::Constructed, State::Reconstruction});

    if (initialized_cell_info_ && initialized_level_info_) {
        state_ = State::Ready;
    }
}