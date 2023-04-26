/**
 * @file mesh-kidmom.hpp
 *
 * @brief KidMom tracks relationships between mother and daughter cells
 * @date 2019-02-15
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_KIDMOM_HPP_
#define EAP_MESH_KIDMOM_HPP_

// STL Includes
#include <cstddef>
#include <utility>

// Third Party Includes
#include <optional>
#include <span>

// Internal Includes
#include <comm-token.hpp>
#include <utility-offset_integer.hpp>

// Local Includes
#include "mesh-cells.hpp"
#include "mesh-constants.hpp"
#include "mesh-forward.hpp"
#include "mesh-types.hpp"

namespace eap {
namespace mesh {
struct KidMomOptions {
    /// Uses SomeToSome when initializing the kid_mom tokens
    bool kid_mom_use_s2s;
    /// Uses SomeToSome when initializing the mom_kid tokens
    bool mom_kid_use_s2s;
    /// Uses SomeToSome when initializing the mom_kids tokens
    bool mom_kids_use_s2s;
};

class KidMom {
    friend class Levels;

  public:
    /**
     * @brief Construct a partially initialized KidMom object. Requires multi-stage initialization.
     * `ResizeLocal` should be called next, followed by `Initialize`.
     */
    KidMom();

    /**
     * @brief Resizes KidMom to match the new local size.
     *
     * @param num_cells Number of local cells
     * @param newsize New size of arrays
     */
    void ResizeLocal(local_index_t num_cells, local_index_t newsize);

    /**
     * @brief Initializes the KidMom object with the options specified by `KidMomOptions`
     *
     * @param options Options for configuring KidMom.
     */
    void Initialize(KidMomOptions const &options);

    /**
     * @brief
     */
    void InitializeLevel1();

    std::pair<local_index_t, global_index_t> BuildTop(Cells const &cells);

    local_index_t SetupAllTop(Cells const &cells, local_index_t num_top);

    void BuildPack(Cells const &cells, MeshDualView<OptionalFortranLocalIndex *> cell_level);

    void KidMomBuild(Cells &cells);

    void MomKidBuild(Cells &cells);

    void MomKidFree();

    void MomKidsBuild(Cells &cells, int num_dims);

    void ResetMothersAndDaughters(Cells const &cells);

    void ResetMothersAndDaughtersNew(Cells const &cells, local_index_t new_cells);

    void ReconMove(std::span<FortranLocalIndex const> send_start,
                   std::span<local_index_t const> send_length,
                   std::span<FortranLocalIndex const> recv_start,
                   std::span<local_index_t const> recv_length);

    template <typename InputView,
              typename OutputView,
              typename ValueType = typename OutputView::non_const_value_type>
    void KidGet(local_index_t level, InputView const &input, OutputView &output) {
        kid_token_[level].Get<InputView, OutputView, ValueType>(
            eap::comm::TokenOperation::Copy, input, output);
    }

    template <typename InputView,
              typename OutputView,
              typename ValueType = typename OutputView::non_const_value_type>
    void KidPut(local_index_t level, InputView const &input, OutputView &output) {
        kid_token_[level].Put<InputView, OutputView, ValueType>(
            eap::comm::TokenOperation::Copy, input, output);
    }

    template <typename InputView,
              typename OutputView,
              typename ValueType = typename OutputView::non_const_value_type>
    void KidPutV(local_index_t level, InputView const &input, OutputView &output) {
        kid_token_[level].PutV<InputView, OutputView, ValueType>(
            eap::comm::TokenOperation::Copy, input, output);
    }

    template <typename InputView,
              typename OutputView,
              typename ValueType = typename OutputView::non_const_value_type>
    void KidPutVInv(local_index_t level, InputView const &input, OutputView &output) {
        kid_token_[level].PutVInv<InputView, OutputView, ValueType>(
            eap::comm::TokenOperation::Copy, input, output);
    }

    template <typename InputView,
              typename OutputView,
              typename ValueType = typename OutputView::non_const_value_type>
    void MomGet(local_index_t level, InputView const &input, OutputView &output) {
        mom_kids_token_[level].Get<InputView, OutputView, ValueType>(
            eap::comm::TokenOperation::Copy, input, output);
    }

    /* Accessors */
    auto const &ltop() const { return ltop_; }

    void SyncCellsAtLevelHost();

    std::span<FortranLocalIndex const> CellsAtLevelHost(local_index_t level) const {
        EE_PRELUDE

        static_assert(
            std::is_same<decltype(KidMom::lpoint_)::array_layout, Kokkos::LayoutLeft>::value,
            "KidMomLevelCellRange assumes contiguous layout of lpoint_");

        EE_ASSERT(!lopack_.need_sync_host());
        EE_ASSERT(!lpoint_.need_sync_host());

        auto lopack = lopack_.view_host();
        auto lpoint = lpoint_.view_host();

        return std::span<FortranLocalIndex const>(&lpoint(lopack(level)),
                                                     &lpoint(lopack(level + 1)));
    }

    MeshDualView<FortranLocalIndex const *> CellsAtLevel(local_index_t level) const {
        EE_PRELUDE

        EE_ASSERT(!lopack_.need_sync_host());

        auto lopack = lopack_.view_host();

        return Kokkos::subview(lpoint_, Kokkos::make_pair(lopack(level), lopack(level + 1)));
    }

    void SetNumPack(local_index_t num_pack) { num_pack_ = num_pack; }

  private:
    /* Routines */

    void CheckLoHiPackAndLevels(Cells const &cells,
                                MeshDualView<OptionalFortranLocalIndex *> cell_level);

    void ReconDivParent(Cells const &cells, local_index_t parentl, local_index_t childl) {
        cell_daughter_(parentl) = cells.cell_address()(childl);
    }

    void ReconDivChild(local_index_t l, local_index_t parent_address) {
        cell_mother_(l) = parent_address;
        cell_daughter_(l) = 0;
    }

    void ClearDaughter(local_index_t l) { cell_daughter_(l) = 0; }

    bool IsTop(Cells const &cells, local_index_t cell) const;

    std::vector<int> KidMomBuildToPes(eap::comm::TokenBuilder const &token_builder) const;

    std::vector<int> MomKidBuildToPes(eap::comm::TokenBuilder const &token_builder) const;

    std::vector<int> MomKidsBuildToPes(eap::comm::TokenBuilder const &token_builder,
                                       int num_dims) const;

    /* Data Members */

    KidMomOptions options_;

    local_index_t num_pack_;
    local_index_t max_num_pack_;
    local_index_t num_levels_;

    /// cell_mother_(i) is the mother of cell i (Global Address)
    MeshView<OptionalFortranGlobalIndex *> cell_mother_;

    /// cell_daughter_(i) is the "first" daughter of cell i (Global Address)
    MeshView<OptionalFortranGlobalIndex *> cell_daughter_;

    MeshView<FortranLocalIndex *> ltop_;

    /// Number of real cells + ghost cells on this processor
    local_index_t all_num_top_ = 0;

    MeshView<FortranLocalIndex *> all_top_;

    /// Cells at each level, indexed by lopack_ and hipack_
    MeshDualView<FortranLocalIndex *> lpoint_;

    /// Starting index for cells at this level in lpoint_
    MeshDualView<local_index_t *> lopack_;

    /// Up to eap::mesh::MAX_LEVELS tokens
    std::vector<comm::Token> kid_token_;

    /// Up to eap::mesh::MAX_LEVELS tokens
    std::vector<comm::Token> mom_token_;

    /// Up to eap::mesh::MAX_LEVELS * numkids
    std::vector<comm::Token> mom_kids_token_;
};
} // namespace mesh
} // namespace eap

#endif // EAP_MESH_KIDMOM_HPP_