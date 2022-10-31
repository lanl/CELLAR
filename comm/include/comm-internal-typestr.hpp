#ifndef EAP_COMM_INTERNAL_TYPESTR_HPP_
#define EAP_COMM_INTERNAL_TYPESTR_HPP_

#include <cstddef>

#include <nonstd/string_view.hpp>

#define EAP_COMM_INTERNAL_SPECIALIZE_TYPETOSTR(TypeName)                                           \
    template <>                                                                                    \
    struct type_to_str<TypeName> {                                                                 \
        constexpr static nonstd::string_view name() { return #TypeName; }                          \
    }

namespace eap {
namespace comm {
namespace internal {
template <typename T>
struct type_to_str {
    constexpr static nonstd::string_view name() { return "T"; }
};

EAP_COMM_INTERNAL_SPECIALIZE_TYPETOSTR(char);
EAP_COMM_INTERNAL_SPECIALIZE_TYPETOSTR(std::int32_t);
EAP_COMM_INTERNAL_SPECIALIZE_TYPETOSTR(std::int64_t);
EAP_COMM_INTERNAL_SPECIALIZE_TYPETOSTR(float);
EAP_COMM_INTERNAL_SPECIALIZE_TYPETOSTR(double);
} // namespace internal
} // namespace comm
} // namespace eap

#endif // EAP_COMM_INTERNAL_TYPESTR_HPP_