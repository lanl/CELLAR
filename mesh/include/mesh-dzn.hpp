/**
 * @file mesh-dzn.hpp
 *
 * @brief Defined Zone Resolution
 * @date 2019-07-01
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_DZN_HPP_
#define EAP_MESH_DZN_HPP_

// STL Includes
#include <cstdint>

// Third Party Includes
#include <Kokkos_Core.hpp>
#include <optional>

// Internal Includes
#include <error-macros.hpp>
#include <utility-math.hpp>
#include <utility-memory.hpp>

// Local Includes
#include <mesh-constants.hpp>
#include <mesh-types.hpp>

namespace eap {
namespace mesh {
enum class Kode : std::uint8_t {
    /**
     * @brief Applies DZN without considering DZN's material when requested.
     */
    IgnoreMaterial = 0,
    /**
     * @brief Applies DZN only if it specifies a material and it matches.
     */
    Material = 1,
    /**
     * @brief Applies DZN to all requests.
     */
    Unconditional = 2,
    /**
     * @brief Applies DZN if it doesn't specify a material
     */
    MissingMaterial = 3,
};

/**
 * @brief Shape of DZN
 */
enum class ZoneShape : std::uint8_t {
    BricksAndRectangles = 0,
    SpheresAndCircles = 1,
    ConesAndWedges = 2,
    Cylinder = 3,
};

/**
 * @brief Options applied to a DZN
 */
struct ZoneOptions {
    /**
     * @brief A filter for the DZN that is applied based on `kode` passed to the DZN routine.
     */
    Kode kode;
    /**
     * @brief The shape of the DZN region
     */
    ZoneShape use;
    /**
     * @brief If true, the resolution adjustment is relative. If false, it's absolute.
     */
    bool add;
    /**
     * @brief If true, the DZN applies to points outside of it. If false, it applies to points
     * inside it.
     */
    bool outer;
};

/**
 * @brief The Fortran representation of ZoneOptions.
 *
 * @details
 *
 *  The typdzn parameter is an integer, each of whose lowest 4 digits define a
 *  different bit of functionality. For example, say we have a typdzn value of
 *
 *      typdzn = wxyz
 *
 *  where w,x,y,z each stand for one digit integers.
 *
 *  Valid values of w are 0 and 1 (really 0 and not-0, but lets be simple here)
 *
 *     0 defines the condition that we want the resolution INSIDE our control
 *       volume to be resolved to at least some level
 *     1 defines the condition that we want the resolution OUTSIDE our control
 *       volume to be resolved to at least some level
 *
 *  Valid values of x are 0, 1, 2 and 3
 *       This digit appears to restrict whether a particular dzn condition is
 *       active based on whether it is composed of a given material or not,
 *       via its matdzn value. This is a big mess, because a lot of the
 *       code conditionals for calling splitdzn depend on this digit and are
 *       outside of the routine itself.
 *
 *     0 Don't pay attention to matdzn value
 *
 *     1 Do level specification only if we have a matdzn and its the right one
 *
 *     2 Do level specification unconditionally, without regard to matdzn value
 *
 *     3 Do level specification only if we don't have a matdzn specified
 *
 *  Valid values of y are 0 and 1
 *     0 determine the resolution based on externally derived (to this
 *       subroutine) criteria, or if we are inside the DZN, by the DZN level
 *       specified by the user.
 *     1 determine the resolution based on the number of levels +/- the
 *       resolution defined by externally parameter
 *
 *  Valid values of z are 0, 1, 2 and 3
 *     0 defines a square/rectangular region
 *     1 defines a circular/spherical region
 *     2 defines a conic/wedge        region
 *     3 defines a cylinder/tube      region
 *     For each of these choices, a number of additional input deck arrays
 *     must be defined, to specify the size and extent of the DZN region.
 */
class ZoneOptionsDigits {
  public:
    ZoneOptionsDigits() = default;
    explicit ZoneOptionsDigits(int digits);

    /**
     * @brief Converts the digits to a ZoneOptions structure.
     *
     * @return The options specified by the digits
     */
    KOKKOS_FUNCTION constexpr ZoneOptions GetOptions() const {
        return ZoneOptions{GetKode(), GetZoneShape(), GetAdd(), GetOuter()};
    }

    KOKKOS_FUNCTION constexpr ZoneShape GetZoneShape() const {
        return static_cast<ZoneShape>(digits_ % 10);
    }
    KOKKOS_FUNCTION constexpr bool GetAdd() const { return ((digits_ / 10) % 10) != 0; }
    KOKKOS_FUNCTION constexpr Kode GetKode() const {
        return static_cast<Kode>((digits_ / 100) % 10);
    }
    KOKKOS_FUNCTION constexpr bool GetOuter() const { return ((digits_ / 1000) % 10) != 0; }

    /**
     * @brief Sets digits to values from ZoneOptions
     *
     * @param opts A set of ZoneOptions
     */
    void SetOptions(ZoneOptions const &opts);

  private:
    int digits_;
};

struct NewDzn {
    ZoneOptionsDigits options;
    std::optional<local_diff_t> level;
    std::optional<local_index_t> material;
    std::optional<Kokkos::pair<double, double>> r;
    std::optional<Kokkos::pair<double, double>> rd;
    std::optional<Kokkos::pair<double, double>> x;
    std::optional<Kokkos::pair<double, double>> xd;
    std::optional<Kokkos::pair<double, double>> y;
    std::optional<Kokkos::pair<double, double>> yd;
    std::optional<Kokkos::pair<double, double>> z;
    std::optional<Kokkos::pair<double, double>> zd;
    std::optional<double> size;
};

/**
 * @brief Holds a set of DZN regions.
 */
struct DefinedZoneResolutionInfo {
    local_index_t numdzn;

    /** @brief Options for each DZN */
    Kokkos::View<ZoneOptionsDigits *, eap::HostMemorySpace> options;

    /** @brief Absolute or relative level for each DZN */
    Kokkos::View<local_diff_t *, eap::HostMemorySpace> levels;

    /** @brief Material, or lack thereof, for each DZN */
    Kokkos::View<OptionalFortranLocalIndex *, eap::HostMemorySpace> materials;

    /** @brief Radius range for sphere/cone/cylinder DZNs */
    Kokkos::View<Kokkos::pair<double, double> *, eap::HostMemorySpace> radiuses;
    /** @brief Radius range deltas for sphere/cone/cylinder DZNs */
    Kokkos::View<Kokkos::pair<double, double> *, eap::HostMemorySpace> radius_deltas;
    /** @brief x range for bricks/cones/cylinder DZNs */
    Kokkos::View<Kokkos::pair<double, double> *, eap::HostMemorySpace> xs;
    /** @brief x delta range for bricks/cones/cylinder DZNs */
    Kokkos::View<Kokkos::pair<double, double> *, eap::HostMemorySpace> x_deltas;
    /** @brief y range for bricks/cones/cylinder DZNs */
    Kokkos::View<Kokkos::pair<double, double> *, eap::HostMemorySpace> ys;
    /** @brief y delta range for bricks/cones/cylinder DZNs */
    Kokkos::View<Kokkos::pair<double, double> *, eap::HostMemorySpace> y_deltas;
    /** @brief z range for bricks/cones/cylinder DZNs */
    Kokkos::View<Kokkos::pair<double, double> *, eap::HostMemorySpace> zs;
    /** @brief z delta range for bricks/cones/cylinder DZNs */
    Kokkos::View<Kokkos::pair<double, double> *, eap::HostMemorySpace> z_deltas;

    /** @brief cell size in cm to be forced. Only considers if levels for this DZN is 0 */
    Kokkos::View<double *, eap::HostMemorySpace> sizes;

    /** @brief Construct the DefinedZoneResolutionInfo */
    DefinedZoneResolutionInfo();

    /** @brief Gets a SubView of the DZN info */
    DefinedZoneResolutionInfo SubView(local_index_t nlow, local_index_t nhigh) const;

    /** @brief Reserves some space */
    void Reserve(local_index_t size);
    /** @brief Resizes the array. */
    void Resize(local_index_t size);
    /** @brief Grows the array by `size` */
    void Grow(local_index_t size);

    /** @brief Pushes a new dzn info */
    void Push(NewDzn const &dzn_info);
};
} // namespace mesh
} // namespace eap

#endif // EAP_MESH_DZN_HPP_