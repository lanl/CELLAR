/**
 * @file utility-optional_integer.hpp
 *
 * @brief OptionalInteger is a tool for setting an invalid sentinel value for an integer.
 * @date 2019-02-28
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_UTILITY_OPTIONAL_INTEGER_HPP_
#define EAP_UTILITY_OPTIONAL_INTEGER_HPP_

// STL Includes
#include <limits>
#include <numeric>
#include <ostream>

// Third Party Includes
#include <Kokkos_Macros.hpp>
#include <mpi/datatype.hpp>
#include <optional>

namespace eap {
namespace utility {

/**
 * @brief A non-type-specific null integer type
 */
struct NullIntegerType {};

/**
 * @brief A value that can be used to reset an OptionalInteger to the sentinel value.
 */
constexpr NullIntegerType nullint;

/**
 * @brief
 *  OptionalInteger is a type mirroring std::optional that stores an integer value where certain
 *  values for that integer are invalid.
 *
 * @detail
 *  OptionalInteger will always have the same size and alignment as Integer.
 *
 * @tparam Integer The type of the internal Integer type
 * @tparam SentinelType
 *  The type of the anchor value for the Integer type. Needed if Integer is not a built-in
 * @tparam ISentinelAnchor An Integer value that is the anchor for the range of invalid values
 * @tparam ToMax
 *  If true, every value >= ISentinelAnchor is invalid. If false, every value <= ISentinelAnchor
 * is invalid
 */
template <typename Integer,
          typename SentinelType = Integer,
          SentinelType ISentinelAnchor = std::is_signed<SentinelType>()
                                             ? std::numeric_limits<SentinelType>::min()
                                             : std::numeric_limits<SentinelType>::max(),
          bool ToMax = !std::is_signed<SentinelType>()>
class OptionalInteger {
  public:
    /**
     * @brief The actual Integer type
     *
     * @detail
     * snake_case in-order to match std::optional
     */
    using value_type = Integer;

    /**
     * @brief The minimum value for the invalid value range of the OptionalInteger
     * specialization
     */
    static constexpr const SentinelType MinValue =
        ToMax ? std::numeric_limits<SentinelType>::min() : ISentinelAnchor + 1;

    /**
     * @brief The maximum value for the invalid value range of the OptionalInteger
     * specialization
     */
    static constexpr const SentinelType MaxValue =
        ToMax ? ISentinelAnchor - 1 : std::numeric_limits<SentinelType>::max();

    /**
     * @brief
     *  The value that will be used as the Sentinel when an OptionalInteger is
     * default-constructed or reset.
     */
    KOKKOS_FORCEINLINE_FUNCTION static constexpr SentinelType Sentinel() {
        return ToMax ? std::numeric_limits<SentinelType>::max() : ISentinelAnchor;
    }

    // Constructors

    /**
     * @brief OptionalInteger is nullint by default
     */
    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger() noexcept : internal_(Sentinel()) {}

    /**
     * @brief OptionalInteger constructed from nullint
     */
    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger(NullIntegerType) noexcept
        : internal_(Sentinel()) {}

    /**
     * @brief OptionalInteger constructed from a value.
     */
    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger(value_type val) noexcept
        : internal_(val) {}

    constexpr OptionalInteger(OptionalInteger const &) noexcept = default;
    constexpr OptionalInteger(OptionalInteger &&) noexcept = default;

    // Assignment Operators
    constexpr OptionalInteger &operator=(OptionalInteger const &) noexcept = default;
    constexpr OptionalInteger &operator=(OptionalInteger &&) noexcept = default;

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &operator=(NullIntegerType) noexcept {
        internal_ = Sentinel();
        return *this;
    }

    template <typename U = value_type>
    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &operator=(U const &value) noexcept {
        internal_ = value;
        return *this;
    }

    template <typename... Args>
    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &
    operator=(OptionalInteger<Args...> const &other) noexcept {
        if (other) {
            internal_ = other.value();
        } else {
            internal_ = Sentinel();
        }
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr void
    operator=(OptionalInteger const &other) volatile noexcept {
        if (other) {
            internal_ = other.value();
        } else {
            internal_ = Sentinel();
        }
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr void
    operator=(OptionalInteger const volatile &other) volatile noexcept {
        if (other) {
            internal_ = other.value();
        } else {
            internal_ = Sentinel();
        }
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger volatile &
    operator=(OptionalInteger &&other) volatile noexcept {
        if (other) {
            internal_ = other.value();
        } else {
            internal_ = Sentinel();
        }
        return *this;
    }

    // Observers
    // Implicit Conversions

    /**
     * @brief Equivalent to `has_value`
     */
    KOKKOS_FORCEINLINE_FUNCTION constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    /**
     * @brief Equivalent to `has_value`
     */
    KOKKOS_FORCEINLINE_FUNCTION constexpr explicit operator bool() const volatile noexcept {
        return has_value();
    }

    // Dereference
    KOKKOS_FORCEINLINE_FUNCTION constexpr value_type &operator*() noexcept { return internal_; }
    KOKKOS_FORCEINLINE_FUNCTION constexpr value_type const &operator*() const noexcept {
        return internal_;
    }
    KOKKOS_FORCEINLINE_FUNCTION constexpr value_type volatile &operator*() volatile noexcept {
        return internal_;
    }
    KOKKOS_FORCEINLINE_FUNCTION constexpr value_type const volatile &operator*() const
        volatile noexcept {
        return internal_;
    }

    // Methods

    /**
     * @brief Checks if value is non-nullint
     *
     * @detail
     * snake_case in-order to match std::optional
     *
     * @return True if the value is not nullint
     */
    KOKKOS_FORCEINLINE_FUNCTION constexpr bool has_value() const noexcept {
        if (ToMax) {
            return internal_ < Sentinel();
        } else {
            return internal_ > Sentinel();
        }
    }

    /**
     * @brief Checks if value is non-nullint
     *
     * @detail
     * snake_case in-order to match std::optional
     *
     * @return True if the value is not nullint
     */
    KOKKOS_FORCEINLINE_FUNCTION constexpr bool has_value() const volatile noexcept {
        if (ToMax) {
            return internal_ < Sentinel();
        } else {
            return internal_ > Sentinel();
        }
    }

    /**
     * @brief
     * Returns a reference to the integer value if `has_value()` is true, otherwise throws
     * `bad_optional_access`.
     *
     * @detail
     * snake_case in-order to match std::optional
     *
     * @return integer reference
     */
    constexpr value_type &value() {
        if (has_value()) {
            return **this;
        } else {
            throw std::bad_optional_access();
        }
    }

    /**
     * @brief
     * Returns a reference to the integer value if `has_value()` is true, otherwise throws
     * `bad_optional_access`.
     *
     * @detail
     * snake_case in-order to match std::optional
     *
     * @return integer reference
     */
    constexpr value_type const &value() const {
        if (has_value()) {
            return **this;
        } else {
            throw std::bad_optional_access();
        }
    }

    /**
     * @brief
     * Returns a reference to the integer value if `has_value()` is true, otherwise throws
     * `bad_optional_access`.
     *
     * @detail
     * snake_case in-order to match std::optional
     *
     * @return integer reference
     */
    constexpr value_type const volatile &value() const volatile {
        if (has_value()) {
            return **this;
        } else {
            throw std::bad_optional_access();
        }
    }

    /**
     * @brief
     * Returns a copy of the integer value if `has_value()` is true, otherwise constructs and
     * returns a new `value_type` from `default_value`
     *
     * @tparam U type of `default_value`. Must be convertible to `value_type`
     * @param default_value The alternate value
     * @return Unwrapped integer or `default_value`
     */
    template <typename U>
    KOKKOS_FORCEINLINE_FUNCTION constexpr value_type value_or(U &&default_value) const noexcept {
        if (has_value()) {
            return **this;
        } else {
            return static_cast<value_type>(std::forward<U>(default_value));
        }
    }

    // Modifiers
    /**
     * @brief Swap `this` with `other`
     *
     * @param other Another OptionalInteger
     */
    KOKKOS_FORCEINLINE_FUNCTION void swap(OptionalInteger &other) noexcept {
        std::swap(internal_, other.internal_);
    }

    /**
     * @brief Resets to nullint
     */
    KOKKOS_FORCEINLINE_FUNCTION constexpr void reset() noexcept { internal_ = Sentinel(); }

    // Integer op assignment
    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &operator+=(Integer val) noexcept {
        if (has_value()) internal_ += val;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &operator-=(Integer val) noexcept {
        if (has_value()) internal_ -= val;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &operator*=(Integer val) noexcept {
        if (has_value()) internal_ *= val;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &operator/=(Integer val) noexcept {
        if (has_value()) internal_ /= val;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &operator%=(Integer val) noexcept {
        if (has_value()) internal_ %= val;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &operator&=(Integer val) noexcept {
        if (has_value()) internal_ &= val;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &operator|=(Integer val) noexcept {
        if (has_value()) internal_ |= val;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &operator^=(Integer val) noexcept {
        if (has_value()) internal_ ^= val;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &operator<<=(Integer val) noexcept {
        if (has_value()) internal_ <<= val;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger &operator>>=(Integer val) noexcept {
        if (has_value()) internal_ >>= val;
        return *this;
    }

    // Increment/Decrement
    // Allows modification in-place
    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger operator++() noexcept {
        if (has_value()) ++internal_;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger operator++(int) noexcept {
        OptionalInteger result = *this;
        if (has_value()) ++(*this);
        return result;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger operator--() noexcept {
        if (has_value()) --internal_;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OptionalInteger operator--(int) noexcept {
        OptionalInteger result = *this;
        if (has_value()) --(*this);
        return result;
    }

    // Stream overload
    friend std::ostream &operator<<(std::ostream &os, OptionalInteger const &val) {
        if (val.has_value()) {
            os << val.value();
        } else {
            os << "nullint";
        }
        return os;
    }

  private:
    value_type internal_;
};

// Optional-to-Optional comparisons
template <typename T,
          typename TSentinelType,
          TSentinelType TISentinelAnchor,
          bool TToMax,
          typename U,
          typename USentinelType,
          USentinelType UISentinelAnchor,
          bool UToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator==(OptionalInteger<T, TSentinelType, TISentinelAnchor, TToMax> const &x,
           OptionalInteger<U, USentinelType, UISentinelAnchor, UToMax> const &y) noexcept {
    if (bool(x) != bool(y)) {
        return false;
    }

    if (!x) {
        return true;
    }

    return *x == *y;
}

template <typename T,
          typename TSentinelType,
          TSentinelType TISentinelAnchor,
          bool TToMax,
          typename U,
          typename USentinelType,
          USentinelType UISentinelAnchor,
          bool UToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator!=(OptionalInteger<T, TSentinelType, TISentinelAnchor, TToMax> const &x,
           OptionalInteger<U, USentinelType, UISentinelAnchor, UToMax> const &y) noexcept {
    return !(x == y);
}

template <typename T,
          typename TSentinelType,
          TSentinelType TISentinelAnchor,
          bool TToMax,
          typename U,
          typename USentinelType,
          USentinelType UISentinelAnchor,
          bool UToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<(OptionalInteger<T, TSentinelType, TISentinelAnchor, TToMax> const &x,
          OptionalInteger<U, USentinelType, UISentinelAnchor, UToMax> const &y) noexcept {
    if (!x) {
        return bool(y); // x is invalid - x < y if y has a value, otherwise equal
    }

    // x is valid from here

    if (!y) {
        return false; // x is valid, y is not - therefore x is > y
    }

    // y is valid from here

    return *x < *y;
}

template <typename T,
          typename TSentinelType,
          TSentinelType TISentinelAnchor,
          bool TToMax,
          typename U,
          typename USentinelType,
          USentinelType UISentinelAnchor,
          bool UToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<=(OptionalInteger<T, TSentinelType, TISentinelAnchor, TToMax> const &x,
           OptionalInteger<U, USentinelType, UISentinelAnchor, UToMax> const &y) noexcept {
    if (!x) {
        return true; // x is invalid - therefore x <= y
    }

    // x is valid from here

    if (!y) {
        return false; // x is valid, y is not - therefore x is > y
    }

    // y is valid from here

    return *x <= *y;
}

template <typename T,
          typename TSentinelType,
          TSentinelType TISentinelAnchor,
          bool TToMax,
          typename U,
          typename USentinelType,
          USentinelType UISentinelAnchor,
          bool UToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>(OptionalInteger<T, TSentinelType, TISentinelAnchor, TToMax> const &x,
          OptionalInteger<U, USentinelType, UISentinelAnchor, UToMax> const &y) noexcept {
    return !(x <= y);
}

template <typename T,
          typename TSentinelType,
          TSentinelType TISentinelAnchor,
          bool TToMax,
          typename U,
          typename USentinelType,
          USentinelType UISentinelAnchor,
          bool UToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>=(OptionalInteger<T, TSentinelType, TISentinelAnchor, TToMax> const &x,
           OptionalInteger<U, USentinelType, UISentinelAnchor, UToMax> const &y) noexcept {
    return !(x < y);
}
template <typename T,
          typename TSentinelType,
          TSentinelType TISentinelAnchor,
          bool TToMax,
          typename U,
          typename USentinelType,
          USentinelType UISentinelAnchor,
          bool UToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator==(OptionalInteger<T, TSentinelType, TISentinelAnchor, TToMax> const volatile &x,
           OptionalInteger<U, USentinelType, UISentinelAnchor, UToMax> const volatile &y) noexcept {
    if (bool(x) != bool(y)) {
        return false;
    }

    if (!x) {
        return true;
    }

    return *x == *y;
}

template <typename T,
          typename TSentinelType,
          TSentinelType TISentinelAnchor,
          bool TToMax,
          typename U,
          typename USentinelType,
          USentinelType UISentinelAnchor,
          bool UToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator!=(OptionalInteger<T, TSentinelType, TISentinelAnchor, TToMax> const volatile &x,
           OptionalInteger<U, USentinelType, UISentinelAnchor, UToMax> const volatile &y) noexcept {
    return !(x == y);
}

template <typename T,
          typename TSentinelType,
          TSentinelType TISentinelAnchor,
          bool TToMax,
          typename U,
          typename USentinelType,
          USentinelType UISentinelAnchor,
          bool UToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<(OptionalInteger<T, TSentinelType, TISentinelAnchor, TToMax> const volatile &x,
          OptionalInteger<U, USentinelType, UISentinelAnchor, UToMax> const volatile &y) noexcept {
    if (!x) {
        return bool(y); // x is invalid - x < y if y has a value, otherwise equal
    }

    // x is valid from here

    if (!y) {
        return false; // x is valid, y is not - therefore x is > y
    }

    // y is valid from here

    return *x < *y;
}

template <typename T,
          typename TSentinelType,
          TSentinelType TISentinelAnchor,
          bool TToMax,
          typename U,
          typename USentinelType,
          USentinelType UISentinelAnchor,
          bool UToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<=(OptionalInteger<T, TSentinelType, TISentinelAnchor, TToMax> const volatile &x,
           OptionalInteger<U, USentinelType, UISentinelAnchor, UToMax> const volatile &y) noexcept {
    if (!x) {
        return true; // x is invalid - therefore x <= y
    }

    // x is valid from here

    if (!y) {
        return false; // x is valid, y is not - therefore x is > y
    }

    // y is valid from here

    return *x <= *y;
}

template <typename T,
          typename TSentinelType,
          TSentinelType TISentinelAnchor,
          bool TToMax,
          typename U,
          typename USentinelType,
          USentinelType UISentinelAnchor,
          bool UToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>(OptionalInteger<T, TSentinelType, TISentinelAnchor, TToMax> const volatile &x,
          OptionalInteger<U, USentinelType, UISentinelAnchor, UToMax> const volatile &y) noexcept {
    return !(x <= y);
}

template <typename T,
          typename TSentinelType,
          TSentinelType TISentinelAnchor,
          bool TToMax,
          typename U,
          typename USentinelType,
          USentinelType UISentinelAnchor,
          bool UToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>=(OptionalInteger<T, TSentinelType, TISentinelAnchor, TToMax> const volatile &x,
           OptionalInteger<U, USentinelType, UISentinelAnchor, UToMax> const volatile &y) noexcept {
    return !(x < y);
}

// Optional-to-nullint comparisons
template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator==(OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt,
           NullIntegerType) noexcept {
    return !opt;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator==(NullIntegerType,
           OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt) noexcept {
    return !opt;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator!=(OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt,
           NullIntegerType) noexcept {
    return bool(opt);
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator!=(NullIntegerType,
           OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt) noexcept {
    return bool(opt);
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<(OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &,
          NullIntegerType) noexcept {
    return false;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<(NullIntegerType,
          OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt) noexcept {
    return bool(opt);
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<=(OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt,
           NullIntegerType) noexcept {
    return !opt;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<=(NullIntegerType,
           OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &) noexcept {
    return true;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>(OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt,
          NullIntegerType) noexcept {
    return bool(opt);
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>(NullIntegerType,
          OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &) noexcept {
    return false;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>=(OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &,
           NullIntegerType) noexcept {
    return true;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>=(NullIntegerType,
           OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt) noexcept {
    return !opt;
}

// Optional-to-T comparisons
template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax, typename U>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator==(OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt,
           U const &val) noexcept {
    return opt ? *opt == val : false;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax, typename U>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator==(U const &val,
           OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt) noexcept {
    return opt ? val == *opt : false;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax, typename U>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator!=(OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt,
           U const &val) noexcept {
    return opt ? *opt != val : true;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax, typename U>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator!=(U const &val,
           OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt) noexcept {
    return opt ? val != *opt : true;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax, typename U>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<(OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt,
          U const &val) noexcept {
    return opt ? *opt < val : true;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax, typename U>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<(U const &val,
          OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt) noexcept {
    return opt ? val < *opt : false;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax, typename U>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<=(OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt,
           U const &val) noexcept {
    return opt ? *opt <= val : true;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax, typename U>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<=(U const &val,
           OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt) noexcept {
    return opt ? val <= *opt : false;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax, typename U>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>(OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt,
          U const &val) noexcept {
    return opt ? *opt > val : false;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax, typename U>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>(U const &val,
          OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt) noexcept {
    return opt ? val > *opt : true;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax, typename U>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>=(OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt,
           U const &val) noexcept {
    return opt ? *opt >= val : false;
}

template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax, typename U>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>=(U const &val,
           OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> const &opt) noexcept {
    return opt ? val >= *opt : true;
}

// Declaration of constant values on Optional Integer.
// Required for C++14: https://stackoverflow.com/a/28846608/700492
// Can be removed in C++17
template <typename Integer, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
constexpr SentinelType OptionalInteger<Integer, SentinelType, ISentinelAnchor, ToMax>::MinValue;

template <typename Integer, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
constexpr SentinelType OptionalInteger<Integer, SentinelType, ISentinelAnchor, ToMax>::MaxValue;

/**
 * @brief Indicates the only valid values of a signed integer are its non-negative values. Negative
 * values are nullint.
 *
 * @tparam Integer Integer type
 */
template <typename Integer, typename = std::enable_if<std::is_signed<Integer>::value>>
using NonNegativeInteger = OptionalInteger<Integer, Integer, -1, false>;
} // namespace utility
} // namespace eap

namespace std {
/**
 * @brief Overload of std::swap for eap::utility::OptionalInteger<T, SentinelType, ISentinelAnchor,
 * ToMax>
 */
template <typename T, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
KOKKOS_FORCEINLINE_FUNCTION void
swap(::eap::utility::OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> &lhs,
     ::eap::utility::OptionalInteger<T, SentinelType, ISentinelAnchor, ToMax> &rhs) noexcept {
    lhs.swap(rhs);
}
} // namespace std

namespace mpi {
/// An OptionalInteger behaves in MPI is as-if it was the underlying Integer
template <typename Integer, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
struct DatatypeTraits<eap::utility::OptionalInteger<Integer, SentinelType, ISentinelAnchor, ToMax>>
    : public DatatypeTraits<Integer> {};
} // namespace mpi

namespace Kokkos {
// An OptionalInteger in Kokkos only has the max and min reduction operations
template <typename Integer, typename SentinelType, SentinelType ISentinelAnchor, bool ToMax>
struct reduction_identity<
    eap::utility::OptionalInteger<Integer, SentinelType, ISentinelAnchor, ToMax>> {
    KOKKOS_FORCEINLINE_FUNCTION constexpr static eap::utility::
        OptionalInteger<Integer, SentinelType, ISentinelAnchor, ToMax>
        max() {
        return eap::utility::nullint;
    }
    KOKKOS_FORCEINLINE_FUNCTION constexpr static eap::utility::
        OptionalInteger<Integer, SentinelType, ISentinelAnchor, ToMax>
        min() {
        return Kokkos::reduction_identity<Integer>::min();
    }
};
} // namespace Kokkos

#endif // EAP_UTILITY_OPTIONAL_INTEGER_HPP_