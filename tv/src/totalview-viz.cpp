/**
 * @file totalview-viz.cpp
 *
 * @brief C++View Type Transformations for TotalView
 * @date 2019-07-10
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Self Include
#include <totalview-viz.hpp>

// STL Includes
#include <cstdint>

// Third Party Includes
#include <Kokkos_Core.hpp>

// Internal Includes
#include <utility-kokkos.hpp>

using namespace eap::totalview;

using eap::utility::kokkos::IsLayoutLeft;
using eap::utility::kokkos::IsLayoutRight;

extern template void eap::totalview::internal::MarkImplAsUsed<bool>();
extern template void eap::totalview::internal::MarkImplAsUsed<std::int32_t>();
extern template void eap::totalview::internal::MarkImplAsUsed<std::uint32_t>();
extern template void eap::totalview::internal::MarkImplAsUsed<std::int64_t>();
extern template void eap::totalview::internal::MarkImplAsUsed<std::uint64_t>();
extern template void eap::totalview::internal::MarkImplAsUsed<float>();
extern template void eap::totalview::internal::MarkImplAsUsed<double>();

extern "C" void eap_totalview_mark_as_used() {
    internal::MarkImplAsUsed<bool>();
    internal::MarkImplAsUsed<std::int32_t>();
    internal::MarkImplAsUsed<std::uint32_t>();
    internal::MarkImplAsUsed<std::int64_t>();
    internal::MarkImplAsUsed<std::uint64_t>();
    internal::MarkImplAsUsed<float>();
    internal::MarkImplAsUsed<double>();
}

const char *TTFError::what() const noexcept {
    switch (code_) {
    case TV_ttf_ec_not_active:
        return "TV_ttf_ec_not_active";
    case TV_ttf_ec_invalid_characters:
        return "TV_ttf_ec_invalid_characters";
    case TV_ttf_ec_buffer_exhausted:
        return "TV_ttf_ec_buffer_exhausted";
    default:
        return "TTFError: Unknown TV_ttf_error_codes";
    }
}

void eap::totalview::AddRow(const char *field_name, const char *type_name, const void *value) {
    auto const code = TV_ttf_add_row(field_name, type_name, value);
    if (TV_ttf_ec_ok != code) {
        throw TTFError(code);
    }
}

void eap::totalview::internal::PrintView(unsigned rank,
                                         Kokkos::LayoutStride const &layout,
                                         void const *data) {

    std::stringstream ss;
    ss << "value_type";

    if (IsLayoutRight(rank, layout)) {
        AddRow("layout", TV_ttf_type_ascii_string, "LayoutRight");

        for (unsigned i = 0; i < rank; i++) {
            ss << "[" << layout.dimension[i] << "]";
        }

        auto const values_type = ss.str();
        AddRow("values", values_type.c_str(), data);
    } else if (IsLayoutLeft(rank, layout)) {
        AddRow("layout", TV_ttf_type_ascii_string, "LayoutLeft");

        unsigned i = rank;
        while (i-- > 0) {
            ss << "[" << layout.dimension[i] << "]";
        }

        auto const values_type = ss.str();
        AddRow("values", values_type.c_str(), data);
    } else {
        AddRow("layout", TV_ttf_type_ascii_string, "Unknown");
    }
}
