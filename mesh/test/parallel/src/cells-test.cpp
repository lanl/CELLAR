/**
 * @file cells.cpp
 *
 * @brief Tests for eap::mesh::cells
 * @date 2019-05-10
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Third Party Includes
#include <gtest/gtest.h>

// Internal Includes
#include <comm-token.hpp>
#include <mesh-cells.hpp>
#include <utility-linear_view.hpp>

using eap::global_index_t;
using eap::local_index_t;
using eap::comm::TokenBuilder;
using eap::mesh::Cells;
using eap::utility::MakeLinearView;
using mpi::rank_t;

TEST(Cells, Basic) {
    auto comm = mpi::Comm::world();
    auto builder = TokenBuilder::FromComm(comm);
    Cells cells(builder);

    local_index_t const num_local_cells = 10 + comm.rank();

    cells.SetNumLocalCells(num_local_cells);
    cells.ResizeCellArrays(num_local_cells, 2);

    {
        auto const &all_num_local_cells = cells.all_num_local_cells();

        auto all_num_local_cells_unit = MakeLinearView(all_num_local_cells);

        std::vector<global_index_t> expected(comm.size(), 0);
        ASSERT_EQ(nonstd::span<global_index_t const>(expected), all_num_local_cells_unit.Span());
    }

    cells.UpdateGlobalBase();

    {
        auto const &all_num_local_cells = cells.all_num_local_cells();

        auto all_num_local_cells_unit = MakeLinearView(all_num_local_cells);

        std::vector<global_index_t> expected_num_local_cells(comm.size(), 0);
        for (rank_t i = 0; i < comm.size(); i++) {
            expected_num_local_cells[i] = 10 + i;
        }

        ASSERT_EQ(nonstd::span<global_index_t const>(expected_num_local_cells),
                  all_num_local_cells_unit.Span());
    }

    {
        auto const &global_base_address = cells.global_base_address();

        auto global_base_address_unit = MakeLinearView(global_base_address);

        std::vector<global_index_t> expected_bases(comm.size() + 1, 0);
        for (rank_t i = 1; i < comm.size() + 1; i++) {
            expected_bases[i] = expected_bases[i - 1] + 10 + (i - 1);
        }

        ASSERT_EQ(nonstd::span<global_index_t const>(expected_bases),
                  global_base_address_unit.Span());
    }

    // Sanity test for CheckGlobalBase
    cells.CheckGlobalBase();
}