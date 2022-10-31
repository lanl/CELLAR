/**
 * @file mesh-ffi-dzn.h
 *
 * @brief FFI for DZN library
 * @date 2019-08-07
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_FFI_DZN_H_
#define EAP_MESH_FFI_DZN_H_

// Internal includes
#include <abi-extern.h>
#include <abi-fortran_interop.h>
#include <abi-ndarray.h>

// Local Includes
#include <mesh-ffi-handles.h>

typedef struct eap_mesh_dzn_fields_t {
    int numdzn;
    abi_ndarray_t options;
    abi_ndarray_t levels;
    abi_ndarray_t materials;
    abi_ndarray_t radiuses;
    abi_ndarray_t radius_deltas;
    abi_ndarray_t xs;
    abi_ndarray_t x_deltas;
    abi_ndarray_t ys;
    abi_ndarray_t y_deltas;
    abi_ndarray_t zs;
    abi_ndarray_t z_deltas;
    abi_ndarray_t sizes;
} eap_mesh_dzn_fields_t;

typedef struct eap_mesh_new_dzn_t {
    int options;
    int level;
    int material;
    double r[2];
    double rd[2];
    double x[2];
    double xd[2];
    double y[2];
    double yd[2];
    double z[2];
    double zd[2];
    double size;
} eap_mesh_new_dzn_t;

EXTERN_C_BEGIN

FC_INTEROP_CREATE_C_DECL(mesh, dzn);
FC_INTEROP_DELETE_C_DECL(mesh, dzn);

void eap_mesh_dzn_get_fields(eap_mesh_dzn_t const *dznffi, eap_mesh_dzn_fields_t *fields);

void eap_mesh_dzn_push(eap_mesh_dzn_t *dznffi, eap_mesh_new_dzn_t const *newdznffi);

void eap_mesh_split_dzn_filter_kode(eap_mesh_dzn_t const *dznffi,
                                    int numdim,
                                    double time,
                                    int kode,
                                    int nlow,
                                    int nhigh,
                                    double testx,
                                    double testy,
                                    double testz,
                                    int level_base,
                                    int *level_set);

void eap_mesh_process_cells_in_dzn_filter_kode(eap_mesh_dzn_t const *dznffi,
                                               int numdim,
                                               eap_mesh_cpp_cells_t const *cells,
                                               double time,
                                               int kode,
                                               int llow,
                                               int lhigh,
                                               abi_ndarray_t const *level_base,
                                               abi_ndarray_t const *level_set);

void eap_mesh_process_cells_in_dzn_filter_missing_material(eap_mesh_dzn_t const *dznffi,
                                                           int numdim,
                                                           eap_mesh_cpp_cells_t const *cells,
                                                           double time,
                                                           int llow,
                                                           int lhigh,
                                                           abi_ndarray_t const *level_base,
                                                           abi_ndarray_t const *level_set);

void eap_mesh_process_cells_in_dzn_filter_with_material(eap_mesh_dzn_t const *dznffi,
                                                        int numdim,
                                                        eap_mesh_cpp_cells_t const *cells,
                                                        double time,
                                                        int material,
                                                        int llow,
                                                        int lhigh,
                                                        abi_ndarray_t const *level_base,
                                                        abi_ndarray_t const *level_set);

EXTERN_C_END

#endif // EAP_MESH_FFI_DZN_H_