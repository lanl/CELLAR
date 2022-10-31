/**
 * @file comm-ffi-concretize.hpp
 *
 * @brief Tools for converting generic FFI types to concrete types
 * @date 2019-01-15
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_COMM_FFI_CONCRETIZE_HPP_
#define EAP_COMM_FFI_CONCRETIZE_HPP_

// STL Includes
#include <cstdint>
#include <type_traits>

// Internal Includes
#include <abi-ndarray.h>
#include <utility-kokkos.hpp>
#include <utility-memory.hpp>

// Local Includes
#include <comm-ffi-types.h>

namespace eap {
namespace comm {
namespace ffi {
namespace internal {
template <typename T>
struct count_depth {
    static constexpr size_t value = 0;
};

template <typename T>
struct count_depth<T *> {
    static constexpr size_t value = count_depth<std::remove_pointer_t<T *>>::value + 1;
};

template <typename T>
constexpr size_t count_depth_v = count_depth<T>::value;

static_assert(count_depth_v<void> == 0, "count_depth_v failed");
static_assert(count_depth_v<void *> == 1, "count_depth_v failed");
static_assert(count_depth_v<void **> == 2, "count_depth_v failed");
static_assert(count_depth_v<void ***> == 3, "count_depth_v failed");

template <typename T, size_t N>
struct with_num_dims {
    using type = std::add_pointer_t<typename with_num_dims<T, N - 1>::type>;
};

template <typename T>
struct with_num_dims<T, 0> {
    using type = T;
};

template <typename T, size_t N>
using with_num_dims_t = typename with_num_dims<T, N>::type;

static_assert(std::is_same<int *, with_num_dims_t<int, 1>>::value, "with_num_dims_t failed");
static_assert(std::is_same<int **, with_num_dims_t<int, 2>>::value, "with_num_dims_t failed");
static_assert(std::is_same<int ***, with_num_dims_t<int, 3>>::value, "with_num_dims_t failed");

static_assert(std::is_same<int const *, with_num_dims_t<int const, 1>>::value,
              "with_num_dims_t failed");
static_assert(std::is_same<int const **, with_num_dims_t<int const, 2>>::value,
              "with_num_dims_t failed");
static_assert(std::is_same<int const ***, with_num_dims_t<int const, 3>>::value,
              "with_num_dims_t failed");

template <typename T>
struct is_base_const {
    static constexpr bool value = std::is_const<T>::value;
};

template <typename T>
struct is_base_const<T *> {
    static constexpr bool value = is_base_const<std::remove_pointer_t<T>>::value;
};

template <typename T>
constexpr bool is_base_const_v = is_base_const<T>::value;

template <typename T, typename U>
struct swap_base {
    using type =
        with_num_dims_t<std::conditional_t<is_base_const_v<T>, U const, U>, count_depth_v<T>>;
};

template <typename T, typename U>
using swap_base_t = typename swap_base<T, U>::type;

static_assert(std::is_same<int **, swap_base_t<void **, int>>::value, "swap_base_t failed");
static_assert(std::is_same<int const **, swap_base_t<void const **, int>>::value,
              "swap_base_t failed");
} // namespace internal

/**
 * @brief
 * An adapter type that can cast abi_ndarray_t types to concrete Kokkos::View types with the correct
 * striding information.
 *
 * @tparam DataType
 * A "generic" Kokkos View datatype, where the base type will be replaced with a concrete type
 */
template <typename DataType>
class ToKokkosView {
  public:
    /**
     * @brief A Type that is a Kokkos::View that is returned by `to_view`
     *
     * @tparam T Target base type
     */
    template <typename T>
    using ViewType = Kokkos::View<internal::swap_base_t<DataType, T>,
                                  Kokkos::LayoutStride,
                                  eap::HostMemorySpace,
                                  Kokkos::MemoryUnmanaged>;

    /**
     * @brief Construct a new ToKokkosView object
     *
     * @param array A generic array type
     */
    explicit ToKokkosView(abi_ndarray_t const &array) : array_(array) {}

    /**
     * @brief
     * Produces a Kokkos::View from `array` of a concrete datatype. Behavior undefined if `T` is
     * not a valid type for the buffer pointed to by `array`
     *
     * @tparam T Base type to convert to
     * @return ViewType<T> The Kokkos::View type to use.
     */
    template <typename T>
    ViewType<T> to_view() {
        return utility::kokkos::ViewFromNdarray<internal::swap_base_t<DataType, T>>(array_);
    }

  private:
    abi_ndarray_t const &array_;
};

/**
 * @brief
 * Given a generic datatype `dt`, calls `kernel` with a concrete Kokkos::View type for each
 * ToKokkosView adapted array in `arrays`. `kernel` must take argument as generic type.
 *
 * @details
 * @code{.cpp}
 * // Copy one generic abi_ndarray_t to another
 * void copy_array(eap_comm_datatype_t dt, abi_ndarray_t from, abi_ndarray_t to) {
 *     ConcretizeViews(dt, [](auto from, auto to) {
 *         Kokkos::deep_copy(to, from);
 *     },
 *     // Convert `from` into an immutable 1D array of type `dt`
 *     ToKokkosView<void const *>(from),
 *     // Convert `to` into a mutable 1D array of type `dt`
 *     ToKokkosView<void *>(to));
 * }
 * @endcode
 *
 * @tparam Fn A generic function object
 * @tparam Arrays A list of ToKokosView values
 * @param dt The datatype to concretize to
 * @param kernel A generic function object
 * @param arrays A list of array values adapted using `ToKokkosView`.
 */
template <typename Fn, typename... Arrays>
void ConcretizeViews(eap_comm_datatype_t dt, Fn kernel, Arrays... arrays) {
    switch (dt) {
    case eap_comm_datatype_bool:
        kernel(arrays.template to_view<bool>()...);
        break;
    case eap_comm_datatype_int32:
        kernel(arrays.template to_view<std::int32_t>()...);
        break;
    case eap_comm_datatype_int64:
        kernel(arrays.template to_view<std::int64_t>()...);
        break;
    case eap_comm_datatype_float:
        kernel(arrays.template to_view<float>()...);
        break;
    case eap_comm_datatype_double:
        kernel(arrays.template to_view<double>()...);
        break;
    default:
        // TODO: Use eap-error once available.
        std::cerr << "Invalid datatype: " << dt << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

} // namespace ffi
} // namespace comm
} // namespace eap

#endif // EAP_COMM_FFI_CONCRETIZE_HPP_