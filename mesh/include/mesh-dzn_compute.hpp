/**
 * @file mesh-dzn_compute.hpp
 *
 * @brief Algorithms for checking whether DZNs apply
 * @date 2019-08-08
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_DZN_COMPUTE_HPP_
#define EAP_MESH_DZN_COMPUTE_HPP_

// Internal Includes
#include <utility-memory.hpp>

// Local Includes
#include <mesh-cells.hpp>
#include <mesh-dzn.hpp>

namespace eap {
namespace mesh {

/**
 * @brief
 *  Compute the level for each cell in the range [llow,lhigh) by applying the DZN regions in `dzn`.
 *
 * @tparam FilterFn
 *  Type of functor for filtering out DZNs. Runs on host
 * @param dzn
 *  DZN Properties
 * @param numdim
 *  Number of dimensions of current run
 * @param cell_center
 * @param time
 *  Elapsed time
 * @param llow
 *  Lowest cell index
 * @param lhigh
 *  Highest cell index
 * @param lev_base
 *  Base level for relative levels
 * @param level_set
 *  Output array for levels. Should be over all cells
 * @param filter
 *  Filtering function for DZNs
 */
template <typename FilterFn>
void ProcessCellsInDZNWithFilter(
    DefinedZoneResolutionInfo const &dzn,
    std::uint8_t numdim,
    Cells const &cells,
    double time,
    local_index_t llow,
    local_index_t lhigh,
    Kokkos::View<local_index_t const *, eap::HostMemorySpace> level_base,
    Kokkos::View<local_index_t *, eap::HostMemorySpace> level_set,
    FilterFn filter);

/**
 * @brief Determine which DZN applies at the position (test.x, test.y, test.z).
 *
 * @details
 *  The selected DZN is always the final DZN in the list that "matches" the position.
 *
 * @tparam FilterFn
 *  Type of the Filter function (typically a lambda)
 * @param dzn
 *  DZN Properties
 * @param numdim
 *  Number of dimensions of current run
 * @param time
 *  Elapsed time
 * @param test
 *  Point to check
 * @param filter
 *  A check on DZNs to decide whether it should be applied.
 * @return
 *  If an DZNs match, the index of the matching DZN. Otherwise nonstd::nullopt.
 */
template <typename FilterFn>
nonstd::optional<eap::local_index_t> FindDZNWithFilter(DefinedZoneResolutionInfo const &dzn,
                                                       std::uint8_t numdim,
                                                       double time,
                                                       Coordinates<double> test,
                                                       FilterFn filter);

/**
 * @brief Determine which DZN applies at the position (test.x, test.y, test.z).
 *
 * @details
 *  The selected DZN is always the final DZN in the list that "matches" the position.
 *
 * @param dzn
 *  DZN Properties
 * @param numdim
 *  Number of dimensions of current run
 * @param time
 *  Elapsed time
 * @param test
 *  Point to check
 * @return
 *  If an DZNs match, the index of the matching DZN. Otherwise nonstd::nullopt.
 */
nonstd::optional<eap::local_index_t> FindDZN(DefinedZoneResolutionInfo const &dzn,
                                             std::uint8_t numdim,
                                             double time,
                                             Coordinates<double> test);

/**
 * @brief Determine what level we need to split cells at position (text.x, test.y, test.z).
 *
 * @details
 *  The selected DZN is always the final DZN in the list that "matches" the position.
 *
 * @tparam FilterFn
 *  Type of the Filter function (typically a lambda)
 * @param dzn
 *  DZN Properties
 * @param numdim
 *  Number of dimensions of current run
 * @param time
 *  Elapsed time
 * @param test
 *  Point to check
 * @param level_base
 *  The base level to add relative levels to
 * @param filter
 *  A check on DZNs to decide whether it should be applied.
 * @return
 *  If an DZNs match, a level. Otherwise nonstd::nullopt
 */
template <typename FilterFn>
nonstd::optional<eap::local_index_t> SplitDZNWithFilter(DefinedZoneResolutionInfo const &dzn,
                                                        std::uint8_t numdim,
                                                        double time,
                                                        Coordinates<double> test,
                                                        local_index_t level_base,
                                                        FilterFn filter);

/**
 * @brief Determine what level we need to split cells at position (xtest, ytest, ztest).
 *
 * @details
 *  SplitDZN only considers nlow to nhigh for the DZNs passed to it in `dzn`. The "winning" DZN is
 *  always the final DZN in the list that "matches" the position.
 *
 * @param dzn
 *  DZN Properties
 * @param numdim
 *  Number of dimensions of current run
 * @param time
 *  Elapsed time
 * @param test
 *  Point to check
 * @param level_base
 *  The base level to add relative levels to
 * @return
 *  If an DZNs match, a level. Otherwise nonstd::nullopt
 */
nonstd::optional<eap::local_index_t> SplitDZN(DefinedZoneResolutionInfo const &dzn,
                                              std::uint8_t numdim,
                                              double time,
                                              Coordinates<double> test,
                                              local_index_t level_base);

/**
 * @brief Determine what level we need to split cells at position (xtest, ytest, ztest).
 *
 * @details
 *  SplitDZN only considers nlow to nhigh for the DZNs passed to it in `dzn`. The "winning" DZN is
 *  always the final DZN in the list that "matches" the position.
 *
 * @param dzn
 *  DZN Properties
 * @param numdim
 *  Number of dimensions of current run
 * @param time
 *  Elapsed time
 * @param kode
 *  What kode the DZN must have to match.
 * @param test
 *  Point to check
 * @param level_base
 *  The base level to add relative levels to
 * @return
 *  If an DZNs match, a level. Otherwise nonstd::nullopt
 */
nonstd::optional<eap::local_index_t> SplitDZNFilterKode(DefinedZoneResolutionInfo const &dzn,
                                                        std::uint8_t numdim,
                                                        double time,
                                                        Kode kode,
                                                        Coordinates<double> test,
                                                        local_index_t level_base);

/**
 * @brief
 *  Compute the level for each cell in the range [llow,lhigh) by applying the DZN regions in `dzn`.
 *
 * @param dzn
 *  DZN Properties
 * @param numdim
 *  Number of dimensions of current run
 * @param cells
 *  The cells object
 * @param time
 *  Elapsed time
 * @param kode
 *  What kode the DZN must have to match.
 * @param llow
 *  Lowest cell index
 * @param lhigh
 *  Highest cell index
 * @param lev_base
 *  Base level for relative levels
 * @param level_set
 *  Output array for levels. Should be over all cells
 */
void ProcessCellsInDZNFilterKode(
    DefinedZoneResolutionInfo const &dzn,
    std::uint8_t numdim,
    Cells const &cells,
    double time,
    Kode kode,
    local_index_t llow,
    local_index_t lhigh,
    Kokkos::View<local_index_t const *, eap::HostMemorySpace> level_base,
    Kokkos::View<local_index_t *, eap::HostMemorySpace> level_set);

/**
 * @brief
 *  Compute the level for each cell in the range [llow,lhigh) without a material by applying the DZN
 *  regions in `dzn` that apply onto cells with no material.
 *
 * @param dzn
 *  DZN Properties
 * @param numdim
 *  Number of dimensions of current run
 * @param cells
 *  The cells object
 * @param time
 *  Elapsed time
 * @param llow
 *  Lowest cell index
 * @param lhigh
 *  Highest cell index
 * @param lev_base
 *  Base level for relative levels
 * @param level_set
 *  Output array for levels. Should be over all cells
 */
void ProcessCellsInDZNFilterMissingMaterial(
    DefinedZoneResolutionInfo const &dzn,
    std::uint8_t numdim,
    Cells const &cells,
    double time,
    local_index_t llow,
    local_index_t lhigh,
    Kokkos::View<local_index_t const *, eap::HostMemorySpace> level_base,
    Kokkos::View<local_index_t *, eap::HostMemorySpace> level_set);

/**
 * @brief
 *  Compute the level for each cell in the range [llow,lhigh) by applying the DZN regions in `dzn`.
 *
 * @param dzn
 *  DZN Properties
 * @param numdim
 *  Number of dimensions of current run
 * @param cells
 *  The cells object
 * @param time
 *  Elapsed time
 * @param material
 *  What material the DZN must have to match.
 * @param llow
 *  Lowest cell index
 * @param lhigh
 *  Highest cell index
 * @param lev_base
 *  Base level for relative levels
 * @param level_set
 *  Output array for levels. Should be over all cells
 */
void ProcessCellsInDZNFilterWithMaterial(
    DefinedZoneResolutionInfo const &dzn,
    std::uint8_t numdim,
    Cells const &cells,
    double time,
    local_index_t material,
    local_index_t llow,
    local_index_t lhigh,
    Kokkos::View<local_index_t const *, eap::HostMemorySpace> level_base,
    Kokkos::View<local_index_t *, eap::HostMemorySpace> level_set);

namespace internal {

KOKKOS_INLINE_FUNCTION Kokkos::pair<double, double>
ApplyDeltas(Kokkos::pair<double, double> const &vals,
            Kokkos::pair<double, double> const &deltas,
            double time) {
    Kokkos::pair<double, double> results;
    results.first = vals.first + deltas.first * time;
    results.second = vals.second + deltas.second * time;
    return results;
}

KOKKOS_INLINE_FUNCTION bool InRange(Kokkos::pair<double, double> const &range, double test) {
    return test >= range.first && test <= range.second;
}

void ProcessCellsInDZN(DefinedZoneResolutionInfo const &dzn,
                       size_t n,
                       std::uint8_t numdim,
                       Cells const &cells,
                       double time,
                       local_index_t llow,
                       local_index_t lhigh,
                       Kokkos::View<local_index_t const *, eap::HostMemorySpace> level_base,
                       Kokkos::View<local_index_t *> lbuf,
                       Kokkos::View<local_index_t *, eap::HostMemorySpace> level_set);
} // namespace internal
} // namespace mesh
} // namespace eap

template <typename FilterFn>
void eap::mesh::ProcessCellsInDZNWithFilter(
    DefinedZoneResolutionInfo const &dzn,
    std::uint8_t numdim,
    Cells const &cells,
    double time,
    local_index_t llow,
    local_index_t lhigh,
    Kokkos::View<local_index_t const *, eap::HostMemorySpace> level_base,
    Kokkos::View<local_index_t *, eap::HostMemorySpace> level_set,
    FilterFn filter) {
    using namespace internal;

    using eap::utility::Square;
    using eap::utility::SquarePair;

    EE_DIAG_PRE

    Kokkos::View<local_index_t *> lbuf("eap::mesh::ProcessCellsInDZNWithFilter::lbuf",
                                       lhigh - llow);

    for (local_index_t n = 0; n < dzn.numdzn; n++) {
        auto options = dzn.options(n).GetOptions();
        if (!filter(n, options)) continue;

        internal::ProcessCellsInDZN(
            dzn, n, numdim, cells, time, llow, lhigh, level_base, lbuf, level_set);
    }

    Kokkos::fence();

    EE_DIAG_POST
}

template <typename FilterFn>
nonstd::optional<eap::local_index_t>
eap::mesh::FindDZNWithFilter(DefinedZoneResolutionInfo const &dzn,
                             std::uint8_t numdim,
                             double time,
                             Coordinates<double> test,
                             FilterFn filter) {
    using namespace internal;

    using eap::utility::Square;

    EE_DIAG_PRE

    // The original Fortran code counted up through the DZNs, ultimately the final applicable
    // DZN was applied. This code instead iterates backwards through the DZNs, breaking once a
    // match is found. The effect of the code should be the same, but this code will be slightly
    // faster.
    auto n = dzn.numdzn;
    while (n-- > 0) {
        auto options = dzn.options(n).GetOptions();

        auto const use_inside = !options.outer;
        if (!filter(n, options)) continue;

        auto const rrange = ApplyDeltas(dzn.radiuses(n), dzn.radius_deltas(n), time);
        auto const xrange = ApplyDeltas(dzn.xs(n), dzn.x_deltas(n), time);
        auto const yrange = ApplyDeltas(dzn.ys(n), dzn.y_deltas(n), time);
        auto const zrange = ApplyDeltas(dzn.ys(n), dzn.y_deltas(n), time);

        auto const drtest = rrange.second - rrange.first;
        auto const dxtest = xrange.second - xrange.first;
        auto const dytest = yrange.second - yrange.first;
        auto const dztest = zrange.second - zrange.first;

        if (options.use == ZoneShape::BricksAndRectangles) {
            bool inside;
            if (numdim == 3) {
                inside =
                    InRange(xrange, test.x) && InRange(yrange, test.y) && InRange(zrange, test.z);
            } else if (numdim == 2) {
                inside = InRange(xrange, test.x) && InRange(yrange, test.y);
            } else {
                // NOTE: The original Fortran code also checked the yrange here, which seemed pretty
                // nonsensical to me for a 1D test.
                inside = InRange(xrange, test.x);
            }

            if (inside == use_inside) {
                return n;
            }
        } else if (options.use == ZoneShape::SpheresAndCircles) {
            double rsqr;
            if (numdim == 3) {
                rsqr = Square(test.x - xrange.first) + Square(test.y - yrange.first) +
                       Square(test.z - zrange.first);
            } else if (numdim == 2) {
                rsqr = Square(test.x - xrange.first) + Square(test.y - yrange.first);
            } else {
                rsqr = Square(test.x - xrange.first);
            }

            auto const inside = (rrange.second > rrange.first)
                                    ? rsqr >= Square(rrange.first) && rsqr <= Square(rrange.second)
                                    : rsqr <= Square(rrange.first);

            if (inside == use_inside) {
                return n;
            }
        } else if (options.use == ZoneShape::ConesAndWedges) {
            if (numdim == 3) {
                auto const denom = Square(dxtest) + Square(dytest) + Square(dztest);

                if (rrange.first >= 0.0 && rrange.second >= 0.0 && denom > 0.0) {
                    auto const s =
                        (dxtest * (test.x - xrange.first) + dytest * (test.y - yrange.first) +
                         dztest * (test.z - zrange.first)) /
                        denom;
                    auto const rctest = rrange.first + s * drtest;
                    auto const xctest = xrange.first + s * dxtest;
                    auto const yctest = yrange.first + s * dytest;
                    auto const zctest = zrange.first + s * dztest;

                    auto const rtest = Square(rctest);

                    auto const rsqr =
                        Square(xctest - test.x) + Square(yctest - test.y) + Square(zctest - test.z);

                    auto const inside = (s >= 0.0 && s <= 1.0 && rsqr <= rtest);
                    if (inside == use_inside) {
                        return n;
                    }
                }
            } else if (numdim == 2) {
                auto const denom = Square(dxtest) + Square(dytest);

                if (rrange.first >= 0.0 && rrange.second >= 0.0 && denom > 0.0) {
                    auto const s =
                        (dxtest * (test.x - xrange.first) + dytest * (test.y - yrange.first)) /
                        denom;
                    auto const rctest = rrange.first + s * drtest;
                    auto const xctest = xrange.first + s * dxtest;
                    auto const yctest = yrange.first + s * dytest;

                    auto const rtest = Square(rctest);

                    auto const rsqr = Square(xctest - test.x) + Square(yctest - test.y);

                    auto const inside = (s >= 0.0 && s <= 1.0 && rsqr <= rtest);
                    if (inside == use_inside) {
                        return n;
                    }
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
                    auto const s =
                        (dxtest * (test.x - xrange.first) + dytest * (test.y - yrange.first) +
                         dztest * (test.z - zrange.first)) *
                        factor;
                    auto const xctest = xrange.first + s * dxtest;
                    auto const yctest = yrange.first + s * dytest;
                    auto const zctest = zrange.first + s * dztest;

                    auto const rsqr =
                        Square(xctest - test.x) + Square(yctest - test.y) + Square(zctest - test.z);

                    auto const inside = s >= 0.0 && s <= 1.0 && rsqr >= Square(rrange.first) &&
                                        rsqr <= Square(rrange.second);

                    if (inside == use_inside) {
                        return n;
                    }
                }
            } else if (numdim == 2) {
                auto const denom = Square(dxtest) + Square(dytest);

                if (rrange.first >= 0.0 && rrange.second > rrange.first && denom > 0.0) {
                    auto const factor = 1.0 / denom;
                    auto const s =
                        (dxtest * (test.x - xrange.first) + dytest * (test.y - yrange.first)) *
                        factor;
                    auto const xctest = xrange.first + s * dxtest;
                    auto const yctest = yrange.first + s * dytest;

                    auto const rsqr = Square(xctest - test.x) + Square(yctest - test.y);

                    auto const inside = s >= 0.0 && s <= 1.0 && rsqr >= Square(rrange.first) &&
                                        rsqr <= Square(rrange.second);

                    if (inside == use_inside) {
                        return n;
                    }
                }
            } else {
                EE_RAISE("typdzn 1st digit of 3 selected a ZoneShape::Cylinder DZN for a "
                         << numdim
                         << "dimension problem. Cylinder DZNs are only supported for 2D and "
                            "3D problems.");
            }
        }
    }

    return nonstd::nullopt;

    EE_DIAG_POST
}

template <typename FilterFn>
nonstd::optional<eap::local_index_t>
eap::mesh::SplitDZNWithFilter(DefinedZoneResolutionInfo const &dzn,
                              std::uint8_t numdim,
                              double time,
                              Coordinates<double> test,
                              local_index_t level_base,
                              FilterFn filter) {
    if (auto found_dzn = FindDZNWithFilter(dzn, numdim, time, test, filter)) {
        auto const n = *found_dzn;
        auto const scratch = dzn.levels(n);

        // The level that splitdzn sets is the final listed DZN which matches.
        if (dzn.options(n).GetAdd()) {
            if (scratch != 0) {
                return std::max(local_index_t(1), level_base + scratch);
            }
        } else {
            if (scratch > 0) {
                return scratch;
            }
        }
    }

    return nonstd::nullopt;
}

#endif // EAP_MESH_DZN_COMPUTE_HPP_