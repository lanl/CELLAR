#ifndef EAP_MESH_INTERNAL_LOG_HPP
#define EAP_MESH_INTERNAL_LOG_HPP

#include <ostream>

namespace eap {
namespace mesh {
namespace internal {
std::ostream &mesh_err();

[[noreturn]] void unimplemented(char const *description) noexcept;
} // namespace internal
} // namespace mesh
} // namespace eap

#endif // EAP_MESH_INTERNAL_LOG_HPP