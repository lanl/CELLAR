/**
 * @file totalview-viz.hpp
 *
 * @brief Utilities for defining C++View Type Transformations for TotalView.
 * @date 2019-07-10
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_TOTALVIEW_VIZ_HPP_
#define EAP_TOTALVIEW_VIZ_HPP_

#ifdef EAP_TOTALVIEW
// STL Includes
#include <array>
#include <exception>
#include <sstream>
#include <type_traits>

// Third Party Includes
#include <Kokkos_Core.hpp>
#include <Kokkos_DynRankView.hpp>
#include <tv_data_display.h>

// Internal Includes
#include <utility-memory.hpp>
#endif // EAP_TOTALVIEW

namespace eap {
namespace totalview {
#ifdef EAP_TOTALVIEW
void AddRow(const char *field_name, const char *type_name, const void *value);

class TTFError : public std::exception {
  public:
    explicit TTFError(int code) : code_(code) {}

    virtual const char *what() const noexcept override;

    int code() const { return code_; }

  private:
    int code_;
};

namespace internal {
template <typename T>
void MarkImplAsUsed() {}

void PrintView(unsigned rank, Kokkos::LayoutStride const &layout, void const *data);

template <typename DataType>
TV_ttf_format_result PrintDynView(
    Kokkos::DynRankView<DataType, Kokkos::LayoutStride, eap::HostMemorySpace> const &dyn_view) {

    PrintView(dyn_view.rank(), dyn_view.layout(), dyn_view.data());
    return TV_ttf_format_ok;
}

template <typename View>
TV_ttf_format_result DisplayView(View const &v) {
    if (!v.span_is_contiguous()) return TV_ttf_format_raw;

    Kokkos::DynRankView<typename View::const_value_type,
                        typename View::array_layout,
                        typename View::memory_space>
        dyn_view = v;
    Kokkos::DynRankView<typename View::const_value_type,
                        Kokkos::LayoutStride,
                        typename View::memory_space>
        dyn_view_stride = dyn_view;

    return PrintDynView(dyn_view_stride);
}
} // namespace internal

#endif // EAP_TOTALVIEW
} // namespace totalview
} // namespace eap

// Macros
#ifdef EAP_TOTALVIEW

#define EAP_TOTALVIEW_DEFINE_DISPLAY_VIEW(...)                                                     \
    int TV_ttf_display_type(__VA_ARGS__ const *v) {                                                \
        try {                                                                                      \
            return ::eap::totalview::internal::DisplayView(*v);                                    \
        } catch (::eap::totalview::TTFError const &) {                                             \
            return TV_ttf_format_failed;                                                           \
        }                                                                                          \
    }
#else
#define EAP_TOTALVIEW_DEFINE_DISPLAY_VIEW(...)
#endif // EAP_TOTALVIEW

#define EAP_TOTALVIEW_DEFINE_COMMON_FOR_DATA_TYPE(data_type)                                       \
    EAP_TOTALVIEW_DEFINE_DISPLAY_VIEW(Kokkos::View<data_type>)                                     \
    EAP_TOTALVIEW_DEFINE_DISPLAY_VIEW(Kokkos::View<data_type, Kokkos::LayoutRight>)                \
    EAP_TOTALVIEW_DEFINE_DISPLAY_VIEW(Kokkos::View<data_type, Kokkos::LayoutLeft>)                 \
    EAP_TOTALVIEW_DEFINE_DISPLAY_VIEW(Kokkos::View<data_type, Kokkos::LayoutStride>)               \
    EAP_TOTALVIEW_DEFINE_DISPLAY_VIEW(                                                             \
        Kokkos::View<data_type, Kokkos::LayoutRight, eap::HostMemorySpace>)                        \
    EAP_TOTALVIEW_DEFINE_DISPLAY_VIEW(                                                             \
        Kokkos::View<data_type, Kokkos::LayoutLeft, eap::HostMemorySpace>)                         \
    EAP_TOTALVIEW_DEFINE_DISPLAY_VIEW(                                                             \
        Kokkos::View<data_type, Kokkos::LayoutStride, eap::HostMemorySpace>)

#define EAP_TOTALVIEW_DEFINE_COMMON_FOR_VALUE_TYPE(value_type)                                     \
    EAP_TOTALVIEW_DEFINE_COMMON_FOR_DATA_TYPE(value_type *)                                        \
    EAP_TOTALVIEW_DEFINE_COMMON_FOR_DATA_TYPE(value_type const *)                                  \
    EAP_TOTALVIEW_DEFINE_COMMON_FOR_DATA_TYPE(value_type **)                                       \
    EAP_TOTALVIEW_DEFINE_COMMON_FOR_DATA_TYPE(value_type const **)                                 \
    EAP_TOTALVIEW_DEFINE_COMMON_FOR_DATA_TYPE(value_type ***)                                      \
    EAP_TOTALVIEW_DEFINE_COMMON_FOR_DATA_TYPE(value_type const ***)

#endif // EAP_TOTALVIEW_VIZ_HPP_