/**
 * @brief blocks.hpp defines data types and routines for calculating the coordinates for blocks
 * given a linear address space.
 *
 * @file mesh-blocks.hpp
 *
 * @date 2018-08-21
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifndef EAP_MESH_BLOCKS_HPP_
#define EAP_MESH_BLOCKS_HPP_

// STL includes
#include <cstdio> // HACK: We need this include up here before the rest of the includes due to some
                  // issue with XCode 11 not including all the necessary headers from each STL
                  // header

#include <algorithm>
#include <array>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <tuple>

// Internal dependency includes
#include <utility-diagnostic.hpp>

// Local includes
#include "mesh-types.hpp"

namespace eap {
namespace mesh {
// necessary forward declarations
class BlockLayout;

/**
 * @brief Method for laying out blocks in Grid.
 *
 * The integral value for each option matches the values used in the original EAP code to reduce
 * confusion.
 */
enum class BlockLayoutMethod {
    /**
     * @brief Blocks are laid out first in the first-dimension ascending, then second-dimension,
     * then third-dimension.
     */
    ColumnMajor = 1,
    /**
     * @brief UNIMPLEMENTED
     */
    Hilbert = 2,
    /**
     * @brief Blocks are laid out first-dimension ascending in even number rows and first-dimension
     * descending in odd number rows, and in a typical manner for the third-dimension.
     */
    ColumnMajorZigZag = 3,
    /**
     * @brief Blocks are laid out first in the first-dimension ascending, then third-dimension, then
     * second-dimension.
     */
    ColumnMajorReverse = 4,
};

/**
 * @brief Thrown if the BlockLayoutMethod in BlockLayout is unknown.
 */
class UnknownBlockLayoutMethodError : public std::invalid_argument {
  public:
    /**
     * @brief Construct a new UnknownBlockLayoutMethodError object
     *
     * @param method
     *  The unkown block layout method.
     */
    explicit UnknownBlockLayoutMethodError(BlockLayoutMethod method)
        : invalid_argument(internal_what()), method_(method) {}

    /**
     * @brief Returns the unkown block layout method causing the issue.
     *
     * @return BlockLayoutMethod
     *  The unkown block layout method.
     */
    BlockLayoutMethod method() const { return method_; }

  private:
    BlockLayoutMethod method_;

    std::string internal_what() const {
        std::stringstream ss;
        ss << "Unknown block layout method: " << static_cast<int>(method_)
           << ". Please use a supported BlockLayoutMethod.";

        return ss.str();
    }
};

/**
 * @brief Three-tuple of global coordinates for a block object
 */
using BlockCoordinates = Coordinates<global_index_t>;

/**
 * @brief Thrown when invalid extent values are supplied to a function taking a set of extents.
 */
class InvalidBlockExtentsError : public std::range_error {
  public:
    /**
     * @brief Construct a new InvalidBlockExtentsError object
     *
     * @param invalid_extent
     *  The invalid extents value.
     */
    explicit InvalidBlockExtentsError(global_index_t invalid_extent)
        : range_error(internal_what()), invalid_extent_(invalid_extent) {}

    /**
     * @brief Returns the invalid extent value causing the issue.
     *
     * @return global_index_t
     *  The invalid extent value.
     */
    global_index_t invalid_extent() const { return invalid_extent_; }

  private:
    global_index_t invalid_extent_ = 0;

    std::string internal_what() const {
        std::stringstream ss;
        ss << "Received invalid block extent: " << invalid_extent_
           << ". Block extents must be a positive value.";

        return ss.str();
    }
};

/**
 * @brief Thrown when an invalid address is passed to BlockLayout::GetCoordinates
 */
class InvalidBlockAddressError : public std::range_error {
  public:
    /**
     * @brief Construct a new InvalidBlockAddressError object
     *
     * @param invalid_address
     *  The invalid address that caused the exception.
     * @param max_address
     *  The maximum allowable value for addresses in this context.
     */
    InvalidBlockAddressError(global_index_t invalid_address, global_index_t max_address)
        : range_error(internal_what()),
          invalid_address_(invalid_address),
          max_address_(max_address) {}

    /**
     * @brief Returns the invalid address that caused the exception.
     *
     * @return global_index_t
     */
    global_index_t invalid_address() const { return invalid_address_; }

    /**
     * @brief Returns the maximum allowable value for addresses in this context.
     *
     * @return global_index_t
     */
    global_index_t max_address() const { return max_address_; }

  private:
    global_index_t invalid_address_ = 0;
    global_index_t max_address_ = 0;

    std::string internal_what() const {
        std::stringstream ss;
        ss << "Received invalid block address: " << invalid_address_
           << ". Block addresses for this layout must be a valid address in the range [0,"
           << max_address_ << ").";

        return ss.str();
    }
};

/**
 * @brief Thrown when invalid coorindates are passed to BlockLayout::GetAddress.
 */
class InvalidBlockCoordinatesError : public std::range_error {
  public:
    /**
     * @brief Construct a new InvalidBlockCoordinatesError object
     *
     * @param coords
     *  The invalid coordinates
     * @param bounds
     *  The upper bounds for the target BlockLayout
     */
    InvalidBlockCoordinatesError(BlockCoordinates const &coords, BlockCoordinates const &bounds)
        : range_error(internal_what()), coords_(coords), bounds_(bounds) {}

    /**
     * @brief Returns the invalid coordinates that caused the error.
     *
     * @return BlockCoordinates
     */
    BlockCoordinates invalid_coords() const noexcept { return coords_; }

    /**
     * @brief Returns the upper bounds for the target BlockLayout
     *
     * @return BlockCoordinates
     */
    BlockCoordinates bounds() const noexcept { return bounds_; }

  private:
    BlockCoordinates coords_;
    BlockCoordinates bounds_;

    std::string internal_what() const {
        std::stringstream ss;
        ss << "Received invalid block coordinates: "
           << "(" << coords_.x << "," << coords_.y << "," << coords_.z
           << "). The bounds for this block layout are (" << bounds_.x << "," << bounds_.y << ","
           << bounds_.z << ")";

        return ss.str();
    }
};

/**
 * @brief A construct for determining the three-dimensional location for a given block address using
 * a prescribed layout method.
 */
class BlockLayout {
  public:
    /**
     * @brief Construct a new BlockLayout object
     *
     * @param method
     *  The block method to use.
     * @param extent0
     *  The first-dimension extent. Must be >= 1.
     * @param extent1
     *  The second-dimension extent. Must be >= 1.
     * @param extent2
     *  The third-dimension extent. Must be >= 1.
     */
    BlockLayout(BlockLayoutMethod const method,
                global_index_t const extent0,
                global_index_t const extent1 = 1,
                global_index_t const extent2 = 1);

    /**
     * @brief Returns the rank of the underlying array. E.g. the number of extents >1.
     *
     * @return unsigned char
     *  The rank of the target mesh.
     */
    unsigned char rank() const noexcept {
        return (extents_[0] > 1) + (extents_[1] > 1) + (extents_[2] > 1);
    }

    /**
     * @brief Returns the number of blocks that the BlockLayout object addresses.
     *
     * @return global_index_t
     *  The number of blocks that the BlockLayout object addresses.
     */
    global_index_t size() const noexcept { return extents_[0] * extents_[1] * extents_[2]; }

    /**
     * @brief Returns the extents of the block grid.
     *
     * @return std::array<global_index_t, 3>
     *  A list of the extents of the block grid.
     */
    std::array<global_index_t, 3> extents() const noexcept { return extents_; }

    /**
     * @brief Gets the coordinates of some arbitrary block address.
     *
     * Defined inline to improve performance since this function is likely to be called in a
     * hotpath.
     *
     * @param address
     *  The address in the linear block address space. Must be in the range [0,size())
     * @return BlockCoordinates
     *  The coordinates of the given address according to the BlockLayoutMethod. Coorindates will be
     *  in the range of { x: [0,extents()[0]), y: [0,extents()[1]), z: [0, extents()[2]) }
     */
    BlockCoordinates GetCoordinates(global_index_t address) const {
        // Disabling the -Wtype-limits temporarily.
        // We are only retaining these checks in case we decide to change the addressing to signed
        // integers.
        DIAGNOSTIC_PUSH
        DIAGNOSTIC_DISABLE_WARNING_UNSIGNED_COMPARISON_WITH_ZERO
        if (address < 0 || address >= size()) {
            throw InvalidBlockAddressError(address, size());
        }
        DIAGNOSTIC_POP

        switch (method) {
        case BlockLayoutMethod::ColumnMajor:
            return BlockCoordinates{
                address % imax(),
                (address / imax()) % jmax(),
                address / ijmax(),
            };
        case BlockLayoutMethod::ColumnMajorZigZag: {
            auto const i = address % imax();
            auto const j = (address / imax()) % jmax();

            return BlockCoordinates{
                (j % 2) == 1 ? (imax() - i - 1) : i,
                j,
                address / ijmax(),
            };
        }
        case BlockLayoutMethod::ColumnMajorReverse:
            return BlockCoordinates{
                address % imax(),
                address / ikmax(),
                (address / imax()) % kmax(),
            };
        case BlockLayoutMethod::Hilbert:
            throw std::runtime_error("Hilbert blocks are not implemented.");
        }

        throw UnknownBlockLayoutMethodError(method);
    }

    global_index_t GetAddress(BlockCoordinates const &coords) const {
        // Disabling the -Wtype-limits temporarily.
        // We are only retaining these checks in case we decide to change the addressing to signed
        // integers.
        DIAGNOSTIC_PUSH
        DIAGNOSTIC_DISABLE_WARNING_UNSIGNED_COMPARISON_WITH_ZERO
        if (coords.x < 0 || coords.x >= extents_[0] || coords.y < 0 || coords.y >= extents_[1] ||
            coords.z < 0 || coords.z >= extents_[2]) {
            throw InvalidBlockCoordinatesError(
                coords, BlockCoordinates{extents_[0], extents_[1], extents_[2]});
        }
        DIAGNOSTIC_POP

        switch (method) {
        case BlockLayoutMethod::ColumnMajor:
            return coords.z * ijmax() + coords.y * imax() + coords.x;
        case BlockLayoutMethod::ColumnMajorZigZag: {
            auto const i = (coords.y % 2) == 1 ? (imax() - coords.x - 1) : coords.x;

            return coords.z * ijmax() + coords.y * imax() + i;
        }
        case BlockLayoutMethod::ColumnMajorReverse:
            return coords.y * ikmax() + coords.z * imax() + coords.x;
        case BlockLayoutMethod::Hilbert:
            throw std::runtime_error("Hilbert blocks are not implemented.");
        }

        throw UnknownBlockLayoutMethodError(method);
    }

  private:
    BlockLayoutMethod method;
    std::array<global_index_t, 3> extents_;

    global_index_t imax() const noexcept { return extents_[0]; }
    global_index_t jmax() const noexcept { return extents_[1]; }
    global_index_t kmax() const noexcept { return extents_[2]; }

    global_index_t ijmax() const noexcept { return imax() * jmax(); }
    global_index_t ikmax() const noexcept { return imax() * kmax(); }
    global_index_t ijkmax() const noexcept { return imax() * jmax() * kmax(); }
};
} // namespace mesh
} // namespace eap

#endif // EAP_MESH_BLOCKS_HPP_