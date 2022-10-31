/**
 * @file mesh-ffi-levels.h
 *
 * @brief FFI for eap::mesh::Levels
 * @date 2019-04-22
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_FFI_LEVELS_H_
#define EAP_MESH_FFI_LEVELS_H_

// STL Includes
#include <stdint.h>

// Internal Includes
#include <abi-extern.h>
#include <abi-fortran_interop.h>
#include <abi-ndarray.h>
#include <comm-ffi-types.h>
#include <utility-ffi-addressing.h>

// Local Includes
#include <mesh-ffi-cells.h>
#include <mesh-ffi-handles.h>
#include <mesh-ffi-types.h>

EXTERN_C_BEGIN

typedef struct eap_mesh_cpp_levels_opts {
    int8_t num_dim;
    double cell_size[3];
    bool kid_mom_use_s2s;
    bool mom_kid_use_s2s;
    bool mom_kids_use_s2s;
} eap_mesh_cpp_levels_opts;

FC_INTEROP_CREATE_C_DECL(mesh, cpp_levels);
FC_INTEROP_DELETE_C_DECL(mesh, cpp_levels);

void eap_mesh_cpp_levels_initialize(eap_mesh_cpp_levels_t *levels,
                                    eap_mesh_cpp_levels_opts const *options);
void eap_mesh_cpp_levels_initialize_level1(eap_mesh_cpp_levels_t *levels);

void eap_mesh_cpp_levels_build_pack(eap_mesh_cpp_levels_t *levels,
                                    eap_mesh_cpp_cells_t const *cells);
void eap_mesh_cpp_levels_build_top(eap_mesh_cpp_levels_t *levels,
                                   eap_mesh_cpp_cells_t const *cells);

void eap_mesh_cpp_levels_reset_mothers_and_daughters(eap_mesh_cpp_levels_t *levels,
                                                     eap_mesh_cpp_cells_t const *cells);

void eap_mesh_cpp_levels_reset_mothers_and_daughters_new(eap_mesh_cpp_levels_t *levels,
                                                         eap_mesh_cpp_cells_t const *cells,
                                                         eap_local_t new_cells);

typedef struct eap_mesh_cpp_levels_setup_all_top_fields {
    abi_ndarray_t all_top;
} eap_mesh_cpp_levels_setup_all_top_fields;

void eap_mesh_cpp_levels_setup_all_top(eap_mesh_cpp_levels_t *levels,
                                       eap_mesh_cpp_cells_t const *cells,
                                       eap_mesh_cpp_levels_setup_all_top_fields *fields);

typedef struct eap_mesh_cpp_levels_resize_local_fields {
    abi_ndarray_t cell_daughter;
    abi_ndarray_t cell_mother;
    abi_ndarray_t ltop;
    abi_ndarray_t cell_level;
    abi_ndarray_t flag;
    abi_ndarray_t flag_tag;
} eap_mesh_cpp_levels_resize_local_fields;

void eap_mesh_cpp_levels_resize_local(eap_mesh_cpp_levels_t *levels,
                                      eap_local_t num_cells,
                                      eap_local_t newsize,
                                      eap_mesh_cpp_levels_resize_local_fields *fields);

void eap_mesh_cpp_levels_recon_move_f(eap_mesh_cpp_levels_t *levels,
                                      eap_local_t data_length,
                                      size_t length,
                                      eap_local_t const *send_start,
                                      eap_local_t const *send_length,
                                      eap_local_t const *recv_start,
                                      eap_local_t const *recv_length);

void eap_mesh_cpp_levels_foreach_at_level(eap_mesh_cpp_levels_t const *levels,
                                          eap_local_t const level,
                                          void *context,
                                          void (*function)(void *context, eap_local_t cell));

void eap_mesh_cpp_levels_map_at_level_f(eap_mesh_cpp_levels_t const *levels,
                                        eap_local_t const level,
                                        abi_ndarray_t const out,
                                        void *context,
                                        int32_t (*function)(void *context, eap_local_t cell));

void eap_mesh_cpp_levels_count_filter_at_level(eap_mesh_cpp_levels_t const *levels,
                                               eap_mesh_cpp_cells_t const *cells,
                                               eap_local_t const level,
                                               void *context,
                                               bool (*function)(void *context, eap_local_t cell),
                                               eap_local_t *count);

void eap_mesh_cpp_levels_filter_at_level_f(eap_mesh_cpp_levels_t const *levels,
                                           eap_mesh_cpp_cells_t const *cells,
                                           eap_local_t const level,
                                           abi_ndarray_t const out,
                                           void *context,
                                           bool (*function)(void *context, eap_local_t cell));

void eap_mesh_cpp_levels_collect_at_level_f(eap_mesh_cpp_levels_t const *levels,
                                            eap_mesh_cpp_cells_t const *cells,
                                            eap_local_t const level,
                                            abi_ndarray_t const out);

void eap_mesh_cpp_levels_clear_at_level(eap_mesh_cpp_levels_t *levels,
                                        eap_local_t const level,
                                        abi_ndarray_t const flags,
                                        eap_local_t *num_cleared);

void eap_mesh_cpp_levels_num_at_level(eap_mesh_cpp_levels_t const *levels,
                                      eap_local_t const level,
                                      eap_local_t *num);

void eap_mesh_cpp_levels_recon_pack(eap_mesh_cpp_levels_t *levels,
                                    eap_local_t nlow,
                                    eap_local_t nhigh,
                                    eap_local_t nstep,
                                    eap_local_t move_num,
                                    eap_local_t const *move_from,
                                    eap_local_t const *move_to);

void eap_mesh_cpp_levels_set_flags(eap_mesh_cpp_levels_t *levels,
                                   eap_local_t low,
                                   eap_local_t high,
                                   void *context,
                                   int32_t (*function)(void *, eap_local_t));

void eap_mesh_cpp_levels_set_flags_scalar(eap_mesh_cpp_levels_t *levels,
                                          eap_local_t low,
                                          eap_local_t high,
                                          int32_t value);

void eap_mesh_cpp_levels_set_flags_all(eap_mesh_cpp_levels_t *levels, int32_t value);

void eap_mesh_cpp_levels_set_flags_scalar_when(eap_mesh_cpp_levels_t *levels,
                                               eap_local_t low,
                                               eap_local_t high,
                                               int32_t value,
                                               void *context,
                                               int32_t (*function)(void *, eap_local_t));

void eap_mesh_cpp_levels_set_flags_scalar_from_list(eap_mesh_cpp_levels_t *levels,
                                                    size_t list_size,
                                                    eap_local_t const *list,
                                                    int32_t value);

void eap_mesh_cpp_levels_set_flags_scalar_and_tags_from_list(eap_mesh_cpp_levels_t *levels,
                                                             size_t list_size,
                                                             eap_local_t const *list,
                                                             int32_t value,
                                                             size_t tags_size,
                                                             int32_t const *tags);

void eap_mesh_cpp_levels_set_flags_from_tuples(eap_mesh_cpp_levels_t *levels,
                                               size_t tups_size,
                                               eap_mesh_loc_val const *tups);

void eap_mesh_cpp_levels_set_flag_tags_from_tuples(eap_mesh_cpp_levels_t *levels,
                                                   size_t tups_size,
                                                   eap_mesh_loc_val const *tups);

void eap_mesh_cpp_levels_set_flags_from_tuples_key_and_scalar(eap_mesh_cpp_levels_t *levels,
                                                              size_t tups_size,
                                                              eap_mesh_loc_val const *tups,
                                                              int32_t value);

void eap_mesh_cpp_levels_set_flag_tags_from_tuples_key_and_scalar(eap_mesh_cpp_levels_t *levels,
                                                                  size_t tups_size,
                                                                  eap_mesh_loc_val const *tups,
                                                                  int32_t value);

void eap_mesh_cpp_levels_mom_kid_build(eap_mesh_cpp_levels_t *levels, eap_mesh_cpp_cells_t *cells);
void eap_mesh_cpp_levels_mom_kid_free(eap_mesh_cpp_levels_t *levels);

void eap_mesh_cpp_levels_kid_mom_build(eap_mesh_cpp_levels_t *levels, eap_mesh_cpp_cells_t *cells);
void eap_mesh_cpp_levels_mom_kids_build(eap_mesh_cpp_levels_t *levels, eap_mesh_cpp_cells_t *cells);

void eap_mesh_cpp_levels_check_lohipack_and_levels(eap_mesh_cpp_levels_t *levels,
                                                   eap_mesh_cpp_cells_t const *cells);

void eap_mesh_cpp_levels_recon_div_parent(eap_mesh_cpp_levels_t *levels,
                                          eap_mesh_cpp_cells_t const *cells,
                                          eap_local_t parentl,
                                          eap_local_t childl);

void eap_mesh_cpp_levels_recon_div_child(eap_mesh_cpp_levels_t *levels,
                                         eap_mesh_cpp_cells_t const *cells,
                                         eap_local_t level,
                                         eap_local_t parentl,
                                         eap_local_t l);

void eap_mesh_cpp_levels_clear_daughter(eap_mesh_cpp_levels_t *levels, eap_local_t l);

void eap_mesh_cpp_levels_kid_get(eap_mesh_cpp_levels_t *levels,
                                 eap_local_t level,
                                 abi_ndarray_t const *input,
                                 abi_ndarray_t const *output,
                                 eap_comm_datatype_t datatype);

void eap_mesh_cpp_levels_kid_put(eap_mesh_cpp_levels_t *levels,
                                 eap_local_t level,
                                 abi_ndarray_t const *input,
                                 abi_ndarray_t const *output,
                                 eap_comm_datatype_t datatype);

void eap_mesh_cpp_levels_kid_put_v(eap_mesh_cpp_levels_t *levels,
                                   eap_local_t level,
                                   abi_ndarray_t const *input,
                                   abi_ndarray_t const *output,
                                   eap_comm_datatype_t datatype);

void eap_mesh_cpp_levels_kid_put_inv(eap_mesh_cpp_levels_t *levels,
                                     int32_t level,
                                     abi_ndarray_t const *input,
                                     abi_ndarray_t const *output,
                                     eap_comm_datatype_t datatype);

void eap_mesh_cpp_levels_mom_get(eap_mesh_cpp_levels_t *levels,
                                 eap_local_t level,
                                 abi_ndarray_t const *input,
                                 abi_ndarray_t const *output,
                                 eap_comm_datatype_t datatype);

void eap_mesh_cpp_levels_mark_as_initialized(eap_mesh_cpp_levels_t *levels);

void eap_mesh_cpp_levels_apply_dzns(eap_mesh_cpp_levels_t *levels,
                                    eap_mesh_dzn_t const *dzn,
                                    int numdim,
                                    eap_mesh_cpp_cells_t const *cells,
                                    double time,
                                    int kode);

void eap_mesh_cpp_levels_set_num_pack(eap_mesh_cpp_levels_t *levels, eap_local_t num_pack);

void eap_mesh_cpp_levels_set_level_max_from_tuples(eap_mesh_cpp_levels_t *levels,
                                                   size_t tups_size,
                                                   eap_mesh_loc_val const *tups);

EXTERN_C_END

#endif // EAP_MESH_FFI_LEVELS_H_