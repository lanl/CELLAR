/**
 * @file utility-linear_view.hpp
 *
 * @brief Implements a data structure to facilitate working with non-unit stride Kokkos::Views.
 * @date 2019-01-10
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_UTILITY_LINEAR_VIEW_HPP_
#define EAP_UTILITY_LINEAR_VIEW_HPP_

// STL Includes
#include <memory>

// Third Party Includes
#include <Kokkos_Core.hpp>
#include <span>

// Local Includes
#include <utility-memory.hpp>

namespace eap {
namespace utility {
/**
 * @brief
 * LinearView exposes a synced view into a potentially non-unit strided buffer. Syncs back to view
 * on destruction.
 *
 * @details
 * Extra allocation is only incurred if the buffer is not flat.
 *
 * Example:
 * ```c++
// Creates a row-major 2D array
Kokkos::View<int **, Kokkos::LayoutRight> arr_2d("test", N, 2);

// Creates a subview of the first dimension, stride of 2
Kokkos::View<int *, Kokkos::LayoutStride> arr = Kokkos::subview(arr_2d, Kokkos::ALL, 0);

ASSERT_EQ(2, arr.stride_0());

{
    // Creates a LinearView - this should allocate a separate, unit-stride linear buffer.
    LinearView<int> arr_view(arr);

    // Get a mutable span for the LinearView
    auto view_span = arr_view.Span();

    // Fill out the span
    std::iota(view_span.begin(), view_span.end(), 0);
}
// LinearView is deallocated, so it should sync back to the original View.

for (auto i = 0; i < N; i++) {
    ASSERT_EQ(i, arr(i));
}
 * ```
 *
 * @tparam T Type of 1D Kokkos::View
 */
template <typename T>
class LinearView {
  public:
    using value_type = T;
    using non_const_value_type = std::remove_const_t<T>;

    explicit LinearView(Kokkos::View<value_type *, Kokkos::LayoutStride, eap::HostMemorySpace> view)
        : view_(std::move(view)) {
        if (view_.stride_0() != 1) {
            flat_buffer_ =
                std::unique_ptr<non_const_value_type[]>(new non_const_value_type[view_.extent(0)]);
            for (size_t i = 0; i < view_.extent(0); i++) {
                flat_buffer_[i] = view_(i);
            }
        }
    }

    LinearView(LinearView const &) = delete;
    LinearView &operator=(LinearView const &) = delete;

    LinearView(LinearView &&) = default;
    LinearView &operator=(LinearView &&) = default;

    ~LinearView() { Sync(); }

    /**
     * @brief Forces sync to the Kokkos::View.
     */
    void Sync() {
        Sync(std::conditional_t<std::is_const<value_type>::value, const_sync_t, mutable_sync_t>{});
    }

    /**
     * @brief Returns a flat, immutable copy of the Kokkos::View. Does not cause the LinearView to
     * sync.
     *
     * @return std::span<T const> A flat view into the Kokkos::View
     */
    std::span<T const> Span() const {
        if (flat_buffer_) {
            return std::span<T const>(flat_buffer_.get(), view_.extent(0));
        } else {
            return std::span<T const>(view_.data(), view_.extent(0));
        }
    }

    /**
     * @brief Returns a flat, mutable copy of the Kokkos::View. Causes LinearView to sync when it is
     * destroyed.
     *
     * @return std::span<T> Mutable, flat view into the Kokkos::View.
     */
    std::span<T> Span() {
        needs_sync_ = true;
        if (flat_buffer_) {
            return std::span<T>(flat_buffer_.get(), view_.extent(0));
        } else {
            return std::span<T>(view_.data(), view_.extent(0));
        }
    }

  private:
    struct mutable_sync_t {};
    struct const_sync_t {};

    void Sync(mutable_sync_t) {
        if (!flat_buffer_) return;
        if (!needs_sync_) return;

        for (size_t i = 0; i < view_.extent(0); i++) {
            view_(i) = flat_buffer_[i];
        }
        needs_sync_ = false;
    }

    void Sync(const_sync_t) {}

    Kokkos::View<value_type *, Kokkos::LayoutStride, eap::HostMemorySpace> view_;
    std::unique_ptr<non_const_value_type[]> flat_buffer_;
    bool needs_sync_ = false;
};

template <typename View>
LinearView<typename View::value_type> MakeLinearView(View view) {
    return LinearView<typename View::value_type>(view);
}
} // namespace utility
} // namespace eap

#endif // EAP_UTILITY_LINEAR_VIEW_HPP_