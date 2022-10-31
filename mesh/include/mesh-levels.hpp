/**
 * @file mesh-levels.hpp
 *
 * @brief Data structure for traversing levels of resolution in the EAP Mesh
 * @date 2019-02-14
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_LEVELS_HPP_
#define EAP_MESH_LEVELS_HPP_

// STL Includes
#include <cstddef>
#include <initializer_list>

// Local Includes
#include "mesh-cells.hpp"
#include "mesh-constants.hpp"
#include "mesh-dzn.hpp"
#include "mesh-kidmom.hpp"
#include "mesh-types.hpp"

namespace eap {
namespace mesh {
struct LevelOptions {
    std::int8_t num_dim;
    double cell_size[3];
    KidMomOptions kid_mom;
};

/**
 * @brief
 *
 */
class Levels {
  public:
    // Public, read-only array references

    /**
     * @brief Mother of each cell
     */
    MeshView<OptionalFortranGlobalIndex const *> cell_mother;

    /**
     * @brief Daughter of each cell
     */
    MeshView<OptionalFortranGlobalIndex const *> cell_daughter;

    /**
     * @brief A list of local top level cells - active cells with no daughters
     */
    MeshView<FortranLocalIndex const *> ltop;

    /**
     * @brief A list of local + clone top level cells - active cells with no daughters
     */
    MeshView<FortranLocalIndex const *> all_top;

    /**
     * @brief A list of levels for local cells
     */
    MeshDualView<OptionalFortranLocalIndex const *> cell_level;

    /// Triggers refinement (>0) or coarsening (<0) for current cycle
    MeshView<std::int32_t const *> flag;

    /// (potential) refinement tag for current cycle
    MeshView<std::int32_t const *> flag_tag;

  public:
    /**
     * @brief Default constructor is used in multi-stage initialization. Required by xRage.
     * Prefer `Levels(LevelOptions const &)`.
     */
    Levels();

    /**
     * @brief Resizes Levels to match the new array size
     *
     * @param num_cells Number of real cells on local processor
     * @param newsize The size to grow the array to
     */
    void ResizeLocal(local_index_t num_cells, local_index_t newsize);

    /**
     * @brief Initializes the Levels structure based on options
     *
     * @param options A set of options to use for Levels.
     */
    void Initialize(LevelOptions const &options);

    /**
     * @brief Initializes Levels to be ready for use with a newly started, unrefined simulation.
     *
     * @details Requires that Levels::Initialize be called
     */
    void InitializeLevel1();

    /**
     * @brief
     * Levels is initialized externally by restart. This allows an external caller to mark the
     * cell data as being initialized.
     */
    void MarkAsInitialized();

    /**
     * @brief Updates Levels' internal state to address changes to the cell_level array in Cells
     *
     * @param cells The associated `Cells` structure. Must pass this cells in all future calls to
     * levels.
     */
    void BuildPack(Cells const &cells);

    /**
     * @brief Updates Levels' top level cell information (i.e. ltop) to address changes in cell
     * refinement.
     *
     * @param cells The associated `Cells` structure. Must pass this cells in all future calls to
     * levels.
     */
    void BuildTop(Cells const &cells);

    /**
     * @brief Updates Levels' information on all top level cells (i.e. alltop) to address changes in
     * cell refinement.
     *
     * @param cells The associated `Cells` structure. Must pass this cells in all future calls to
     * levels.
     */
    void SetupAllTop(Cells const &cells);

    /**
     * @brief Allocates communication patterns for exchanging information between cell mothers' and
     * their first child.
     *
     * @param cells The associated `Cells` structure. Must pass this cells in all future calls to
     * levels.
     */
    void MomKidBuild(Cells &cells) { kid_mom_.MomKidBuild(cells); }

    /**
     * @brief Frees mod-kid communication patterns.
     */
    void MomKidFree() { kid_mom_.MomKidFree(); }

    /**
     * @brief Allocates communication patterns for exchanging information between cell mothers' and
     * all of their children.
     *
     * @param cells The associated `Cells` structure. Must pass this cells in all future calls to
     * levels.
     */
    void MomKidsBuild(Cells &cells) { kid_mom_.MomKidsBuild(cells, num_dims_); }

    /**
     * @brief Allocates communication patterns for exchanging information cells and their mothers.
     *
     * @param cells The associated `Cells` structure. Must pass this cells in all future calls to
     * levels.
     */
    void KidMomBuild(Cells &cells) { kid_mom_.KidMomBuild(cells); }

    /**
     * @brief Updates cell_mother and cell_daughter information to match changes to cell_address
     *
     * @param cells The associated `Cells` structure. Must pass this cells in all future calls to
     * levels.
     */
    void ResetMothersAndDaughters(Cells const &cells) { kid_mom_.ResetMothersAndDaughters(cells); }

    /**
     * @brief Updates cell_mother and cell_daughter arrays to account for newly added cells.
     *
     * @param cells The associated `Cells` structure. Must pass this cells in all future calls to
     * levels.
     * @param new_cells The number of new cells
     */
    void ResetMothersAndDaughtersNew(Cells const &cells, local_index_t new_cells) {
        kid_mom_.ResetMothersAndDaughtersNew(cells, new_cells);
    }

    /**
     * @brief Updates arrays to match changes made in recon
     *
     * @param data_length The length of the arrays that are part of the recon
     * @param send_start Start of each send segment
     * @param send_length Length of each send segment
     * @param recv_start Start of each receive segment
     * @param recv_length Length of each receive segment
     * @param indexing The indexing mode used by ReconMove, documents how the provided arrays are
     * indexed.
     */
    void ReconMove(local_index_t data_length,
                   nonstd::span<FortranLocalIndex const> send_start,
                   nonstd::span<local_index_t const> send_length,
                   nonstd::span<FortranLocalIndex const> recv_start,
                   nonstd::span<local_index_t const> recv_length);

    /**
     * @brief Re-packs level information after a recon
     *
     * @param nlow start of cell range
     * @param nhigh end of cell range
     * @param nstep indexing direction (-1 or + 1)
     * @param move_from Cell indices to move from
     * @param move_to Cell indicies to move to
     */
    void ReconPack(local_index_t nlow,
                   local_index_t nhigh,
                   local_diff_t nstep,
                   MeshView<FortranLocalIndex const *> move_from,
                   MeshView<FortranLocalIndex const *> move_to);

    /**
     * @brief Validates that level and cells are in a coherent state
     *
     * @param cells The associated `Cells` structure. Must pass this cells in all future calls to
     * levels.
     */
    void CheckLoHiPackAndLevels(Cells const &cells) {
        kid_mom_.CheckLoHiPackAndLevels(cells, cell_level_);
    }

    /**
     * @brief Sets childl as the child of parentl
     *
     * @param cells The associated `Cells` structure. Must pass this cells in all future calls to
     * levels.
     * @param parentl parent cell
     * @param childl child cell
     */
    void ReconDivParent(Cells const &cells, local_index_t parentl, local_index_t childl) {
        kid_mom_.ReconDivParent(cells, parentl, childl);
    }

    /**
     * @brief Sets parentl as the parent of childl
     *
     * @param cells The associated `Cells` structure. Must pass this cells in all future calls to
     * levels.
     * @param level level of divided cell
     * @param parentl parent cell
     * @param l child cell
     */
    void
    ReconDivChild(Cells const &cells, local_index_t level, local_index_t parentl, local_index_t l);

    /**
     * @brief Erases cell's child
     *
     * @param l cell
     */
    void ClearDaughter(local_index_t l) { kid_mom_.ClearDaughter(l); }

    /**
     * @brief Returns a list of the cells at `level`
     *
     * @param level Cell level
     * @return List of cells
     */
    Kokkos::DualView<FortranLocalIndex const *> CellsAtLevel(local_index_t level) const {
        return kid_mom_.CellsAtLevel(level);
    }

    /**
     * @brief Calls function for each cell at `level`
     *
     * @tparam Fn (local_index_t l) -> void
     * @param level cell level
     * @param function function to execute
     */
    template <typename Fn>
    void ForEachAtLevelSerial(local_index_t level, Fn function) const {
        auto const lopack = kid_mom_.lopack_.view_host();
        auto const lpoint = kid_mom_.lpoint_.view_host();

        for (auto lp = lopack(level); lp < lopack(level + 1); lp++) {
            function(lpoint(lp));
        }
    }

    /**
     * @brief Calls function for each cell at `level` in parallel
     *
     * @tparam ExecutionSpace The execution space to execute function in
     * @tparam MemorySapce The memory space of the cell index array
     * @tparam Fn (local_index_t l) -> void
     * @param level cell level
     * @param function function to execute
     * @param label label of execution
     */
    template <typename ExecutionSpace = Kokkos::DefaultExecutionSpace,
              typename MemorySpace = typename ExecutionSpace::memory_space,
              typename Fn = void>
    void ParallelForEachAtLevel(local_index_t level,
                                Fn function,
                                std::string const &label = "Levels::ParallelForEachAtLevel") const {
        auto const lopack = kid_mom_.lopack_.view_host();
        auto const lpoint = kid_mom_.lpoint_.view<MemorySpace>();

        Kokkos::parallel_for(
            label,
            Kokkos::RangePolicy<ExecutionSpace>(lopack(level), lopack(level + 1)),
            KOKKOS_LAMBDA(local_index_t lp) { function(lpoint(lp)); });
    }

    /**
     * @brief Calls function and maps for each cell at `level` in parallel
     *
     * @tparam ExecutionSpace The execution space to execute function in
     * @tparam MemorySapce The memory space of the cell index array
     * @tparam OutView Type of array to map into
     * @tparam Fn (local_index_t l) -> T
     * @param level cell level
     * @param out output array
     * @param mapping mapping funciton
     * @param label label of execution
     */
    template <typename ExecutionSpace = Kokkos::DefaultExecutionSpace,
              typename MemorySpace = typename ExecutionSpace::memory_space,
              typename OutView = void,
              typename Fn = void>
    void MapCellsAtLevel(local_index_t level,
                         OutView const &out,
                         Fn mapping,
                         std::string const &label = "Levels::MapCellsAtLevel") const {
        ParallelForEachAtLevel<ExecutionSpace, MemorySpace>(
            level,
            KOKKOS_LAMBDA(local_index_t l) {
                out(l) = Kokkos::make_pair<local_index_t, decltype(mapping(l))>(l, mapping(l));
            },
            label);
    }

    /**
     * @brief Calls function and counts which return true for each cell at `level`, if present, in
     * parallel
     *
     * @tparam ExecutionSpace The execution space to execute function in
     * @tparam MemorySapce The memory space of the cell index array
     * @tparam Fn (local_index_t l) -> bool
     * @param level cell level
     * @param num_cell number of cells (only used when level is nullint)
     * @param predicate filter function
     * @return local_index_t Number of cells that matched filter
     */
    template <typename ExecutionSpace = Kokkos::DefaultExecutionSpace,
              typename MemorySpace = typename ExecutionSpace::memory_space,
              typename Fn = void>
    local_index_t CountFilterAtLevel(utility::OptionalInteger<local_index_t> level,
                                     local_index_t num_cell,
                                     Fn predicate) const {
        if (!level) {
            local_index_t num = 0;
            auto cell_level = cell_level_.view<MemorySpace>();
            Kokkos::parallel_reduce(
                "Levels::CountFilterAtLevel[level = nullint]",
                Kokkos::RangePolicy<ExecutionSpace>(0, num_cell),
                KOKKOS_LAMBDA(local_index_t l, local_index_t & num) {
                    if (!cell_level(l) && predicate(l)) ++num;
                },
                num);
            return num;
        } else {
            auto const lopack = kid_mom_.lopack_.view_host();
            auto const lpoint = kid_mom_.lpoint_.view<MemorySpace>();

            local_index_t num = 0;
            Kokkos::parallel_reduce(
                "Levels::CountFilterAtLevel[level >= 0]",
                Kokkos::RangePolicy<ExecutionSpace>(lopack(*level), lopack(*level + 1)),
                KOKKOS_LAMBDA(local_index_t lp, local_index_t & num) {
                    if (predicate(lpoint(lp))) ++num;
                },
                num);
            return num;
        }
    }

    /**
     * @brief Filters cells matched by predicate into `out`
     *
     * @tparam OutView Output view type
     * @tparam Fn (local_index_t l) -> bool
     * @param level cell level
     * @param num_cell number of cells (only used if `level` is nullint)
     * @param out output view
     * @param predicate filter function
     */
    template <typename OutView, typename Fn>
    void FilterAtLevel(utility::OptionalInteger<local_index_t> level,
                       local_index_t num_cell,
                       OutView out,
                       Fn predicate) const {
        if (!level) {
            auto const cell_level = cell_level_.view_host();

            local_index_t n = 0;
            for (local_index_t l = 0; l < num_cell; l++) {
                if (!cell_level(l) && predicate(l)) {
                    out(n++) = l;
                }
            }
        } else {
            auto const lopack = kid_mom_.lopack_.view_host();
            auto const lpoint = kid_mom_.lpoint_.view_host();

            local_index_t n = 0;
            for (local_index_t lp = lopack(*level); lp < lopack(*level + 1); lp++) {
                if (predicate(lpoint(lp))) {
                    out(n++) = lp;
                }
            }
        }
    }

    /**
     * @brief Gets the cells at `level`, or with no `level
     *
     * @tparam View Output array type
     * @param cells The associated `Cells` structure. Must pass this cells in all future calls to
     * levels.
     * @param level cell level (can be nullint)
     * @param out output array
     */
    template <typename View>
    void CollectAtLevel(Cells const &cells,
                        utility::OptionalInteger<local_index_t> level,
                        View out) const {
        EE_DIAG_PRE

        if (!level) {
            EE_ASSERT(out.extent(0) >= cells.num_local_cells());
            FilterAtLevel(0, cells.num_local_cells(), out, [](auto const) { return true; });
        } else {
            EE_ASSERT(out.extent(0) >= NumAtLevel(*level));
            Kokkos::deep_copy(out, CellsAtLevel(*level).view_host());
        }

        EE_DIAG_POST
    }

    /**
     * @brief Clears the un-marked cells at `level`
     *
     * @tparam ExecutionSpace The execution space to execute function in
     * @tparam MemorySapce The memory space of the cell index array
     * @tparam FlagView view of Flag array (should be boolish)
     * @param level cell level
     * @param flags marks cells as true if should not be cleared, false if should be cleared
     * @return local_index_t number of cleared cells
     */
    template <typename ExecutionSpace = Kokkos::DefaultExecutionSpace,
              typename MemorySpace = typename ExecutionSpace::memory_space,
              typename FlagView = void>
    local_index_t ClearAtLevel(local_index_t level, FlagView flags) {
        kid_mom_.SyncCellsAtLevelHost();

        auto cells_at_level_dual = kid_mom_.CellsAtLevel(level);

        cells_at_level_dual.sync<MemorySpace>();
        cell_level_.sync<MemorySpace>();

        auto cells_at_level = cells_at_level_dual.view<MemorySpace>();
        auto cell_level = cell_level_.view<MemorySpace>();

        local_index_t sum = 0;
        Kokkos::parallel_reduce(
            "eap::mesh::Levels::ClearAtLevel",
            Kokkos::RangePolicy<ExecutionSpace>(0, cells_at_level.extent(0)),
            KOKKOS_LAMBDA(local_index_t const i, local_index_t &update) {
                local_index_t const l = cells_at_level(i);

                if (!flags(l)) {
                    cell_level(l) = utility::nullint;
                    update++;
                }
            },
            sum);

        cell_level_.modify<MemorySpace>();

        return sum;
    }

    /**
     * @brief Get the number of cells at `level`
     *
     * @param level cell level
     * @return local_index_t number of cells at `level`
     */
    local_index_t NumAtLevel(local_index_t level) const {
        return kid_mom_.lopack_.view_host()(level + 1) - kid_mom_.lopack_.view_host()(level);
    }

    /**
     * @brief Set the flags of each cell to result of `function`.
     *
     * @tparam ExecutionSpace execution space to run `function` in
     * @tparam Fn (local_index_t l) -> int32_t
     * @param low cell start
     * @param high cell end
     * @param function flagging function
     */
    template <typename ExecutionPolicy = Kokkos::DefaultExecutionSpace, typename Fn = void>
    void SetFlags(local_index_t low, local_index_t high, Fn function) {
        auto const flag = flag_;

        Kokkos::parallel_for(
            "Levels::SetFlags",
            Kokkos::RangePolicy<ExecutionPolicy>(low, high),
            KOKKOS_LAMBDA(local_index_t const l) { flag_(l) = function(l); });
    }

    /**
     * @brief Set the flags of each cell in [low,high) to value
     *
     * @param low cell start
     * @param high cell end
     * @param value flag
     */
    void SetFlagsScalar(local_index_t low, local_index_t high, int32_t value);

    /**
     * @brief Sets all flags to `value`
     *
     * @param value flag
     */
    void SetFlagsAll(int32_t value);

    /**
     * @brief Set the flags of each cell in [low,high) to `value` when function returns a non-zero
     * tag
     *
     * @tparam ExecutionSpace execution space to execute `function` in
     * @tparam Fn (local_index_t l) -> int32_t
     * @param low cell start
     * @param high cell end
     * @param value flag value
     * @param function tagging function
     */
    template <typename ExecutionPolicy = Kokkos::DefaultExecutionSpace, typename Fn = void>
    void SetFlagsScalarWhen(local_index_t low, local_index_t high, int32_t value, Fn function) {
        auto const flag = flag_;

        Kokkos::parallel_for(
            "Levels::SetFlagsScalarWhen",
            Kokkos::RangePolicy<ExecutionPolicy>(low, high),
            KOKKOS_LAMBDA(local_index_t const l) {
                int32_t const tag = function(l);
                if (tag != 0) {
                    flag_(l) = value;
                    flag_tag_(l) = tag;
                }
            });
    }

    /**
     * @brief Set the flags of each cell in `list` to `value`
     *
     * @param list The list of cell indices to flag
     * @param value flag
     */
    void SetFlagsScalarFromList(MeshView<FortranLocalIndex const *> list, int32_t value);

    /**
     * @brief Set the flags of each cell in list to `value` and the tag to the parallel value in
     * `tags`
     *
     * @param list list of cell indices to flag
     * @param value flag
     * @param tags tags for each cell index, parallel to `list`
     */
    void SetFlagsScalarAndTagsFromList(MeshView<FortranLocalIndex const *> list,
                                       int32_t value,
                                       MeshView<int32_t const *> tags);

    /**
     * @brief Set flags using (cell,flag) tuples
     *
     * @param tups (cell,flag) tuples
     */
    void SetFlagsFromTuples(MeshView<Kokkos::pair<FortranLocalIndex, int32_t> const *> tups);

    /**
     * @brief Set flag tags using (cell,tag) tuples
     *
     * @param tups (cell,tag) tuples
     */
    void SetFlagTagsFromTuples(MeshView<Kokkos::pair<FortranLocalIndex, int32_t> const *> tups);

    /**
     * @brief Set flags using `cell` in (cell,tag) tuple to `value`
     *
     * @param tups (cell,flag) tuples
     * @param value flag
     */
    void
    SetFlagsFromTuplesKeyAndScalar(MeshView<Kokkos::pair<FortranLocalIndex, int32_t> const *> tups,
                                   int32_t value);

    /**
     * @brief Set flag tags using `cell` in (cell,tag) tuple to `value`
     *
     * @param tups (cell,flag) tuples
     * @param value tag
     */
    void SetFlagTagsFromTuplesKeyAndScalar(
        MeshView<Kokkos::pair<FortranLocalIndex, int32_t> const *> tups, int32_t value);

    /**
     * @brief Get values in kid cells at `level` from their mothers
     *
     * @tparam InputView Input array type
     * @tparam OutputView Output array type
     * @param level cell level
     * @param input input array (size numcell)
     * @param output output array (size numcell)
     */
    template <typename InputView, typename OutputView>
    void KidGet(local_index_t level, InputView const &input, OutputView &output) {
        kid_mom_.KidGet(level, input, output);
    }

    /**
     * @brief Puts values in kid cells at `level` into their mother cells
     *
     * @tparam InputView Input array type
     * @tparam OutputView Output array type
     * @param level cell level
     * @param input input array (size numcell)
     * @param output output array (size numcell)
     */
    template <typename InputView, typename OutputView>
    void KidPut(local_index_t level, InputView const &input, OutputView &output) {
        kid_mom_.KidPut(level, input, output);
    }

    /**
     * @brief Puts 2D values in kid cells at `level` into their mother cells
     *
     * @tparam InputView Input array type
     * @tparam OutputView Output array type
     * @param level cell level
     * @param input input array (size numcell)
     * @param output output array (size numcell)
     */
    template <typename InputView, typename OutputView>
    void KidPutV(local_index_t level, InputView const &input, OutputView &output) {
        kid_mom_.KidPutV(level, input, output);
    }

    /**
     * @brief Puts 2D columnar values in kid cells at `level` into their mother cells
     *
     * @tparam InputView Input array type
     * @tparam OutputView Output array type
     * @param level cell level
     * @param input input array (size numcell)
     * @param output output array (size numcell)
     */
    template <typename InputView, typename OutputView>
    void KidPutVInv(local_index_t level, InputView const &input, OutputView &output) {
        kid_mom_.KidPutVInv(level, input, output);
    }

    /**
     * @brief Gets values in mother cells at `level` from their first children.
     *
     * @tparam InputView Input array type
     * @tparam OutputView Output array type
     * @param level cell level
     * @param input input array (size numcell)
     * @param output output array (size numcell)
     */
    template <typename InputView, typename OutputView>
    void MomGet(local_index_t level, InputView const &input, OutputView &output) {
        // kid_.KidGet(level, input, output);
        kid_mom_.MomGet(level, input, output);
    }

    /**
     * @brief Updates the max level array by apply the "defined zone resolution" information in DZN
     *
     * @param dzn defined zone resolution information
     * @param numdim number of dimensions in program run
     * @param cells cells array
     * @param time elapsed time since simulation start
     * @param kode kode options to apply to DZN
     */
    void ApplyDZNs(DefinedZoneResolutionInfo const &dzn,
                   std::uint8_t numdim,
                   Cells const &cells,
                   double time,
                   Kode kode);

    /**
     * @brief Sets num_pack
     *
     * @param num_pack num_pack value
     */
    void SetNumPack(local_index_t num_pack) { kid_mom_.SetNumPack(num_pack); }

    /**
     * @brief Set the max level for each cell from the (cell,mxlevel) tuple list
     *
     * @param tups (cell,mxlevel) tuples
     */
    void
    SetLevelMaxFromTuples(MeshView<Kokkos::pair<FortranLocalIndex, local_index_t> const *> tups);

  private:
    /* Routines */
    local_index_t CountChunks(Cells const &cells) const;

    std::vector<std::array<local_index_t, 2>> PopulateChunks(Cells const &cells) const;

    struct GeneratedChunks {
        local_index_t chunk_count;
        MeshView<utility::FortranIndex<local_index_t> *> chunk_ids;
    };

    GeneratedChunks GenerateChunkIds(Cells const &cells) const;

    enum class State {
        /**
         * @brief
         * `Levels` begins in this state, before initialized as level 1 or by restart.
         *
         * @details
         * -> Ready when resized_cell_info_, initialized_level_info_, and initialized_cell_info_
         */
        Constructed,

        /**
         * @brief
         * Level state is in a consistent state for simulation cycling
         *
         * @details
         * Requires: resized_cell_info_, initialized_level_info_, and initialized_cell_info_
         *
         * -> Reconstruction: resets resized_cell_info_, initialized_level_info_, and
         * initialized_cell_info_
         */
        Ready,

        /**
         * @brief
         * Levels is in the process of being transitioned for reconstruction
         *
         * @details
         * -> Ready when resized_cell_info_, initialized_level_info_, and initialized_cell_info_
         */
        Reconstruction
    };

    static constexpr nonstd::string_view StateString(State state) {
#define STATE_CASE(state)                                                                          \
    case state:                                                                                    \
        return #state

        switch (state) {
            STATE_CASE(State::Constructed);
            STATE_CASE(State::Ready);
            STATE_CASE(State::Reconstruction);
        default:
            return "State::Unknown";
        }
#undef STATE_CASE
    }

    template <typename Collection>
    static std::string StatesString(Collection const &collection);

    void CheckState(std::initializer_list<State> valid_states) const;
    void StateTransition(std::initializer_list<State> from, State to);
    void TryTransitionToReady();

    /* Data Members */
    /// Internally tracks the current state of Levels to ensure correct state transitions.
    State state_ = State::Constructed;
    bool resized_cell_info_ = false;
    bool initialized_level_info_ = false;
    bool initialized_cell_info_ = false; // Requires resized_cell_info_ to transition to

    /// Current number of cells, including refined and unrefined, on this processor
    local_index_t num_cells_ = 0;

    KidMom kid_mom_;

    MeshDualView<OptionalFortranLocalIndex *> cell_level_;

    MeshView<double[MAX_LEVELS]> area_, gdxx_, dxyzmn_, dtsize_;

    // Ideally this would be `MeshView<double[MAX_LEVELS] *>`, but that's not supported by Ko
    // Kokkos::View
    MeshView<double[MAX_LEVELS][3]> sides_, sizes_, gsizes_;

    MeshView<local_index_t *> levelmx_;

    /// Triggers refinement (>0) or coarsening (<0) for current cycle
    MeshView<std::int32_t *> flag_;

    /// (potential) refinement tag for current cycle
    MeshView<std::int32_t *> flag_tag_;

    /// Refinement tag tied to current level
    MeshView<std::int32_t *> amr_tag_;

    /// Number of real cells (not ghost) on this processor
    local_index_t num_top_ = 0;

    /// Number of real cells + ghost cells on this processor
    local_index_t all_num_top_ = 0;

    /// Number of real cells on all processors
    global_index_t sum_num_top_ = 0;

    std::int8_t num_dims_ = 0;

    local_index_t chunk_count_ = 0;

    double average_run_length_ = 0.0;

    MeshView<FortranLocalIndex *> chunk_ids_;
};
} // namespace mesh
} // namespace eap

#endif // EAP_MESH_LEVELS_HPP_