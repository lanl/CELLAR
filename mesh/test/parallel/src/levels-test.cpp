/**
 * @file levels.cpp
 *
 * @brief Tests for eap::mesh::Levels
 * @date 2019-03-07
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

// Third Party Includes
#include <gtest/gtest.h>

// Internal Includes
#include <mesh-levels.hpp>

using namespace eap;
using namespace eap::mesh;
using namespace eap::utility::literals;

using eap::comm::TokenBuilder;
using eap::utility::nullint;

// HACK: to work around restriction on lambdas in "private" function bodies
void ImplLevelsBasicTest() {
    constexpr LevelOptions lev_opts{
        3,               // num_dim
        {1.0, 1.0, 1.0}, // cell_size
        KidMomOptions{
            false, // kid_mom_use_s2s
            false, // mom_kid_use_s2s
            false  // mom_kids_use_s2s
        }          // kid_mom
    };

    constexpr local_index_t num_local_cells = 1000;

    auto builder = TokenBuilder::FromComm(mpi::Comm::world());

    Levels levs;
    levs.Initialize(lev_opts);
    // slightly over-allocate levels
    levs.ResizeLocal(num_local_cells, 1100);
    levs.InitializeLevel1();

    Cells cells(builder);
    cells.ResizeCellArrays(num_local_cells, 3);
    cells.SetNumLocalCells(num_local_cells);

    cells.UpdateGlobalBase();

    levs.BuildPack(cells);
    levs.CheckLoHiPackAndLevels(cells);

    levs.MomKidBuild(cells);
    levs.KidMomBuild(cells);
    levs.MomKidsBuild(cells);

    // Test NumAtLevel
    ASSERT_EQ(num_local_cells, levs.NumAtLevel(0));

    {
        Kokkos::View<local_index_t *> cells_at_lev0("cells_at_lev0", num_local_cells);
        levs.CollectAtLevel(cells, 0, cells_at_lev0);

        for (local_index_t i = 0; i < num_local_cells; i++) {
            ASSERT_EQ(i, cells_at_lev0(i));
        }
    }

    {
        // Test CountFilterAtLevel
        auto const num_at_level_over_1 = levs.CountFilterAtLevel(
            0, 0, KOKKOS_LAMBDA(local_index_t const lp) { return lp >= 1; });

        ASSERT_EQ(num_local_cells - 1, num_at_level_over_1);

        // Test FilterAtLevel
        Kokkos::View<local_index_t *> cells_at_level_over_1("cells_at_level_over_1",
                                                            num_at_level_over_1);
        levs.FilterAtLevel(
            0, 0, cells_at_level_over_1, KOKKOS_LAMBDA(local_index_t const lp) { return lp >= 1; });

        for (local_index_t i = 0; i < num_local_cells - 1; i++) {
            ASSERT_EQ(i + 1, cells_at_level_over_1(i));
        }
    }

    // Test MapCellsAtLevel
    auto const base = cells.global_base_address()[mpi::Comm::world().rank()];
    Kokkos::View<Kokkos::pair<local_index_t, global_index_t> *> global_cells("global_cells",
                                                                             num_local_cells);
    levs.MapCellsAtLevel(
        0, global_cells, KOKKOS_LAMBDA(local_index_t const lp) { return base + lp; });

    for (local_index_t i = 0; i < num_local_cells; i++) {
        ASSERT_EQ(base + i, global_cells(i).second);
    }

    // Test SetFlags functions
    levs.SetFlags(
        0, num_local_cells, KOKKOS_LAMBDA(local_index_t const i) { return i % 4; });

    for (local_index_t l = 0; l < num_local_cells; l++) {
        ASSERT_EQ(l % 4, levs.flag(l));
    }

    levs.SetFlagsScalar(0, num_local_cells, 7);

    for (local_index_t l = 0; l < num_local_cells; l++) {
        ASSERT_EQ(7, levs.flag(l));
    }

    levs.SetFlagsAll(8);

    for (local_index_t l = 0; l < levs.flag.extent(0); l++) {
        ASSERT_EQ(8, levs.flag(l));
    }

    levs.SetFlagsScalarWhen(
        0, num_local_cells, 13, KOKKOS_LAMBDA(local_index_t const i) {
            return i % 4 == 0 ? 1 : 0;
        });

    for (local_index_t l = 0; l < num_local_cells; l++) {
        if (l % 4 == 0) {
            ASSERT_EQ(13, levs.flag(l));
            ASSERT_EQ(1, levs.flag_tag(l));
        } else {
            ASSERT_EQ(8, levs.flag(l));
        }
    }

    // reset all to 8
    levs.SetFlagsAll(8);

    {
        Kokkos::View<FortranLocalIndex *, eap::HostMemorySpace> cell_list("cell_list", 10);
        for (local_index_t l = 0; l < 10; l++) {
            cell_list(l) = l * 100;
        }

        levs.SetFlagsScalarFromList(cell_list, 13);

        for (local_index_t l = 0; l < num_local_cells; l++) {
            if (l % 100 == 0) {
                ASSERT_EQ(13, levs.flag(l));
            } else {
                ASSERT_EQ(8, levs.flag(l));
            }
        }
    }

    // reset all to 8
    levs.SetFlagsAll(8);

    {
        Kokkos::View<FortranLocalIndex *, eap::HostMemorySpace> cell_list("cell_list", 10);
        Kokkos::View<int32_t *, eap::HostMemorySpace> tags("tags", num_local_cells);
        for (local_index_t l = 0; l < 10; l++) {
            cell_list(l) = l * 100;
            tags(l * 100) = l;
        }

        levs.SetFlagsScalarAndTagsFromList(cell_list, 13, tags);

        for (local_index_t l = 0; l < num_local_cells; l++) {
            if (l % 100 == 0) {
                ASSERT_EQ(13, levs.flag(l));
                ASSERT_EQ(l / 100, levs.flag_tag(l));
            } else {
                ASSERT_EQ(8, levs.flag(l));
            }
        }
    }

    // reset all to 8
    levs.SetFlagsAll(8);

    {
        Kokkos::View<Kokkos::pair<FortranLocalIndex, int32_t> *, eap::HostMemorySpace> tups("tups",
                                                                                            10);
        for (local_index_t i = 0; i < 10; i++) {
            tups(i).first = i * 100;
            tups(i).second = i;
        }

        levs.SetFlagsFromTuples(tups);

        for (local_index_t l = 0; l < num_local_cells; l++) {
            if (l % 100 == 0) {
                ASSERT_EQ(l / 100, levs.flag(l));
            } else {
                ASSERT_EQ(8, levs.flag(l));
            }
        }
    }

    // reset all to 8
    levs.SetFlagsAll(8);

    {
        Kokkos::View<Kokkos::pair<FortranLocalIndex, int32_t> *, eap::HostMemorySpace> tups("tups",
                                                                                            10);
        for (local_index_t i = 0; i < 10; i++) {
            tups(i).first = i * 100;
            tups(i).second = i * 2;
        }

        levs.SetFlagTagsFromTuples(tups);

        for (local_index_t l = 0; l < num_local_cells; l++) {
            if (l % 100 == 0) {
                ASSERT_EQ(l / 50, levs.flag_tag(l));
            }
        }
    }

    // reset all to 8
    levs.SetFlagsAll(8);

    {
        Kokkos::View<Kokkos::pair<FortranLocalIndex, int32_t> *, eap::HostMemorySpace> tups("tups",
                                                                                            10);
        for (local_index_t i = 0; i < 10; i++) {
            tups(i).first = i * 100;
            // tups(i).second = i * 2;
        }

        levs.SetFlagsFromTuplesKeyAndScalar(tups, 13);

        for (local_index_t l = 0; l < num_local_cells; l++) {
            if (l % 100 == 0) {
                ASSERT_EQ(13, levs.flag(l));
            } else {
                ASSERT_EQ(8, levs.flag(l));
            }
        }
    }

    // reset all to 8
    levs.SetFlagsAll(8);

    {
        Kokkos::View<Kokkos::pair<FortranLocalIndex, int32_t> *, eap::HostMemorySpace> tups("tups",
                                                                                            10);
        for (local_index_t i = 0; i < 10; i++) {
            tups(i).first = i * 100;
            // tups(i).second = i * 2;
        }

        levs.SetFlagTagsFromTuplesKeyAndScalar(tups, 13);

        for (local_index_t l = 0; l < num_local_cells; l++) {
            if (l % 100 == 0) {
                ASSERT_EQ(13, levs.flag_tag(l));
            }
        }
    }

    {
        Kokkos::View<bool *, eap::HostMemorySpace> flags("flags", num_local_cells);
        for (local_index_t i = 0; i < num_local_cells; i++) {
            flags(i) = i % 2 == 0;
        }

        levs.ClearAtLevel(0, flags);

        for (local_index_t i = 0; i < num_local_cells; i++) {
            if (i % 2 == 0) {
                ASSERT_EQ(0_fiu, levs.cell_level.view_host()(i));
            } else {
                ASSERT_EQ(nullint, levs.cell_level.view_host()(i));
            }
        }
    }
}

TEST(Levels, Basic) { ImplLevelsBasicTest(); }