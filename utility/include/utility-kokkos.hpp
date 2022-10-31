/**
 * @file utility-kokkos.hpp
 *
 * @brief Utilities for working with Kokkos views
 * @date 2018-05-22
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef UTILITY_KOKKOS_HPP
#define UTILITY_KOKKOS_HPP

// STL Includes
#include <iostream>
#include <type_traits>

// Third Party Includes
#include <Kokkos_Core.hpp>

// Internal Includes
#include <abi-ndarray.h>

// Local Includes
#include <utility-memory.hpp>

namespace eap {
namespace utility {
namespace kokkos {
template <typename KV>
using layout_is_left = std::is_same<typename KV::array_layout, Kokkos::LayoutLeft>;

template <typename KV>
constexpr auto layout_is_left_v = layout_is_left<KV>::value;

template <typename KV>
using layout_is_right = std::is_same<typename KV::array_layout, Kokkos::LayoutRight>;

template <typename KV>
constexpr auto layout_is_right_v = layout_is_right<KV>::value;

template <typename KV>
using layout_is_stride = std::is_same<typename KV::array_layout, Kokkos::LayoutStride>;

template <typename KV>
constexpr auto layout_is_stride_v = layout_is_stride<KV>::value;

template <typename ArrayLayout>
using transpose_layout = std::conditional_t<
    std::is_same<ArrayLayout, Kokkos::LayoutLeft>::value,
    Kokkos::LayoutRight,
    std::conditional_t<std::is_same<ArrayLayout, Kokkos::LayoutRight>::value,
                       Kokkos::LayoutLeft,
                       std::conditional_t<std::is_same<ArrayLayout, Kokkos::LayoutStride>::value,
                                          Kokkos::LayoutStride,
                                          void>>>;

template <typename KV>
using transpose_mapping =
    Kokkos::View<typename KV::data_type,
                 transpose_layout<typename KV::array_layout>,
                 typename KV::execution_space,
                 //    typename KV::memory_traits, // TODO: Figure out how to merge memory traits
                 Kokkos::MemoryUnmanaged>;

template <typename KV>
constexpr std::enable_if_t<KV::rank == 2 && (layout_is_left_v<KV> || layout_is_right_v<KV>),
                           transpose_mapping<KV>>
Transpose(KV const &view) {
    return transpose_mapping<KV>(view.data(), view.extent(1), view.extent(0));
}

template <typename KV>
constexpr std::enable_if_t<KV::rank == 2 && layout_is_stride_v<KV>, transpose_mapping<KV>>
Transpose(KV const &view) {
    Kokkos::LayoutStride layout(view.extent(1), view.stride_1(), view.extent(0), view.stride_0());

    return transpose_mapping<KV>(view.data(), layout);
}

template <typename KV>
std::enable_if_t<KV::rank == 2, std::basic_ostream<char> &> operator<<(std::basic_ostream<char> &ss,
                                                                       KV const &view) {
    ss << "[" << std::endl;
    for (size_t i = 0; i < view.extent(0); i++) {
        ss << "\t[";
        for (size_t j = 0; j < view.extent(1); j++) {
            if (j != 0) {
                ss << ", ";
            }
            ss << view(i, j);
        }
        ss << "]";
        if (i != view.extent(0) - 1) {
            ss << ",";
        }
        ss << std::endl;
    }
    ss << "]";

    return ss;
}

template <typename KV>
std::enable_if_t<KV::rank == 2, std::string> PrintView(KV const &view) {
    std::stringstream ss;
    ss << view;
    return ss.str();
}

template <typename T, size_t N>
std::basic_ostream<char> &operator<<(std::basic_ostream<char> &ss,
                                     const Kokkos::Array<T, N> &array) {
    ss << "[";
    for (size_t i = 0; i < N; ++i) {
        if (i != 0) {
            ss << ", ";
        }
        ss << array[i];
    }
    ss << "]";

    return ss;
}

template <typename T, size_t N>
std::string PrintArray(const Kokkos::Array<T, N> &array) {
    std::stringstream ss;
    ss << array;
    return ss.str();
}

template <typename KV>
std::enable_if_t<KV::rank == 1 && (layout_is_left_v<KV> || layout_is_right_v<KV>),
                 Kokkos::View<typename KV::data_type[1],
                              typename KV::array_layout,
                              typename KV::execution_space,
                              Kokkos::MemoryUnmanaged>>
Convert1DTo2D(KV const &view) {
    return Kokkos::View<typename KV::data_type[1],
                        typename KV::array_layout,
                        typename KV::execution_space,
                        Kokkos::MemoryUnmanaged>(view.data(), view.extent(0));
}

template <typename KV>
std::enable_if_t<KV::rank == 1 && layout_is_stride_v<KV>,
                 Kokkos::View<typename KV::data_type[1],
                              Kokkos::LayoutStride,
                              typename KV::execution_space,
                              Kokkos::MemoryUnmanaged>>
Convert1DTo2D(KV const &view) {
    Kokkos::LayoutStride layout(view.extent(0), view.stride_0());

    return Kokkos::View<typename KV::data_type[1],
                        Kokkos::LayoutStride,
                        typename KV::execution_space,
                        Kokkos::MemoryUnmanaged>(view.data(), layout);
}

template <typename DataType>
Kokkos::View<DataType, Kokkos::LayoutStride, eap::HostMemorySpace>
ViewFromNdarray(abi_ndarray_t const &ndarray) {
    size_t dimensions[Kokkos::ARRAY_LAYOUT_MAX_RANK] = {};
    size_t strides[Kokkos::ARRAY_LAYOUT_MAX_RANK] = {};

    using traits = Kokkos::ViewTraits<DataType>;
    using value_type = typename traits::value_type;
    constexpr auto rank = Kokkos::ViewTraits<DataType>::rank;

    if (rank != ndarray.rank) {
        std::cerr << "Requested Kokkos view of rank " << size_t(rank) << " for ndarray with rank "
                  << ndarray.rank << "." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::copy(ndarray.dims, ndarray.dims + ndarray.rank, dimensions);
    std::copy(ndarray.strides, ndarray.strides + ndarray.rank, strides);

    // clang-format off
    Kokkos::LayoutStride layout{
        dimensions[0], strides[0],
        dimensions[1], strides[1],
        dimensions[2], strides[2],
        dimensions[3], strides[3],
        dimensions[4], strides[4],
        dimensions[5], strides[5],
        dimensions[6], strides[6],
        dimensions[7], strides[7]
    };
    // clang-format on

    return Kokkos::
        View<DataType, Kokkos::LayoutStride, eap::HostMemorySpace, Kokkos::MemoryUnmanaged>(
            reinterpret_cast<value_type *>(ndarray.data), layout);
}

template <typename View>
abi_ndarray_t ViewToNdarray(View const &view) {
    abi_ndarray_t nd{};

    using const_data_type = typename View::const_data_type;
    using memory_space = typename View::memory_space;

    Kokkos::View<const_data_type, Kokkos::LayoutStride, memory_space> view_stride = view;

    Kokkos::LayoutStride layout = view_stride.layout();

    nd.rank = view_stride.rank;
    std::copy(layout.dimension, layout.dimension + view_stride.rank, nd.dims);
    std::copy(layout.stride, layout.stride + view_stride.rank, nd.strides);
    nd.data = const_cast<void *>(static_cast<void const *>(view_stride.data()));

    return nd;
}

bool IsLayoutRight(size_t rank,
                   Kokkos::LayoutStride const &stride,
                   Kokkos::LayoutRight *right = nullptr);

template <typename KV>
std::enable_if_t<layout_is_right_v<KV>, bool> IsLayoutRight(KV const &view,
                                                            Kokkos::LayoutRight &right) {
    right = view.layout();
    return true;
}

template <typename KV>
std::enable_if_t<layout_is_left_v<KV> && KV::Rank == 1, bool>
IsLayoutRight(KV const &view, Kokkos::LayoutRight &right) {
    right = Kokkos::LayoutRight(view.extent(0));
    return true;
}

template <typename KV>
std::enable_if_t<layout_is_left_v<KV> && (KV::Rank > 1), bool>
IsLayoutRight(KV const & /*view*/, Kokkos::LayoutRight & /*right*/) {
    return false;
}

template <typename KV>
std::enable_if_t<layout_is_stride_v<KV>, bool> IsLayoutRight(KV const &view,
                                                             Kokkos::LayoutRight &right) {
    return IsLayoutRight(view.Rank, view.layout(), &right);
}

bool IsLayoutLeft(size_t rank,
                  Kokkos::LayoutStride const &stride,
                  Kokkos::LayoutLeft *left = nullptr);

template <typename KV>
std::enable_if_t<layout_is_left_v<KV>, bool> IsLayoutLeft(KV const &view,
                                                          Kokkos::LayoutLeft &left) {
    left = view.layout();
    return true;
}

template <typename KV>
std::enable_if_t<layout_is_right_v<KV> && KV::Rank == 1, bool>
IsLayoutLeft(KV const &view, Kokkos::LayoutLeft &left) {
    left = Kokkos::LayoutLeft(view.extent(0));
    return true;
}

template <typename KV>
std::enable_if_t<layout_is_right_v<KV> && (KV::Rank > 1), bool>
IsLayoutLeft(KV const & /*view*/, Kokkos::LayoutLeft & /*left*/) {
    return false;
}

template <typename KV>
std::enable_if_t<layout_is_stride_v<KV>, bool> IsLayoutLeft(KV const &view,
                                                            Kokkos::LayoutLeft &left) {
    return IsLayoutLeft(view.Rank, view.layout(), &left);
}
} // namespace kokkos
} // namespace utility
} // namespace eap

#endif // UTILITY_KOKKOS_HPP