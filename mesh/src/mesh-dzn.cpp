/**
 * @file mesh-dzn.cpp
 *
 * @brief Defined Zone Resolution
 * @date 2019-07-01
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Includes
#include <mesh-dzn.hpp>
#include <mesh-dzn_compute.hpp>

// Third Party Includes
#include <Kokkos_ScatterView.hpp>

// Internal Includes
#include <error-macros.hpp>

using eap::mesh::DefinedZoneResolutionInfo;

using namespace eap;

eap::mesh::ZoneOptionsDigits::ZoneOptionsDigits(int digits) : digits_(digits) {
    EE_DIAG_PRE

    auto const zone_type_int = digits_ % 10;
    EE_ASSERT(zone_type_int >= 0 && zone_type_int <= 3,
              "1st digit of typdzn must be an integer in the range (0,3)");

    auto const add_int = (digits_ / 10) % 10;
    EE_ASSERT(add_int >= 0 && add_int <= 1,
              "2nd digit of typdzn must be an integer in the range (0,1)");

    auto const kode_int = (digits_ / 100) % 10;
    EE_ASSERT(kode_int >= 0 && kode_int <= 3,
              "3rd digit of typdzn must be an integer in the range (0,3)");

    auto const outer_int = (digits_ / 1000) % 10;
    EE_ASSERT(outer_int >= 0 && outer_int <= 1,
              "4th digit of typdzn must be an integer in the range (0,1)");

    EE_DIAG_POST_MSG("digits = " << digits)
}

void eap::mesh::ZoneOptionsDigits::SetOptions(ZoneOptions const &opts) {
    digits_ = static_cast<int>(opts.outer) * 1000 + static_cast<int>(opts.kode) * 100 +
              static_cast<int>(opts.add) * 10 + static_cast<int>(opts.use);
}

DefinedZoneResolutionInfo::DefinedZoneResolutionInfo()
    : numdzn(0),
      options("DefinedZoneResolutionInfo::DefinedZoneResolutionInfo::options", 0),
      levels("DefinedZoneResolutionInfo::DefinedZoneResolutionInfo::levels", 0),
      materials("DefinedZoneResolutionInfo::DefinedZoneResolutionInfo::materials", 0),
      radiuses("DefinedZoneResolutionInfo::DefinedZoneResolutionInfo::radiuses", 0),
      radius_deltas("DefinedZoneResolutionInfo::DefinedZoneResolutionInfo::radius_deltas", 0),
      xs("DefinedZoneResolutionInfo::DefinedZoneResolutionInfo::xs", 0),
      x_deltas("DefinedZoneResolutionInfo::DefinedZoneResolutionInfo::x_deltas", 0),
      ys("DefinedZoneResolutionInfo::DefinedZoneResolutionInfo::ys", 0),
      y_deltas("DefinedZoneResolutionInfo::DefinedZoneResolutionInfo::y_deltas", 0),
      zs("DefinedZoneResolutionInfo::DefinedZoneResolutionInfo::zs", 0),
      z_deltas("DefinedZoneResolutionInfo::DefinedZoneResolutionInfo::z_deltas", 0),
      sizes("DefinedZoneResolutionInfo::DefinedZoneResolutionInfo::sizes", 0) {}

DefinedZoneResolutionInfo DefinedZoneResolutionInfo::SubView(local_index_t nlow,
                                                             local_index_t nhigh) const {
    using Kokkos::subview;
    EE_DIAG_PRE

    EE_ASSERT(nhigh >= nlow);

    EE_ASSERT(nlow <= numdzn);
    EE_ASSERT(nhigh <= numdzn);

    DefinedZoneResolutionInfo target;

    target.numdzn = nhigh - nlow;
    target.options = subview(this->options, Kokkos::make_pair(nlow, nhigh));
    target.levels = subview(this->levels, Kokkos::make_pair(nlow, nhigh));
    target.materials = subview(this->materials, Kokkos::make_pair(nlow, nhigh));
    target.radiuses = subview(this->radiuses, Kokkos::make_pair(nlow, nhigh));
    target.radius_deltas = subview(this->radius_deltas, Kokkos::make_pair(nlow, nhigh));
    target.xs = subview(this->xs, Kokkos::make_pair(nlow, nhigh));
    target.x_deltas = subview(this->x_deltas, Kokkos::make_pair(nlow, nhigh));
    target.ys = subview(this->ys, Kokkos::make_pair(nlow, nhigh));
    target.y_deltas = subview(this->y_deltas, Kokkos::make_pair(nlow, nhigh));
    target.zs = subview(this->zs, Kokkos::make_pair(nlow, nhigh));
    target.z_deltas = subview(this->z_deltas, Kokkos::make_pair(nlow, nhigh));
    target.sizes = subview(this->sizes, Kokkos::make_pair(nlow, nhigh));

    return target;

    EE_DIAG_POST_MSG("[nlow, nhigh) = [" << nlow << ","
                                         << ")");
}

void DefinedZoneResolutionInfo::Reserve(local_index_t size) {
    Kokkos::resize(options, size);
    Kokkos::resize(levels, size);
    Kokkos::resize(materials, size);
    Kokkos::resize(radiuses, size);
    Kokkos::resize(radius_deltas, size);
    Kokkos::resize(xs, size);
    Kokkos::resize(x_deltas, size);
    Kokkos::resize(ys, size);
    Kokkos::resize(y_deltas, size);
    Kokkos::resize(zs, size);
    Kokkos::resize(z_deltas, size);
    Kokkos::resize(sizes, size);
}

void DefinedZoneResolutionInfo::Resize(local_index_t size) {
    numdzn = size;
    Reserve(numdzn);
}

void DefinedZoneResolutionInfo::Grow(local_index_t size) {
    if ((levels.extent(0) - numdzn) < size) {
        if (levels.extent(0) == 0) {
            // Reserve at least 4 elements
            Reserve(std::max<local_index_t>(size, 4));
        } else {
            // Else grow by a factor of 1.5
            Reserve(levels.extent(0) * 3 / 2);
        }
    }
}

void DefinedZoneResolutionInfo::Push(NewDzn const &dzn_info) {
    // Add an element
    Grow(1);

    auto const new_dzn = numdzn;
    numdzn += 1;

    options(new_dzn) = dzn_info.options;
    if (dzn_info.level) levels(new_dzn) = *dzn_info.level;
    if (dzn_info.material) materials(new_dzn) = *dzn_info.material;
    if (dzn_info.r) radiuses(new_dzn) = *dzn_info.r;
    if (dzn_info.rd) radius_deltas(new_dzn) = *dzn_info.rd;
    if (dzn_info.x) xs(new_dzn) = *dzn_info.x;
    if (dzn_info.xd) x_deltas(new_dzn) = *dzn_info.xd;
    if (dzn_info.y) ys(new_dzn) = *dzn_info.y;
    if (dzn_info.yd) y_deltas(new_dzn) = *dzn_info.yd;
    if (dzn_info.z) zs(new_dzn) = *dzn_info.z;
    if (dzn_info.zd) z_deltas(new_dzn) = *dzn_info.zd;
    if (dzn_info.size) sizes(new_dzn) = *dzn_info.size;
}

std::optional<local_index_t> eap::mesh::FindDZN(DefinedZoneResolutionInfo const &dzn,
                                                   std::uint8_t numdim,
                                                   double time,
                                                   Coordinates<double> test) {
    return FindDZNWithFilter(
        dzn, numdim, time, test, [](local_index_t, ZoneOptions const &) { return true; });
}

std::optional<local_index_t> eap::mesh::SplitDZN(DefinedZoneResolutionInfo const &dzn,
                                                    std::uint8_t numdim,
                                                    double time,
                                                    Coordinates<double> test,
                                                    local_index_t level_base) {
    return SplitDZNWithFilter(
        dzn, numdim, time, test, level_base, [](local_index_t, ZoneOptions const &) {
            return true;
        });
}

std::optional<local_index_t> eap::mesh::SplitDZNFilterKode(DefinedZoneResolutionInfo const &dzn,
                                                              std::uint8_t numdim,
                                                              double time,
                                                              Kode kode,
                                                              Coordinates<double> test,
                                                              local_index_t level_base) {
    return SplitDZNWithFilter(
        dzn, numdim, time, test, level_base, [kode](local_index_t, ZoneOptions options) {
            return options.kode == kode || options.kode == Kode::Unconditional;
        });
}

void eap::mesh::ProcessCellsInDZNFilterKode(
    DefinedZoneResolutionInfo const &dzn,
    std::uint8_t numdim,
    Cells const &cells,
    double time,
    Kode kode,
    local_index_t llow,
    local_index_t lhigh,
    Kokkos::View<local_index_t const *, eap::HostMemorySpace> level_base,
    Kokkos::View<local_index_t *, eap::HostMemorySpace> level_set) {
    ProcessCellsInDZNWithFilter(dzn,
                                numdim,
                                cells,
                                time,
                                llow,
                                lhigh,
                                level_base,
                                level_set,
                                [kode](local_index_t, ZoneOptions options) {
                                    return options.kode == kode ||
                                           options.kode == Kode::Unconditional;
                                });
}

void eap::mesh::ProcessCellsInDZNFilterMissingMaterial(
    DefinedZoneResolutionInfo const &dzn,
    std::uint8_t numdim,
    Cells const &cells,
    double time,
    local_index_t llow,
    local_index_t lhigh,
    Kokkos::View<local_index_t const *, eap::HostMemorySpace> level_base,
    Kokkos::View<local_index_t *, eap::HostMemorySpace> level_set) {

    ProcessCellsInDZNWithFilter(dzn,
                                numdim,
                                cells,
                                time,
                                llow,
                                lhigh,
                                level_base,
                                level_set,
                                [&dzn](local_index_t i, ZoneOptions options) {
                                    return options.kode == Kode::MissingMaterial &&
                                           !dzn.materials(i).has_value();
                                });
}

void eap::mesh::ProcessCellsInDZNFilterWithMaterial(
    DefinedZoneResolutionInfo const &dzn,
    std::uint8_t numdim,
    Cells const &cells,
    double time,
    local_index_t material,
    local_index_t llow,
    local_index_t lhigh,
    Kokkos::View<local_index_t const *, eap::HostMemorySpace> level_base,
    Kokkos::View<local_index_t *, eap::HostMemorySpace> level_set) {

    ProcessCellsInDZNWithFilter(dzn,
                                numdim,
                                cells,
                                time,
                                llow,
                                lhigh,
                                level_base,
                                level_set,
                                [&dzn, material](local_index_t i, ZoneOptions options) {
                                    return options.kode == Kode::Material &&
                                           (!dzn.materials(i).has_value() ||
                                            *dzn.materials(i) == material);
                                });
}

void eap::mesh::internal::ProcessCellsInDZN(
    DefinedZoneResolutionInfo const &dzn,
    size_t n,
    std::uint8_t numdim,
    Cells const &cells,
    double time,
    local_index_t llow,
    local_index_t lhigh,
    Kokkos::View<local_index_t const *, eap::HostMemorySpace> level_base,
    Kokkos::View<local_index_t *> lbuf,
    Kokkos::View<local_index_t *, eap::HostMemorySpace> level_set) {
    using namespace internal;

    using eap::utility::Square;
    using eap::utility::SquarePair;

    EE_DIAG_PRE

    auto const options = dzn.options(n).GetOptions();

    auto const levels = dzn.levels;
    auto const use_inside = !options.outer;

    auto const cell_center = cells.cell_center();

    auto const for_cells = [llow, lhigh](auto fn) {
        Kokkos::parallel_for(Kokkos::RangePolicy<>(llow, lhigh), fn);
    };

    auto const set_buf = KOKKOS_LAMBDA(local_index_t l, bool inside) {
        if (inside == use_inside) {
            lbuf(l - llow) = levels(n);
        }
    };

    auto const rrange = ApplyDeltas(dzn.radiuses(n), dzn.radius_deltas(n), time);
    auto const xrange = ApplyDeltas(dzn.xs(n), dzn.x_deltas(n), time);
    auto const yrange = ApplyDeltas(dzn.ys(n), dzn.y_deltas(n), time);
    auto const zrange = ApplyDeltas(dzn.ys(n), dzn.y_deltas(n), time);

    auto const drtest = rrange.second - rrange.first;
    auto const dxtest = xrange.second - xrange.first;
    auto const dytest = yrange.second - yrange.first;
    auto const dztest = zrange.second - zrange.first;

    if (options.use == ZoneShape::BricksAndRectangles) {
        if (numdim == 3) {
            for_cells(KOKKOS_LAMBDA(local_index_t l) {
                set_buf(l,
                        InRange(xrange, cell_center(l, X_DIR)) &&
                            InRange(yrange, cell_center(l, Y_DIR)) &&
                            InRange(zrange, cell_center(l, Z_DIR)));
            });
        } else if (numdim == 2) {
            for_cells(KOKKOS_LAMBDA(local_index_t l) {
                set_buf(l,
                        InRange(xrange, cell_center(l, X_DIR)) &&
                            InRange(yrange, cell_center(l, Y_DIR)));
            });
        } else {
            for_cells(KOKKOS_LAMBDA(local_index_t l) {
                set_buf(l, InRange(xrange, cell_center(l, X_DIR)));
            });
        }
    } else if (options.use == ZoneShape::SpheresAndCircles) {
        if (rrange.second > rrange.first) {
            if (numdim == 3) {
                for_cells(KOKKOS_LAMBDA(local_index_t l) {
                    set_buf(l,
                            InRange(SquarePair(rrange),
                                    Square(cell_center(l, X_DIR) - xrange.first) +
                                        Square(cell_center(l, Y_DIR) - yrange.first) +
                                        Square(cell_center(l, Z_DIR) - zrange.first)));
                });
            } else if (numdim == 2) {
                for_cells(KOKKOS_LAMBDA(local_index_t l) {
                    set_buf(l,
                            InRange(SquarePair(rrange),
                                    Square(cell_center(l, X_DIR) - xrange.first) +
                                        Square(cell_center(l, Y_DIR) - yrange.first)));
                });
            } else {
                for_cells(KOKKOS_LAMBDA(local_index_t l) {
                    set_buf(
                        l,
                        InRange(SquarePair(rrange), Square(cell_center(l, X_DIR) - xrange.first)));
                });
            }
        } else if (rrange.first > 0.0) {
            if (numdim == 3) {
                for_cells(KOKKOS_LAMBDA(local_index_t l) {
                    set_buf(l,
                            Square(cell_center(l, X_DIR) - xrange.first) +
                                    Square(cell_center(l, Y_DIR) - yrange.first) +
                                    Square(cell_center(l, Z_DIR) - zrange.first) <=
                                Square(rrange.first));
                });
            } else if (numdim == 2) {
                for_cells(KOKKOS_LAMBDA(local_index_t l) {
                    set_buf(l,
                            Square(cell_center(l, X_DIR) - xrange.first) +
                                    Square(cell_center(l, Y_DIR) - yrange.first) <=
                                Square(rrange.first));
                });
            } else {
                for_cells(KOKKOS_LAMBDA(local_index_t l) {
                    set_buf(l,
                            Square(cell_center(l, X_DIR) - xrange.first) <= Square(rrange.first));
                });
            }
        }

    } else if (options.use == ZoneShape::ConesAndWedges) {
        if (numdim == 3) {
            auto const denom = Square(dxtest) + Square(dytest) + Square(dztest);

            if (rrange.first >= 0.0 && rrange.second >= 0.0 && denom > 0.0) {
                auto const factor = 1.0 / denom;
                for_cells(KOKKOS_LAMBDA(local_index_t l) {
                    auto const s = (dxtest * (cell_center(l, X_DIR) - xrange.first) +
                                    dytest * (cell_center(l, Y_DIR) - yrange.first) +
                                    dztest * (cell_center(l, Z_DIR) - zrange.first)) *
                                   factor;
                    auto const rctest = rrange.first + s * drtest;
                    auto const xctest = xrange.first + s * dxtest;
                    auto const yctest = yrange.first + s * dytest;
                    auto const zctest = zrange.first + s * dztest;

                    auto const rtest = Square(rctest);

                    auto const rsqr = Square(xctest - cell_center(l, X_DIR)) +
                                      Square(yctest - cell_center(l, Y_DIR)) +
                                      Square(zctest - cell_center(l, Z_DIR));
                    set_buf(l, s >= 0.0 && s <= 1.0 && rsqr <= rtest);
                });
            }
        } else if (numdim == 2) {
            auto const denom = Square(dxtest) + Square(dytest);

            if (rrange.first >= 0.0 && rrange.second >= 0.0 && denom > 0.0) {
                auto const factor = 1.0 / denom;
                for_cells(KOKKOS_LAMBDA(local_index_t l) {
                    auto const s = (dxtest * (cell_center(l, X_DIR) - xrange.first) +
                                    dytest * (cell_center(l, Y_DIR) - yrange.first)) *
                                   factor;
                    auto const rctest = rrange.first + s * drtest;
                    auto const xctest = xrange.first + s * dxtest;
                    auto const yctest = yrange.first + s * dytest;

                    auto const rtest = Square(rctest);

                    auto const rsqr = Square(xctest - cell_center(l, X_DIR)) +
                                      Square(yctest - cell_center(l, Y_DIR));
                    set_buf(l, s >= 0.0 && s <= 1.0 && rsqr <= rtest);
                });
            }
        } else {
            EE_RAISE("typdzn 1st digit of 2 selected a ZoneShape::ConesAndWedges DZN for a "
                     << numdim
                     << "dimension problem. ConesAndWedges DZNs are only supported for 2D and "
                        "3D problems.");
        }
    } else if (options.use == ZoneShape::Cylinder) {
        if (numdim == 3) {
            auto const denom = Square(dxtest) + Square(dytest) + Square(dztest);

            if (rrange.first >= 0.0 && rrange.second > rrange.first && denom > 0.0) {
                auto const factor = 1.0 / denom;
                for_cells(KOKKOS_LAMBDA(local_index_t l) {
                    auto const s = (dxtest * (cell_center(l, X_DIR) - xrange.first) +
                                    dytest * (cell_center(l, Y_DIR) - yrange.first) +
                                    dztest * (cell_center(l, Z_DIR) - zrange.first)) *
                                   factor;
                    auto const xctest = xrange.first + s * dxtest;
                    auto const yctest = yrange.first + s * dytest;
                    auto const zctest = zrange.first + s * dztest;

                    auto const rsqr = Square(xctest - cell_center(l, X_DIR)) +
                                      Square(yctest - cell_center(l, Y_DIR)) +
                                      Square(zctest - cell_center(l, Z_DIR));

                    set_buf(l,
                            InRange(Kokkos::make_pair(0.0, 1.0), s) &&
                                InRange(SquarePair(rrange), rsqr));
                });
            }
        } else if (numdim == 2) {
            auto const denom = Square(dxtest) + Square(dytest);

            if (rrange.first >= 0.0 && rrange.second > rrange.first && denom > 0.0) {
                auto const factor = 1.0 / denom;
                for_cells(KOKKOS_LAMBDA(local_index_t l) {
                    auto const s = (dxtest * (cell_center(l, X_DIR) - xrange.first) +
                                    dytest * (cell_center(l, Y_DIR) - yrange.first)) *
                                   factor;
                    auto const xctest = xrange.first + s * dxtest;
                    auto const yctest = yrange.first + s * dytest;

                    auto const rsqr = Square(xctest - cell_center(l, X_DIR)) +
                                      Square(yctest - cell_center(l, Y_DIR));

                    set_buf(l,
                            InRange(Kokkos::make_pair(0.0, 1.0), s) &&
                                InRange(SquarePair(rrange), rsqr));
                });
            }
        } else {
            EE_RAISE("typdzn 1st digit of 3 selected a ZoneShape::Cylinder DZN for a "
                     << numdim
                     << "dimension problem. Cylinder DZNs are only supported for 2D and "
                        "3D problems.");
        }
    }

    if (options.add) {
        for_cells(KOKKOS_LAMBDA(local_index_t l) {
            if (lbuf(l - llow) != 0)
                level_set(l) = std::max<local_index_t>(1, level_base(l) + lbuf(l - llow));
        });
    } else {
        for_cells(KOKKOS_LAMBDA(local_index_t l) {
            if (lbuf(l - llow) > 0) level_set(l) = lbuf(l - llow);
        });
    }

    EE_DIAG_POST
}