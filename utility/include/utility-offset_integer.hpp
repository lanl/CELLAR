/**
 * @file utility-offset_integer.hpp
 *
 * @brief OffsetInteger is an abstraction for storing an integer offset from its logical value.
 * @date 2019-02-28
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_UTILITY_OFFSET_INTEGER_HPP_
#define EAP_UTILITY_OFFSET_INTEGER_HPP_

// STL Includes
#include <ostream>

// Third Party Includes
#include <Kokkos_Core.hpp>
#include <mpi/datatype.hpp>

namespace eap {
namespace utility {

/**
 * @brief
 * Abstraction that mirrors an integer type which stores the value offset by some value Offset
 *
 * @tparam Integer Integer type. Must satisfy is_integer
 * @tparam Offset Constant value to offset by. Stored value is logical value + Offset
 */
template <typename Integer, Integer Offset>
class OffsetInteger {
  public:
    // Constructors
    constexpr OffsetInteger() noexcept = default;
    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger(Integer value) noexcept
        : internal_(value + Offset) {}
    constexpr OffsetInteger(OffsetInteger const &) noexcept = default;
    constexpr OffsetInteger(OffsetInteger &&) noexcept = default;

    // Implicit Conversions
    KOKKOS_FORCEINLINE_FUNCTION constexpr operator Integer() const noexcept {
        return internal_ - Offset;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr operator Integer() const volatile noexcept {
        return internal_ - Offset;
    }

    // Assignment Operators
    // We must overload all assignment operators to allow modification in-place, even if it doesn't
    // offer performance advantage
    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger &operator=(Integer const &value) noexcept {
        internal_ = value + Offset;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr void operator=(Integer const &value) volatile noexcept {
        internal_ = value + Offset;
    }

    constexpr OffsetInteger &operator=(OffsetInteger const &) noexcept = default;
    constexpr OffsetInteger &operator=(OffsetInteger &&) noexcept = default;

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger &operator+=(Integer val) noexcept {
        internal_ += val;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger &operator-=(Integer val) noexcept {
        internal_ -= val;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger &operator*=(Integer val) noexcept {
        return (*this = Integer(*this) * val);
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger &operator/=(Integer val) noexcept {
        return (*this = Integer(*this) / val);
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger &operator%=(Integer val) noexcept {
        return (*this = Integer(*this) % val);
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger &operator&=(Integer val) noexcept {
        return (*this = Integer(*this) & val);
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger &operator|=(Integer val) noexcept {
        return (*this = Integer(*this) | val);
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger &operator^=(Integer val) noexcept {
        return (*this = Integer(*this) ^ val);
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger &operator<<=(Integer val) noexcept {
        return (*this = Integer(*this) << val);
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger &operator>>=(Integer val) noexcept {
        return (*this = Integer(*this) >> val);
    }

    // Increment/Decrement
    // Allows modification in-place
    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger operator++() noexcept {
        ++internal_;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger operator++(int) noexcept {
        OffsetInteger result = *this;
        ++(*this);
        return result;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger operator--() noexcept {
        --internal_;
        return *this;
    }

    KOKKOS_FORCEINLINE_FUNCTION constexpr OffsetInteger operator--(int) noexcept {
        OffsetInteger result = *this;
        --(*this);
        return result;
    }

    // Stream overload
    friend std::ostream &operator<<(std::ostream &os, OffsetInteger const &val) {
        os << Integer(val);
        return os;
    }

  private:
    Integer internal_;
};

// Relational overloads
template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<(OffsetInteger<Integer, Offset> const &x,
          OffsetInteger<Integer, Offset> const &y) noexcept {
    return Integer(x) < Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<=(OffsetInteger<Integer, Offset> const &x,
           OffsetInteger<Integer, Offset> const &y) noexcept {
    return Integer(x) <= Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>(OffsetInteger<Integer, Offset> const &x,
          OffsetInteger<Integer, Offset> const &y) noexcept {
    return Integer(x) > Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>=(OffsetInteger<Integer, Offset> const &x,
           OffsetInteger<Integer, Offset> const &y) noexcept {
    return Integer(x) >= Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator==(OffsetInteger<Integer, Offset> const &x,
           OffsetInteger<Integer, Offset> const &y) noexcept {
    return Integer(x) == Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator!=(OffsetInteger<Integer, Offset> const &x,
           OffsetInteger<Integer, Offset> const &y) noexcept {
    return Integer(x) != Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<(OffsetInteger<Integer, Offset> const volatile &x,
          OffsetInteger<Integer, Offset> const volatile &y) noexcept {
    return Integer(x) < Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<=(OffsetInteger<Integer, Offset> const volatile &x,
           OffsetInteger<Integer, Offset> const volatile &y) noexcept {
    return Integer(x) <= Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>(OffsetInteger<Integer, Offset> const volatile &x,
          OffsetInteger<Integer, Offset> const volatile &y) noexcept {
    return Integer(x) > Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>=(OffsetInteger<Integer, Offset> const volatile &x,
           OffsetInteger<Integer, Offset> const volatile &y) noexcept {
    return Integer(x) >= Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator==(OffsetInteger<Integer, Offset> const volatile &x,
           OffsetInteger<Integer, Offset> const volatile &y) noexcept {
    return Integer(x) == Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator!=(OffsetInteger<Integer, Offset> const volatile &x,
           OffsetInteger<Integer, Offset> const volatile &y) noexcept {
    return Integer(x) != Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool operator<(OffsetInteger<Integer, Offset> const &x,
                                                     Integer const &y) noexcept {
    return Integer(x) < y;
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool operator<=(OffsetInteger<Integer, Offset> const &x,
                                                      Integer const &y) noexcept {
    return Integer(x) <= y;
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool operator>(OffsetInteger<Integer, Offset> const &x,
                                                     Integer const &y) noexcept {
    return Integer(x) > y;
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool operator>=(OffsetInteger<Integer, Offset> const &x,
                                                      Integer const &y) noexcept {
    return Integer(x) >= y;
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool operator==(OffsetInteger<Integer, Offset> const &x,
                                                      Integer const &y) noexcept {
    return Integer(x) == y;
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool operator!=(OffsetInteger<Integer, Offset> const &x,
                                                      Integer const &y) noexcept {
    return Integer(x) != y;
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<(Integer const &x, OffsetInteger<Integer, Offset> const &y) noexcept {
    return x < Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator<=(Integer const &x, OffsetInteger<Integer, Offset> const &y) noexcept {
    return x <= Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>(Integer const &x, OffsetInteger<Integer, Offset> const &y) noexcept {
    return x > Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator>=(Integer const &x, OffsetInteger<Integer, Offset> const &y) noexcept {
    return x >= Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator==(Integer const &x, OffsetInteger<Integer, Offset> const &y) noexcept {
    return x == Integer(y);
}

template <typename Integer, Integer Offset>
KOKKOS_FORCEINLINE_FUNCTION constexpr bool
operator!=(Integer const &x, OffsetInteger<Integer, Offset> const &y) noexcept {
    return x != Integer(y);
}
} // namespace utility
} // namespace eap

namespace mpi {
// An OffsetInteger behaves in MPI is as-if it was the underlying Integer
template <typename Integer, Integer Offset>
struct DatatypeTraits<eap::utility::OffsetInteger<Integer, Offset>>
    : public DatatypeTraits<Integer> {};
} // namespace mpi

namespace Kokkos {
// An OffsetInteger behaves in Kokkos only has the max and min reduction operations
template <typename Integer, Integer Offset>
struct reduction_identity<eap::utility::OffsetInteger<Integer, Offset>> {
    KOKKOS_FORCEINLINE_FUNCTION constexpr static eap::utility::OffsetInteger<Integer, Offset>
    max() {
        return eap::utility::OffsetInteger<Integer, Offset>(0);
    }
    KOKKOS_FORCEINLINE_FUNCTION constexpr static eap::utility::OffsetInteger<Integer, Offset>
    min() {
        return eap::utility::OffsetInteger<Integer, Offset>(std::numeric_limits<Integer>::max() -
                                                            Offset);
    }
};
} // namespace Kokkos

#endif // EAP_UTILITY_OFFSET_INTEGER_HPP_