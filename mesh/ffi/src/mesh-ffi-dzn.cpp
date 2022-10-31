/**
 * @file mesh-ffi-dzn.cpp
 *
 * @brief FFI for DZN
 * @date 2019-08-07
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <mesh-ffi-dzn.h>

// STL Includes
#include <memory>

// Internal Includes
#include <abi-fortran_interop_impl.hpp>
#include <mesh-dzn_compute.hpp>
#include <utility-kokkos.hpp>

// Local Includes
#include <mesh-ffi-interop.hpp>

using namespace eap;

using eap::mesh::DefinedZoneResolutionInfo;
using eap::mesh::NewDzn;
using eap::mesh::ZoneOptionsDigits;
using eap::mesh::ffi::CellsFromFFI;
using eap::mesh::ffi::DefinedZoneResolutionInfoFromFFI;
using eap::mesh::ffi::DefinedZoneResolutionInfoToFFI;
using eap::utility::kokkos::ViewFromNdarray;
using eap::utility::kokkos::ViewToNdarray;

FROM_HANDLE_INTERNAL_IMPL_NON_CONST_ONLY(mesh, dzn, DefinedZoneResolutionInfo)

namespace {
Kokkos::pair<double, double> ToPair(double const (&arr)[2]) {
    return Kokkos::make_pair(arr[0], arr[1]);
}

abi_ndarray_t PairViewToNdarray(Kokkos::View<Kokkos::pair<double, double> *> const &view) {
    abi_ndarray_t nd{};

    nd.rank = 2;
    nd.dims[0] = 2;
    nd.dims[1] = view.extent(0);
    nd.strides[0] = 1;
    nd.strides[1] = 2;
    nd.data = const_cast<void *>(static_cast<void const *>(view.data()));

    return nd;
}
} // namespace

FC_INTEROP_CREATE_C_IMPL(mesh, dzn, DefinedZoneResolutionInfo)
FC_INTEROP_DELETE_C_IMPL(mesh, dzn)

EXTERN_C void eap_mesh_dzn_get_fields(eap_mesh_dzn_t const *dznffi, eap_mesh_dzn_fields_t *fields) {
    EAP_EXTERN_PRE

    auto const &dzn = *DefinedZoneResolutionInfoFromFFI(dznffi);

    fields->numdzn = dzn.numdzn;
    fields->options = ViewToNdarray(dzn.options);
    fields->levels = ViewToNdarray(dzn.levels);
    fields->materials = ViewToNdarray(dzn.materials);
    fields->radiuses = PairViewToNdarray(dzn.radiuses);
    fields->radius_deltas = PairViewToNdarray(dzn.radius_deltas);
    fields->xs = PairViewToNdarray(dzn.xs);
    fields->x_deltas = PairViewToNdarray(dzn.x_deltas);
    fields->ys = PairViewToNdarray(dzn.ys);
    fields->y_deltas = PairViewToNdarray(dzn.y_deltas);
    fields->zs = PairViewToNdarray(dzn.zs);
    fields->z_deltas = PairViewToNdarray(dzn.z_deltas);
    fields->sizes = ViewToNdarray(dzn.sizes);

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_dzn_push(eap_mesh_dzn_t *dznffi, eap_mesh_new_dzn_t const *newdznffi) {
    EAP_EXTERN_PRE

    auto &dzn = *DefinedZoneResolutionInfoFromFFI(dznffi);

    NewDzn newdzn;
    newdzn.options = ZoneOptionsDigits(newdznffi->options);
    newdzn.level = newdznffi->level;
    newdzn.material = newdznffi->material;
    newdzn.r = ToPair(newdznffi->r);
    newdzn.rd = ToPair(newdznffi->rd);
    newdzn.x = ToPair(newdznffi->x);
    newdzn.xd = ToPair(newdznffi->xd);
    newdzn.y = ToPair(newdznffi->y);
    newdzn.yd = ToPair(newdznffi->yd);
    newdzn.z = ToPair(newdznffi->z);
    newdzn.zd = ToPair(newdznffi->zd);
    newdzn.size = newdznffi->size;

    dzn.Push(newdzn);

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_split_dzn_filter_kode(eap_mesh_dzn_t const *dznffi,
                                             int numdim,
                                             double time,
                                             int kode,
                                             int nlow,
                                             int nhigh,
                                             double testx,
                                             double testy,
                                             double testz,
                                             int level_base,
                                             int *level_set) {
    EAP_EXTERN_PRE

    auto &dzn = *DefinedZoneResolutionInfoFromFFI(dznffi);

    if (auto opt_level_set =
            eap::mesh::SplitDZNFilterKode(dzn.SubView(nlow, nhigh),
                                          static_cast<uint8_t>(numdim),
                                          time,
                                          static_cast<eap::mesh::Kode>(kode),
                                          eap::mesh::Coordinates<double>{testx, testy, testz},
                                          level_base)) {
        *level_set = *opt_level_set;
    }

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_process_cells_in_dzn_filter_kode(eap_mesh_dzn_t const *dznffi,
                                                        int numdim,
                                                        eap_mesh_cpp_cells_t const *cells,
                                                        double time,
                                                        int kode,
                                                        int llow,
                                                        int lhigh,
                                                        abi_ndarray_t const *level_base,
                                                        abi_ndarray_t const *level_set) {
    EAP_EXTERN_PRE

    eap::mesh::ProcessCellsInDZNFilterKode(*DefinedZoneResolutionInfoFromFFI(dznffi),
                                           static_cast<uint8_t>(numdim),
                                           *CellsFromFFI(cells),
                                           time,
                                           static_cast<eap::mesh::Kode>(kode),
                                           llow,
                                           lhigh,
                                           ViewFromNdarray<local_index_t const *>(*level_base),
                                           ViewFromNdarray<local_index_t *>(*level_set));

    EAP_EXTERN_POST
}

EXTERN_C void
eap_mesh_process_cells_in_dzn_filter_missing_material(eap_mesh_dzn_t const *dznffi,
                                                      int numdim,
                                                      eap_mesh_cpp_cells_t const *cells,
                                                      double time,
                                                      int llow,
                                                      int lhigh,
                                                      abi_ndarray_t const *level_base,
                                                      abi_ndarray_t const *level_set) {
    EAP_EXTERN_PRE

    eap::mesh::ProcessCellsInDZNFilterMissingMaterial(
        *DefinedZoneResolutionInfoFromFFI(dznffi),
        static_cast<uint8_t>(numdim),
        *CellsFromFFI(cells),
        time,
        llow,
        lhigh,
        ViewFromNdarray<local_index_t const *>(*level_base),
        ViewFromNdarray<local_index_t *>(*level_set));

    EAP_EXTERN_POST
}

EXTERN_C void eap_mesh_process_cells_in_dzn_filter_with_material(eap_mesh_dzn_t const *dznffi,
                                                                 int numdim,
                                                                 eap_mesh_cpp_cells_t const *cells,
                                                                 double time,
                                                                 int material,
                                                                 int llow,
                                                                 int lhigh,
                                                                 abi_ndarray_t const *level_base,
                                                                 abi_ndarray_t const *level_set) {
    EAP_EXTERN_PRE

    eap::mesh::ProcessCellsInDZNFilterWithMaterial(
        *DefinedZoneResolutionInfoFromFFI(dznffi),
        static_cast<uint8_t>(numdim),
        *CellsFromFFI(cells),
        time,
        material,
        llow,
        lhigh,
        ViewFromNdarray<local_index_t const *>(*level_base),
        ViewFromNdarray<local_index_t *>(*level_set));

    EAP_EXTERN_POST
}