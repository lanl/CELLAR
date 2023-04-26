/**
 * @file mesh-recon_move.hpp
 *
 * @brief Implements a version of eap::comm::Move that uses the same buffer for both send and
 * receive.
 * @date 2019-06-06
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_RECON_MOVE_HPP_
#define EAP_MESH_RECON_MOVE_HPP_

// Third Party Includes
#include <Kokkos_Core.hpp>

// Internal Includes
#include <utility-linear_view.hpp>

// Local Includes
#include "mesh-types.hpp"

namespace eap {
namespace mesh {
class ReconMovePattern {
  public:
    ReconMovePattern(std::span<FortranLocalIndex const> send_start,
                     std::span<local_index_t const> send_length,
                     std::span<FortranLocalIndex const> recv_start,
                     std::span<local_index_t const> recv_length)
        : send_start_(send_start),
          send_length_(send_length),
          recv_start_(recv_start),
          recv_length_(recv_length) {}

    template <typename View>
    void Move(View const &data) {
        using CopyView = typename View::const_type;
        using CopyViewMut = typename View::non_const_type;

        CopyView copy = [&data]() {
            CopyViewMut copy("ReconMovePattern::Move::copy", data.extent(0));
            Kokkos::deep_copy(copy, data);
            return copy;
        }();

        auto const linear_copy = eap::utility::MakeLinearView(copy);
        auto linear_data = eap::utility::MakeLinearView(data);

        eap::comm::Move(mpi::Comm::world(),
                        send_start_,
                        send_length_,
                        linear_copy.Span(),
                        recv_start_,
                        recv_length_,
                        linear_data.Span());
    }

  private:
    std::span<FortranLocalIndex const> send_start_;
    std::span<local_index_t const> send_length_;
    std::span<FortranLocalIndex const> recv_start_;
    std::span<local_index_t const> recv_length_;
};
} // namespace mesh
} // namespace eap

#endif // EAP_MESH_RECON_MOVE_HPP_