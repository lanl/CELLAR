/**
 * @brief mesh-cells.hpp defines the Cells class that holds information about the cells in the mesh
 *
 * @file mesh-cells.hpp
 * @author Jonas Lippuner <jlippuner@lanl.gov>
 * @date 2018-09-27
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_CELLS_HPP_
#define EAP_MESH_CELLS_HPP_

// STL Includes
#include <vector>

// Third Party Includes
#include <mpi/mpi.hpp>

// Internal Includes
#include <comm-token.hpp>
#include <utility-kokkos.hpp>

// Local Includes
#include "mesh-types.hpp"

namespace eap {
namespace mesh {

/**
 * @brief This class holds various arrays containing information about the cells on the current PE.
 * It mirrors the cells_t type in the xRage FORTRAN code.
 */
class Cells {
  public:
    // Default constructor
    explicit Cells(eap::comm::TokenBuilder const &token_builder);

    // Accessors
    auto num_global_cells() const { return num_global_cells_; }
    auto &num_global_cells() { return num_global_cells_; }

    auto num_local_cells() const { return num_local_cells_; }
    auto &num_local_cells() { return num_local_cells_; }

    auto max_num_local_cells() const { return max_num_local_cells_; }
    auto &max_num_local_cells() { return max_num_local_cells_; }

    auto num_local_cells_with_clones() const { return num_local_cells_with_clones_; }
    auto &num_local_cells_with_clones() { return num_local_cells_with_clones_; }

    auto num_clone_cells() const { return num_local_cells_with_clones() - num_local_cells(); }

    auto num_local_reserve_cells() const { return num_local_reserve_cells_; }
    auto &num_local_reserve_cells() { return num_local_reserve_cells_; }

    const auto &all_num_local_cells() const { return all_num_local_cells_; }
    auto &all_num_local_cells() { return all_num_local_cells_; }

    const auto &global_base_address() const { return global_base_address_; }
    auto &global_base_address() { return global_base_address_; }

    const auto &cell_address() const { return cell_address_; }
    auto &cell_address() { return cell_address_; }

    const auto &cell_active() const { return cell_active_; }
    auto &cell_active() { return cell_active_; }

    const auto &cell_center() const { return cell_center_; }
    auto &cell_center() { return cell_center_; }

    const auto &cell_position() const { return cell_position_; }
    auto &cell_position() { return cell_position_; }

    const auto &cell_half() const { return cell_half_; }
    auto &cell_half() { return cell_half_; }

    const auto &cell_half_lo() const { return cell_half_lo_; }
    auto &cell_half_lo() { return cell_half_lo_; }

    const auto &cell_half_hi() const { return cell_half_hi_; }
    auto &cell_half_hi() { return cell_half_hi_; }

    const auto &cell_volume() const { return cell_volume_; }
    auto &cell_volume() { return cell_volume_; }

    bool Global_arrays_allocated() const { return true; }
    bool Cell_arrays_allocated() const { return cell_arrays_allocated_; }

    eap::comm::TokenBuilder &token_builder() { return token_builder_; }
    eap::comm::TokenBuilder const &token_builder() const { return token_builder_; }

    /**
     * @brief Collective. Merely sets the number of local cells and invalidates global cell state.
     *
     * @details This routine is conceptually collective. All ranks must call this routine in
     * parallel. However, no collective communication is incurred.
     *
     * @param num_local_cells Number of cells on this rank.
     */
    void SetNumLocalCells(local_index_t num_local_cells);

    /**
     * @brief Collective. Updates global state arrays if out-of-date.
     *
     * @details This routine is conceptually collective. All ranks must call this routine in
     * parallel. However, collective communication isn't always incurred.
     */
    void UpdateGlobalBase();

    /**
     * @brief Collective. Updates global base address array if out-of-date and ensures that the
     * global address array is correct.
     *
     * @details This routine is conceptually collective. All ranks must call this routine in
     * parallel. However, collective communication isn't always incurred.
     */
    void CheckGlobalBase();

    /**
     * @brief Collective. Prepares the global state arrays for a resize operation.
     *
     * @details This routine is conceptually collective. All ranks must call this routine in
     * parallel. However, collective communication isn't always incurred.
     *
     * @param new_num_local_cells
     */
    void PrepareGlobalBase(local_index_t new_num_local_cells);

    // Allocate and resize arrays

    /**
     * @brief Resize all the local cell arrays
     *
     * @param size      The new size (usually slightly more than number of cells)
     * @param num_dims  The number of dimensions (for 2D arrays)
     */
    void ResizeCellArrays(const size_t size, const size_t num_dims);

    /**
     * @brief Convert a global cell index to the local cell index on this PE
     *
     * @param global_index    The global cell index of the cell
     * @return local_index_t  The corresponding local cell index of the cell
     */
    local_index_t ToLocal(const global_index_t global_index) const {
        auto rank = mpi::Comm::world().rank();
        return local_index_t(global_index - global_base_address_[rank]);
    }

    /**
     * @brief Convert a local cell index on this PE to the global cell index
     *
     * @param local_index     The local cell index of the cell on this PE
     * @return global_index_t The corresponding global cell index of the cell
     */
    global_index_t ToGlobal(const local_index_t local_index) const {
        auto rank = mpi::Comm::world().rank();
        return global_index_t(local_index) + global_base_address_[rank];
    }

    // Methods

    /**
     * @brief Checks if a cell is active
     *
     * @param cell Cell address
     */
    bool IsActive(local_index_t cell) const { return cell_active_(cell); }

    // pseudo-private
    void UpdateGlobalAddress_(local_index_t num_local_cells);
    void CheckGlobalAddress_() const;

  private:
    void SetGlobalBase(local_index_t num_local_cells);

    /**
     * @brief A TokenBuilder that is configured for num_local_cells_
     */
    comm::TokenBuilder token_builder_;

    global_index_t num_global_cells_;           //!< Global number of cells in the entire simulation
    local_index_t num_local_cells_;             //!< Local number of cells on this PE
    local_index_t max_num_local_cells_;         //!< Maximum number of local cells on any PE
    local_index_t num_local_cells_with_clones_; //!< Local number of cells including clone cells

    /**
     * @brief Actual allocation size of num_local_cells_-sized arrays
     *
     * Arrays of logical size num_local_cells_ (one element per local cell) are actually allocated
     * slightly larger so that they can grow easily. This value is the actual size of these arrays.
     */
    local_index_t num_local_reserve_cells_;

    /**
     * @brief Local number of cells on each PE
     *
     * The number of local cells owned by each PE. This is a 0-indexed array in Fortran.
     * FIXME: Check if 0-indexed Fortran array causes issues and why it needs to be 0-indexed.
     *
     * HACK: TotalView doesn't correctly resolve the type transformation through the local_index_t
     * type alias, so make the type explicitly std::uint32_t here.
     */
    MeshView<LOCAL_INDEX_T_ALIAS *> all_num_local_cells_;

    /**
     * @brief Global index MINUS 1 of the first cell on each PE
     *
     * PE k owns the cells with global addresses global_base_address_(k) + 1 to
     * global_base_address_(k) + all_num_local_cells_(k). This is a 0-indexed array in Frotran.
     * FIXME: Do we really need to subtract 1 from the first global address?
     *
     * HACK: TotalView doesn't correctly resolve the type transformation through the global_index_t
     * type alias, so make the type explicitly std::uint64_t here.
     */
    MeshView<GLOBAL_INDEX_T_ALIAS *> global_base_address_;

    MeshView<FortranGlobalIndex *> cell_address_; //!< Global address of each local cell
    MeshView<bool *> cell_active_; //!< Flag for each local cell indicating whether it is active

    /**
     * @brief Physical coordinates of the cell center of each local cell
     *
     * The cell center coordinates are independent of the geometry. For the geometry-dependent cell
     * centroid, see cell_position_; The first index is the local cell index and the second is the
     * dimension.
     */
    MeshView<double **> cell_center_;

    /**
     * @brief Physical coordinates of the geometric centroid of each local cell
     *
     * The cell centroid is the center of mass of the cell (assuming a uniform density) and it
     * depends on the simulation geometry. For the geometry-independent cell center, see
     * cell_center_. The first index is the local cell index and the second is the dimension.
     */
    MeshView<double **> cell_position_;

    /**
     * @brief Vector containing half the size of the cell in each dimension of each local cell
     *
     * This quantity is indepdendent of the geometry. The first index is the local cell index and
     * the second is the dimension.
     */
    MeshView<double **> cell_half_;

    /**
     * @brief Distance from the lower/higher cell face to the centroid of the cell in each dimension
     * of each local cell
     *
     * Only the x-component of these vectors differ from cell_half_ and they depend on the geometry.
     * The first index is the local cell index and the second is the dimension.
     */
    MeshView<double **> cell_half_lo_, cell_half_hi_;

    /**
     * @brief The volume of each local cell excluding some geometric factors
     *
     * This quantity is geometry-dependent. It is missing a factor of \f$\pi$ in cylindrical
     * geometry and a factor of \f$4\pi/3$ in spherical geometry.
     */
    MeshView<double *> cell_volume_;

    /**
     * @brief Tracks if global state is consistent
     */
    enum class GlobalStateConsistency : std::uint8_t {
        NeedsUpdateGlobalBase,
        NeedsSetNumLocalCells,
        Consistent
    } global_state_consistent_;

    /**
     * @brief Flag indicating whether the local cell arrays have been allocated
     */
    bool cell_arrays_allocated_;
};

} // namespace mesh
} // namespace eap

#endif // EAP_MESH_CELLS_HPP_