/**
 * @file mesh-ffi-levels.cpp
 *
 * @brief FFI for eap::mesh::Levels
 * @date 2019-04-22
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <mesh-ffi-levels.h>

// Internal Includes
#include <abi-fortran_interop_impl.hpp>
#include <comm-ffi-concretize.hpp>
#include <utility-ffi-interop-addressing.hpp>

// Local Includes
#include <mesh-ffi-interop.hpp>

using eap::HostMemorySpace;
using eap::comm::ffi::ConcretizeViews;
using eap::comm::ffi::ToKokkosView;
using eap::mesh::Levels;
using eap::mesh::MeshViewUnmanaged;
using eap::mesh::ffi::CellsFromFFI;
using eap::mesh::ffi::DefinedZoneResolutionInfoFromFFI;
using eap::mesh::ffi::LevelsFromFFI;
using eap::utility::nullint;
using eap::utility::OptionalInteger;
using eap::utility::ffi::FortranLocalAddressSpan;
using eap::utility::ffi::LocalAddressSpan;
using eap::utility::kokkos::ViewFromNdarray;
using eap::utility::kokkos::ViewToNdarray;
using Kokkos::Serial;

using namespace eap;

#define LEVEL_CONVERT(level)                                                                       \
    EE_CHECK(level_convert(level).value(), "level (= " << level << ") must be a valid index")

namespace {
OptionalInteger<local_index_t> level_convert(int32_t level) {
    if (level < 0) {
        return nullint;
    } else {
        return level;
    }
}
} // namespace

FROM_HANDLE_INTERNAL_IMPL_NON_CONST_ONLY(mesh, cpp_levels, Levels)

FC_INTEROP_CREATE_C_IMPL(mesh, cpp_levels, Levels())
FC_INTEROP_DELETE_C_IMPL(mesh, cpp_levels)

EXTERN_C void eap_mesh_cpp_levels_initialize(eap_mesh_cpp_levels_t *levels,
                                             eap_mesh_cpp_levels_opts const *options) {
    EAP_EXTERN_PRE

    eap::mesh::LevelOptions const level_opts{
        options->num_dim,
        {options->cell_size[0], options->cell_size[1], options->cell_size[2]},
        eap::mesh::KidMomOptions{
            options->kid_mom_use_s2s, options->mom_kid_use_s2s, options->mom_kids_use_s2s}};

    LevelsFromFFI(levels)->Initialize(level_opts);

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_initialize_level1(eap_mesh_cpp_levels_t *levels) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->InitializeLevel1();

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_build_pack(eap_mesh_cpp_levels_t *levels,
                                             eap_mesh_cpp_cells_t const *cells) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->BuildPack(*CellsFromFFI(cells));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_build_top(eap_mesh_cpp_levels_t *levels,
                                            eap_mesh_cpp_cells_t const *cells) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->BuildTop(*CellsFromFFI(cells));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_reset_mothers_and_daughters(eap_mesh_cpp_levels_t *levels,
                                                              eap_mesh_cpp_cells_t const *cells) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->ResetMothersAndDaughters(*CellsFromFFI(cells));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_reset_mothers_and_daughters_new(eap_mesh_cpp_levels_t *levels,
                                                                  eap_mesh_cpp_cells_t const *cells,
                                                                  eap_local_t new_cells) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->ResetMothersAndDaughtersNew(*CellsFromFFI(cells), new_cells);

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_setup_all_top(eap_mesh_cpp_levels_t *levelsffi,
                                                eap_mesh_cpp_cells_t const *cells,
                                                eap_mesh_cpp_levels_setup_all_top_fields *fields) {
    EAP_EXTERN_PRE

    auto &levels = *LevelsFromFFI(levelsffi);

    levels.SetupAllTop(*CellsFromFFI(cells));

    if (fields) {
        fields->all_top = ViewToNdarray(levels.all_top);
    }

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_resize_local(eap_mesh_cpp_levels_t *levelsffi,
                                               eap_local_t num_cells,
                                               eap_local_t newsize,
                                               eap_mesh_cpp_levels_resize_local_fields *fields) {
    EAP_EXTERN_PRE

    auto &levels = *LevelsFromFFI(levelsffi);

    levels.ResizeLocal(num_cells, newsize);

    if (fields) {
        fields->cell_daughter = ViewToNdarray(levels.cell_daughter);
        fields->cell_mother = ViewToNdarray(levels.cell_mother);
        fields->ltop = ViewToNdarray(levels.ltop);
        fields->cell_level = ViewToNdarray(levels.cell_level.view_host());
        fields->flag = ViewToNdarray(levels.flag);
        fields->flag_tag = ViewToNdarray(levels.flag_tag);
    }

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_recon_move_f(eap_mesh_cpp_levels_t *levels,
                                               eap_local_t data_length,
                                               size_t length,
                                               eap_local_t const *send_start,
                                               eap_local_t const *send_length,
                                               eap_local_t const *recv_start,
                                               eap_local_t const *recv_length) {

    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->ReconMove(data_length,
                                     FortranLocalAddressSpan(send_start, length),
                                     LocalAddressSpan(send_length, length),
                                     FortranLocalAddressSpan(recv_start, length),
                                     LocalAddressSpan(recv_length, length));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_foreach_at_level(eap_mesh_cpp_levels_t const *levels,
                                                   eap_local_t const level,
                                                   void *context,
                                                   void (*function)(void *context,
                                                                    eap_local_t cell)) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->ForEachAtLevelSerial(
        LEVEL_CONVERT(level),
        [context, function](auto const l) { function(context, static_cast<eap_local_t>(l)); });

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_map_at_level_f(eap_mesh_cpp_levels_t const *levels,
                                                 eap_local_t const level,
                                                 abi_ndarray_t const out,
                                                 void *context,
                                                 eap_local_t (*function)(void *context,
                                                                         eap_local_t cell)) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->MapCellsAtLevel<Serial, HostMemorySpace>(
        LEVEL_CONVERT(level),
        ViewFromNdarray<Kokkos::pair<FortranLocalIndex, int32_t> *>(out),
        KOKKOS_LAMBDA(local_index_t const l) {
            return function(context, static_cast<eap_local_t>(l));
        });

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_count_filter_at_level(eap_mesh_cpp_levels_t const *levels,
                                                        eap_mesh_cpp_cells_t const *cells,
                                                        eap_local_t const level,
                                                        void *context,
                                                        bool (*function)(void *context,
                                                                         eap_local_t cell),
                                                        eap_local_t *count) {
    EAP_EXTERN_PRE

    auto const filter_count = LevelsFromFFI(levels)->CountFilterAtLevel<Serial, HostMemorySpace>(
        level_convert(level),
        CellsFromFFI(cells)->num_local_cells(),
        KOKKOS_LAMBDA(local_index_t const l) {
            return function(context, static_cast<eap_local_t>(l));
        });

    *count = static_cast<eap_local_t>(filter_count);

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_filter_at_level_f(eap_mesh_cpp_levels_t const *levels,
                                                    eap_mesh_cpp_cells_t const *cells,
                                                    eap_local_t const level,
                                                    abi_ndarray_t const out,
                                                    void *context,
                                                    bool (*function)(void *context,
                                                                     eap_local_t cell)) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->FilterAtLevel(level_convert(level),
                                         CellsFromFFI(cells)->num_local_cells(),
                                         ViewFromNdarray<FortranLocalIndex *>(out),
                                         [context, function](auto const l) {
                                             return function(context, static_cast<eap_local_t>(l));
                                         });

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_collect_at_level_f(eap_mesh_cpp_levels_t const *levels,
                                                     eap_mesh_cpp_cells_t const *cells,
                                                     eap_local_t const level,
                                                     abi_ndarray_t const out) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->CollectAtLevel(
        *CellsFromFFI(cells), level_convert(level), ViewFromNdarray<FortranLocalIndex *>(out));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_clear_at_level(eap_mesh_cpp_levels_t *levels,
                                                 eap_local_t const level,
                                                 abi_ndarray_t const flags,
                                                 eap_local_t *num_cleared) {
    EAP_EXTERN_PRE

    auto const num = LevelsFromFFI(levels)->ClearAtLevel<Serial, HostMemorySpace>(
        LEVEL_CONVERT(level), ViewFromNdarray<bool const *>(flags));

    if (num_cleared) {
        *num_cleared = static_cast<eap_local_t>(num);
    }

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_num_at_level(eap_mesh_cpp_levels_t const *levels,
                                               eap_local_t const level,
                                               eap_local_t *num) {
    EAP_EXTERN_PRE

    EE_ASSERT_NOT_NULL(num);

    *num = static_cast<eap_local_t>(LevelsFromFFI(levels)->NumAtLevel(LEVEL_CONVERT(level)));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_recon_pack(eap_mesh_cpp_levels_t *levels,
                                             eap_local_t nlow,
                                             eap_local_t nhigh,
                                             eap_local_t nstep,
                                             eap_local_t move_num,
                                             eap_local_t const *move_from,
                                             eap_local_t const *move_to) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->ReconPack(
        static_cast<local_index_t>(nlow),
        static_cast<local_index_t>(nhigh),
        static_cast<local_diff_t>(nstep),
        MeshViewUnmanaged<FortranLocalIndex const *>(
            reinterpret_cast<FortranLocalIndex const *>(move_from), move_num),
        MeshViewUnmanaged<FortranLocalIndex const *>(
            reinterpret_cast<FortranLocalIndex const *>(move_to), move_num));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_set_flags(eap_mesh_cpp_levels_t *levels,
                                            eap_local_t low,
                                            eap_local_t high,
                                            void *context,
                                            int32_t (*function)(void *, eap_local_t)) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->SetFlags<Serial>(
        low, high, KOKKOS_LAMBDA(local_index_t const i) {
            return function(context, static_cast<eap_local_t>(i));
        });

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_set_flags_scalar(eap_mesh_cpp_levels_t *levels,
                                                   eap_local_t low,
                                                   eap_local_t high,
                                                   int32_t value) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->SetFlagsScalar(low, high, value);

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_set_flags_all(eap_mesh_cpp_levels_t *levels, int32_t value) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->SetFlagsAll(value);

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_set_flags_scalar_when(eap_mesh_cpp_levels_t *levels,
                                                        eap_local_t low,
                                                        eap_local_t high,
                                                        int32_t value,
                                                        void *context,
                                                        int32_t (*function)(void *, eap_local_t)) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->SetFlagsScalarWhen<Serial>(
        low, high, value, KOKKOS_LAMBDA(local_index_t const i) {
            return function(context, static_cast<eap_local_t>(i));
        });

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_set_flags_scalar_from_list(eap_mesh_cpp_levels_t *levels,
                                                             size_t list_size,
                                                             eap_local_t const *list,
                                                             int32_t value) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->SetFlagsScalarFromList(
        MeshViewUnmanaged<FortranLocalIndex const *>(
            reinterpret_cast<FortranLocalIndex const *>(list), list_size),
        value);

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_set_flags_scalar_and_tags_from_list(eap_mesh_cpp_levels_t *levels,
                                                                      size_t list_size,
                                                                      eap_local_t const *list,
                                                                      int32_t value,
                                                                      size_t tags_size,
                                                                      int32_t const *tags) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->SetFlagsScalarAndTagsFromList(
        MeshViewUnmanaged<FortranLocalIndex const *>(
            reinterpret_cast<FortranLocalIndex const *>(list), list_size),
        value,
        MeshViewUnmanaged<int32_t const *>(tags, tags_size));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_set_flags_from_tuples(eap_mesh_cpp_levels_t *levels,
                                                        size_t tups_size,
                                                        eap_mesh_loc_val const *tups) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->SetFlagsFromTuples(
        MeshViewUnmanaged<Kokkos::pair<FortranLocalIndex, int32_t> const *>(
            reinterpret_cast<Kokkos::pair<FortranLocalIndex, int32_t> const *>(tups), tups_size));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_set_flag_tags_from_tuples(eap_mesh_cpp_levels_t *levels,
                                                            size_t tups_size,
                                                            eap_mesh_loc_val const *tups) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->SetFlagTagsFromTuples(
        MeshViewUnmanaged<Kokkos::pair<FortranLocalIndex, int32_t> const *>(
            reinterpret_cast<Kokkos::pair<FortranLocalIndex, int32_t> const *>(tups), tups_size));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_set_flags_from_tuples_key_and_scalar(
    eap_mesh_cpp_levels_t *levels, size_t tups_size, eap_mesh_loc_val const *tups, int32_t value) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->SetFlagsFromTuplesKeyAndScalar(
        MeshViewUnmanaged<Kokkos::pair<FortranLocalIndex, int32_t> const *>(
            reinterpret_cast<Kokkos::pair<FortranLocalIndex, int32_t> const *>(tups), tups_size),
        value);

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_set_flag_tags_from_tuples_key_and_scalar(
    eap_mesh_cpp_levels_t *levels, size_t tups_size, eap_mesh_loc_val const *tups, int32_t value) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->SetFlagTagsFromTuplesKeyAndScalar(
        MeshViewUnmanaged<Kokkos::pair<FortranLocalIndex, int32_t> const *>(
            reinterpret_cast<Kokkos::pair<FortranLocalIndex, int32_t> const *>(tups), tups_size),
        value);

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_mom_kid_build(eap_mesh_cpp_levels_t *levels,
                                                eap_mesh_cpp_cells_t *cells) {
    EAP_EXTERN_PRE
    LevelsFromFFI(levels)->MomKidBuild(*CellsFromFFI(cells));
    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_mom_kid_free(eap_mesh_cpp_levels_t *levels) {
    EAP_EXTERN_PRE
    LevelsFromFFI(levels)->MomKidFree();
    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_kid_mom_build(eap_mesh_cpp_levels_t *levels,
                                                eap_mesh_cpp_cells_t *cells) {
    EAP_EXTERN_PRE
    LevelsFromFFI(levels)->KidMomBuild(*CellsFromFFI(cells));
    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_mom_kids_build(eap_mesh_cpp_levels_t *levels,
                                                 eap_mesh_cpp_cells_t *cells) {
    EAP_EXTERN_PRE
    LevelsFromFFI(levels)->MomKidsBuild(*CellsFromFFI(cells));
    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_check_lohipack_and_levels(eap_mesh_cpp_levels_t *levels,
                                                            eap_mesh_cpp_cells_t const *cells) {
    EAP_EXTERN_PRE
    LevelsFromFFI(levels)->CheckLoHiPackAndLevels(*CellsFromFFI(cells));
    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_recon_div_parent(eap_mesh_cpp_levels_t *levels,
                                                   eap_mesh_cpp_cells_t const *cells,
                                                   eap_local_t parentl,
                                                   eap_local_t childl) {
    EAP_EXTERN_PRE
    LevelsFromFFI(levels)->ReconDivParent(*CellsFromFFI(cells),
                                          static_cast<local_index_t>(parentl),
                                          static_cast<local_index_t>(childl));
    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_recon_div_child(eap_mesh_cpp_levels_t *levels,
                                                  eap_mesh_cpp_cells_t const *cells,
                                                  eap_local_t level,
                                                  eap_local_t parentl,
                                                  eap_local_t l) {
    EAP_EXTERN_PRE
    LevelsFromFFI(levels)->ReconDivChild(*CellsFromFFI(cells),
                                         LEVEL_CONVERT(level),
                                         static_cast<local_index_t>(parentl),
                                         static_cast<local_index_t>(l));
    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_clear_daughter(eap_mesh_cpp_levels_t *levels, int32_t l) {
    EAP_EXTERN_PRE
    LevelsFromFFI(levels)->ClearDaughter(l);
    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_kid_get(eap_mesh_cpp_levels_t *levels,
                                          eap_local_t level,
                                          abi_ndarray_t const *input,
                                          abi_ndarray_t const *output,
                                          eap_comm_datatype_t datatype) {
    EAP_EXTERN_PRE

    auto const level_index = LEVEL_CONVERT(level);

    ConcretizeViews(
        datatype,
        [=](auto input, auto output) { LevelsFromFFI(levels)->KidGet(level_index, input, output); },
        ToKokkosView<void const *>(*input),
        ToKokkosView<void *>(*output));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_kid_put(eap_mesh_cpp_levels_t *levels,
                                          eap_local_t level,
                                          abi_ndarray_t const *input,
                                          abi_ndarray_t const *output,
                                          eap_comm_datatype_t datatype) {
    EAP_EXTERN_PRE

    auto const level_index = LEVEL_CONVERT(level);

    ConcretizeViews(
        datatype,
        [=](auto input, auto output) { LevelsFromFFI(levels)->KidPut(level_index, input, output); },
        ToKokkosView<void const *>(*input),
        ToKokkosView<void *>(*output));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_kid_put_v(eap_mesh_cpp_levels_t *levels,
                                            eap_local_t level,
                                            abi_ndarray_t const *input,
                                            abi_ndarray_t const *output,
                                            eap_comm_datatype_t datatype) {
    EAP_EXTERN_PRE

    auto const level_index = LEVEL_CONVERT(level);

    ConcretizeViews(
        datatype,
        [=](auto input, auto output) {
            LevelsFromFFI(levels)->KidPutV(level_index, input, output);
        },
        ToKokkosView<void const **>(*input),
        ToKokkosView<void **>(*output));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_kid_put_inv(eap_mesh_cpp_levels_t *levels,
                                              eap_local_t level,
                                              abi_ndarray_t const *input,
                                              abi_ndarray_t const *output,
                                              eap_comm_datatype_t datatype) {
    EAP_EXTERN_PRE

    auto const level_index = LEVEL_CONVERT(level);

    ConcretizeViews(
        datatype,
        [=](auto input, auto output) {
            LevelsFromFFI(levels)->KidPutVInv(level_index, input, output);
        },
        ToKokkosView<void const **>(*input),
        ToKokkosView<void **>(*output));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_mom_get(eap_mesh_cpp_levels_t *levels,
                                          eap_local_t level,
                                          abi_ndarray_t const *input,
                                          abi_ndarray_t const *output,
                                          eap_comm_datatype_t datatype) {
    EAP_EXTERN_PRE

    auto const level_index = LEVEL_CONVERT(level);

    ConcretizeViews(
        datatype,
        [=](auto input, auto output) { LevelsFromFFI(levels)->MomGet(level_index, input, output); },
        ToKokkosView<void const *>(*input),
        ToKokkosView<void *>(*output));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_mark_as_initialized(eap_mesh_cpp_levels_t *levels) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->MarkAsInitialized();

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_apply_dzns(eap_mesh_cpp_levels_t *levels,
                                             eap_mesh_dzn_t const *dzn,
                                             int numdim,
                                             eap_mesh_cpp_cells_t const *cells,
                                             double time,
                                             int kode) {
    EAP_EXTERN_PRE

    LevelsFromFFI(levels)->ApplyDZNs(*DefinedZoneResolutionInfoFromFFI(dzn),
                                     static_cast<std::uint8_t>(numdim),
                                     *CellsFromFFI(cells),
                                     time,
                                     static_cast<eap::mesh::Kode>(kode));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_set_num_pack(eap_mesh_cpp_levels_t *levels,
                                               eap_local_t num_pack) {
    EAP_EXTERN_PRE
    LevelsFromFFI(levels)->SetNumPack(num_pack);
    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_cpp_levels_set_level_max_from_tuples(eap_mesh_cpp_levels_t *levels,
                                                            size_t tups_size,
                                                            eap_mesh_loc_val const *tups) {
    EAP_EXTERN_PRE
    LevelsFromFFI(levels)->SetLevelMaxFromTuples(
        MeshViewUnmanaged<Kokkos::pair<FortranLocalIndex, local_index_t> const *>(
            reinterpret_cast<Kokkos::pair<FortranLocalIndex, local_index_t> const *>(tups),
            tups_size));
    EAP_EXTERN_POST
}