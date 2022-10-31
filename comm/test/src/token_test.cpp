#include <array>
#include <numeric>

#include <comm-token.hpp>
#include <gtest/gtest.h>
#include <mpi/op.hpp>

using eap::comm::TokenBuilder;
using eap::comm::TokenOperation;
using Kokkos::View;
using mpi::logical_or;
using mpi::rank_t;
using std::array;
using std::size_t;
using std::uint32_t;
using std::vector;

using namespace eap;
using namespace eap::utility::literals;

template <typename LeftView, typename RightView>
bool views_are_similar(LeftView const &lhs,
                       RightView const &rhs,
                       typename LeftView::value_type max_delta = 0) {
    if (lhs.extent(0) != rhs.extent(0)) return false;
    if (lhs.extent(1) != rhs.extent(1)) return false;

    for (size_t i = 0; i < lhs.extent(0); i++) {
        for (size_t j = 0; j < lhs.extent(1); j++) {
            if (std::abs(lhs(i, j) - rhs(i, j)) >= max_delta) return false;
        }
    }

    return true;
}

TEST(BuildGlobalBase, ComputeBases) {
    using namespace eap::comm::internal;

    auto world = mpi::Comm::world();

    std::vector<uint32_t> numcells;
    std::vector<uint64_t> bases;
    BuildGlobalBase(world, world.rank() + 1, numcells, bases);

    EXPECT_EQ(world.size(), bases.size());
    for (auto i = 0; i < world.size(); i++) {
        EXPECT_EQ(i * (i + 1) / 2, bases[i]);
    }
}

TEST(PeAndLocalAddress, Basic) {
    using namespace eap::comm::internal;

    vector<uint64_t> const bases{0, 7, 16, 22};
    vector<uint64_t> const queries{0, 3, 7, 15, 16, 17, 22, 80};
    vector<PeCellAddress> const answers{
        {0, 0}, {0, 3}, {1, 0}, {1, 8}, {2, 0}, {2, 1}, {3, 0}, {3, 58}};

    EXPECT_EQ(queries.size(), answers.size());
    for (size_t i = 0; i < queries.size(); i++) {
        auto const &answer = answers[i];
        auto const test = PeAndLocalAddress(bases, queries[i]);
        EXPECT_EQ(answer.rank, test.rank) << "at test " << i;
        EXPECT_EQ(answer.local_address, test.local_address) << "at test " << i;
    }
}

TEST(BuildCopyInfo, Basic) {
    using namespace eap::comm::internal;

    vector<FortranLocalIndex> const home_address{0, 1, 2, 3, 4, 5, 6, 7};

    vector<utility::NonNegativeInteger<int>> const away_pe{0, 0, 1, 1, 2, 2, 3, 3};
    vector<OptionalFortranLocalIndex> const away_address{
        6_fiu, 9_fiu, 2_fiu, 8_fiu, 3_fiu, 7_fiu, 4_fiu, 9_fiu};

    auto const copy_info = BuildCopyInfo(2, home_address, away_pe, away_address);

    CopyFromTo const expected{{3, 7}, {4, 5}, {}};

    EXPECT_EQ(expected.copy_from.size(), copy_info.copy_from.size());
    EXPECT_EQ(expected.copy_to.size(), copy_info.copy_to.size());
    EXPECT_EQ(copy_info.copy_from.size(), copy_info.copy_to.size());
    for (size_t i = 0; i < expected.copy_from.size(); i++) {
        EXPECT_EQ(expected.copy_from[i], copy_info.copy_from[i]) << "at test " << i;
        EXPECT_EQ(expected.copy_to[i], copy_info.copy_to[i]) << "at test " << i;
    }
}

TEST(BuildCopyInfo, Reverse) {
    using namespace eap::comm::internal;

    vector<FortranLocalIndex> const home_address{7, 6, 5, 4, 3, 2, 1, 0};

    vector<utility::NonNegativeInteger<int>> const away_pe{0, 0, 1, 1, 2, 2, 3, 3};
    vector<OptionalFortranLocalIndex> const away_address{
        6_fiu, 9_fiu, 2_fiu, 8_fiu, 3_fiu, 7_fiu, 4_fiu, 9_fiu};

    auto const copy_info = BuildCopyInfo(2, home_address, away_pe, away_address);

    CopyFromTo const expected{{3, 7}, {3, 2}, {}};

    EXPECT_EQ(expected.copy_from.size(), copy_info.copy_from.size());
    EXPECT_EQ(expected.copy_to.size(), copy_info.copy_to.size());
    EXPECT_EQ(copy_info.copy_from.size(), copy_info.copy_to.size());
    for (size_t i = 0; i < expected.copy_from.size(); i++) {
        EXPECT_EQ(expected.copy_from[i], copy_info.copy_from[i]) << "at test " << i;
        EXPECT_EQ(expected.copy_to[i], copy_info.copy_to[i]) << "at test " << i;
    }
}

template <typename FP, typename Test>
void get_put_test(Test *t) {
    //
    // This test builds a virtual grid that looks like this:
    //
    //   0: | 1.1| 2.1| 3.1| 4.1| 5.1| 6.1| 7.1| 8.1| 9.1|10.1|
    //   1: | 1.2| 2.2| 3.2| 4.2| 5.2| 6.2| 7.2| 8.2| 9.2|10.2|11.2|
    //   2: | 1.3| 2.3| 3.3| 4.3| 5.3| 6.3| 7.3| 8.3| 9.3|10.3|11.3|12.3|
    //   3: | 1.4| 2.4| 3.4| 4.4| 5.4| 6.4| 7.4| 8.4| 9.4|10.4|11.4|12.4|13.4|
    //
    // Notice that each row in the grid is a different size.
    //
    // Each rank requires the global column corresponding to comm.rank()*2, if it exists.
    //
    // This test builds a token object corresponding to this requirement,
    // retrieves the data from the remote ranks, validates it receives the
    // correct data, doubles it, and returns it to the remote ranks.
    //
    // The final values in the grid should be:
    //
    //   0: | 2.2| 2.1| 6.2| 4.1|10.2| 6.1|14.2| 8.1| 9.1|10.1|
    //   1: | 2.4| 2.2| 6.4| 4.2|10.4| 6.2|14.4| 8.2| 9.2|10.2|11.2|
    //   2: | 2.6| 2.3| 6.6| 4.3|10.6| 6.3|14.6| 8.3| 9.3|10.3|11.3|12.3|
    //   3: | 2.8| 2.4| 6.8| 4.4|10.8| 6.4|14.8| 8.4| 9.4|10.4|11.4|12.4|13.4|
    //

    auto world = mpi::Comm::world();

    // Test for all comm sizes from 1 to max
    for (rank_t last = 0;
         last < world.size() && !world.all_reduce(logical_or(), t->HasFatalFailure());
         last++) {
        auto comm = world.create(world.group().range_incl(0, last));
        if (!comm) continue;

        size_t const num_cells = 10 + comm.rank();

        vector<OptionalFortranGlobalIndex> global_needed;
        for (rank_t rank = 0; rank < comm.size(); rank++) {
            if (comm.rank() * 2 < 10 + rank) {
                global_needed.push_back(OptionalFortranGlobalIndex(
                    10 * rank + (rank * (rank - 1) / 2) + comm.rank() * 2));
            }
        }

        vector<FortranLocalIndex> home_mapping(global_needed.size());
        std::iota(home_mapping.begin(), home_mapping.end(), 0);

        vector<FP> my_data;
        for (size_t i = 0; i < num_cells; i++) {
            my_data.push_back(i + 1 + (comm.rank() + 1) * 0.1);
        }

        vector<FP> get_ans;
        for (rank_t rank = 0; rank < (rank_t)global_needed.size(); rank++) {
            if (comm.rank() * 2 < 10 + rank) {
                get_ans.push_back(comm.rank() * 2 + 1 + (rank + 1) * 0.1);
            }
        }

        vector<FP> put_ans;
        for (size_t i = 0; i < num_cells; i++) {
            auto multiplier = 1;
            if (i % 2 == 0 && i / 2 < (size_t)comm.size()) {
                multiplier = 2;
            }
            put_ans.push_back((i + 1 + (comm.rank() + 1) * 0.1) * multiplier);
        }

        auto builder = eap::comm::TokenBuilder::FromComm(comm.deref());
        builder.SetNumCells(num_cells);

        std::vector<int> to_pes(comm.size(), 0);
        for (rank_t rank = 0; rank < comm.size(); rank++) {
            if (comm.rank() * 2 < 10 + rank) {
                to_pes[rank] = 1;
            }
        }

        builder.SetToPes(std::move(to_pes));

        auto token = builder.BuildGlobal(home_mapping, global_needed);

        auto recv_data = token.Get(TokenOperation::Copy, my_data);

        EXPECT_EQ(get_ans, recv_data);

        for (auto &data : recv_data) {
            data *= 2;
        }

        token.Put(TokenOperation::Copy, recv_data, my_data);

        EXPECT_EQ(put_ans, my_data);
    }
}

TEST(Token, GetPutFloat) { get_put_test<float>(this); }
TEST(Token, GetPutDouble) { get_put_test<double>(this); }

void get_put_v_double_test(mpi::Comm comm, TokenBuilder &builder) {
    vector<OptionalFortranGlobalIndex> global_needed;
    for (rank_t i = 0; i < comm.size(); i++) {
        global_needed.push_back(OptionalFortranGlobalIndex(comm.size() * i + comm.rank()));
    }

    vector<FortranLocalIndex> home_mapping(global_needed.size());
    std::iota(home_mapping.begin(), home_mapping.end(), 0);

    View<double **, eap::HostMemorySpace> get_ans("get_ans", comm.size(), comm.size());
    for (rank_t i = 0; i < comm.size(); i++) {
        for (rank_t j = 0; j < comm.size(); j++) {
            get_ans(i, j) = j * comm.size() + comm.rank() + 1 + (i + 1) * 0.1;
        }
    }

    View<double **, eap::HostMemorySpace> put_ans("put_ans", comm.size(), comm.size());
    for (rank_t i = 0; i < comm.size(); i++) {
        for (rank_t j = 0; j < comm.size(); j++) {
            put_ans(i, j) = j * comm.size() + i + 1 + ((comm.rank() + 1) * 0.1);
        }
    }

    for (size_t i = 0; i < put_ans.extent(0); i++) {
        for (size_t j = 0; j < put_ans.extent(1); j++) {
            put_ans(i, j) *= (i + 1);
        }
    }

    auto token = builder.BuildGlobal(home_mapping, global_needed);

    View<double **, eap::HostMemorySpace> my_data("my_data", comm.size(), comm.size());
    for (rank_t i = 0; i < comm.size(); i++) {
        for (rank_t j = 0; j < comm.size(); j++) {
            my_data(i, j) = j * comm.size() + i + 1 + ((comm.rank() + 1) * 0.1);
        }
    }

    auto recv_data = token.GetV(TokenOperation::Copy, my_data);

    EXPECT_TRUE(views_are_similar(get_ans, recv_data, 0.01));

    for (size_t i = 0; i < recv_data.extent(0); i++) {
        for (size_t j = 0; j < recv_data.extent(1); j++) {
            recv_data(i, j) *= comm.rank() + 1;
        }
    }

    token.PutV(TokenOperation::Copy, recv_data, my_data);

    EXPECT_TRUE(views_are_similar(put_ans, my_data, 0.01));
}

TEST(Token, GetPutVDouble) {
    //
    // This test builds, for each rank, a numpe-by-numpe matrix with the
    // following data:
    //
    //   0: | 1.1| 5.1| 9.1|13.1|
    //      | 2.1| 6.1|10.1|14.1|
    //      | 3.1| 7.1|11.1|15.1|
    //      | 4.1| 8.1|12.1|16.1|
    //
    //   1: | 1.2| 5.2| 9.2|13.2|
    //      | 2.2| 6.2|10.2|14.2|
    //      | 3.2| 7.2|11.2|15.2|
    //      | 4.2| 8.2|12.2|16.2|
    //
    //   2: | 1.3| 5.3| 9.3|13.3|
    //      | 2.3| 6.3|10.3|14.3|
    //      | 3.3| 7.3|11.3|15.3|
    //      | 4.3| 8.3|12.3|16.3|
    //
    //   3: | 1.4| 5.4| 9.4|13.4|
    //      | 2.4| 6.4|10.4|14.4|
    //      | 3.4| 7.4|11.4|15.4|
    //      | 4.4| 8.4|12.4|16.4|
    //
    // Each rank accesses the mype+1 row from each other rank corresponding to
    // mype.
    //
    // This test builds a token object corresponding to this requirement,
    // retrieves the data from the remote ranks, validates it receives the
    // correct data, multiplies by (mype+1), and returns it to the remote ranks.
    //
    // The received data in the case of rank 0 is:
    //
    //   0: | 1.1| 5.1| 9.1|13.1|
    //      | 1.2| 5.2| 9.2|13.2|
    //      | 1.3| 5.3| 9.3|13.3|
    //      | 1.4| 5.4| 9.4|13.4|
    //
    // The final values in the grid should be:
    //
    //   0: | 1.1| 5.1| 9.1|13.1|
    //      | 4.2|12.2|20.2|28.2|
    //      | 9.3|21.3|33.3|45.3|
    //      |16.4|32.4|48.4|64.4|
    //
    //   1: | 1.2| 5.2| 9.2|13.2|
    //      | 4.4|12.4|20.4|28.4|
    //      | 9.6|21.6|33.6|45.6|
    //      |16.8|32.8|48.8|64.8|
    //
    //   2: | 1.3| 5.3| 9.3|13.3|
    //      | 4.6|12.6|20.6|28.6|
    //      | 9.9|21.9|33.9|45.9|
    //      |17.2|33.2|49.2|65.2|
    //
    //   3: | 1.4| 5.4| 9.4|13.4|
    //      | 2.8| 6.8|10.8|14.8|
    //      |10.2|22.2|34.2|46.2|
    //      |17.6|33.6|49.6|65.6|
    //

    auto world = mpi::Comm::world();

    // Test for all comm sizes from 1 to max
    for (rank_t last = 0; last < world.size() && !world.all_reduce(logical_or(), HasFatalFailure());
         last++) {
        auto comm = world.create(world.group().range_incl(0, last));
        if (!comm) continue;

        auto builder = TokenBuilder::FromComm(comm.deref());
        builder.SetNumCells(comm.size());

        auto rma = eap::comm::RmaAllToAll<int>(comm.deref());

        std::array<size_t, 5> const max_recv_sizes{0,
                                                   1,
                                                   comm.size() * 2 * sizeof(double),
                                                   comm.size() * (comm.size() - 1) * sizeof(double),
                                                   1000000};

        for (auto &max_size : max_recv_sizes) {
            if (max_size == 0) {
                builder.ClearMaxGsReceiveSize();
            } else {
                builder.SetMaxGsReceiveSize(max_size);
            }

            for (auto use_rma : std::array<bool, 2>{true, false}) {
                if (use_rma) {
                    builder.UseRmaAllToAll(&rma);
                } else {
                    builder.DisableRmaAllToAll();
                }

                for (auto require_rank_order : std::array<bool, 2>{true, false}) {
                    builder.RequireRankOrderRequestCompletion(require_rank_order);

                    for (auto use_some_to_some : std::array<bool, 2>{true, false}) {
                        if (use_some_to_some) {
                            std::vector<int> to_pes(comm.size(), 1);
                            builder.SetToPes(std::move(to_pes));
                        } else {
                            builder.ClearToAndFromPes();
                        }

                        get_put_v_double_test(comm.deref(), builder);
                    }
                }
            }
        }
    }
}

TEST(Token, GetPutVInvDouble) {
    //
    // This test builds, for each rank, a numpe-by-numpe matrix with the
    // following data:
    //
    //   0: | 1.1| 5.1| 9.1|13.1|
    //      | 2.1| 6.1|10.1|14.1|
    //      | 3.1| 7.1|11.1|15.1|
    //      | 4.1| 8.1|12.1|16.1|
    //
    //   1: | 1.2| 5.2| 9.2|13.2|
    //      | 2.2| 6.2|10.2|14.2|
    //      | 3.2| 7.2|11.2|15.2|
    //      | 4.2| 8.2|12.2|16.2|
    //
    //   2: | 1.3| 5.3| 9.3|13.3|
    //      | 2.3| 6.3|10.3|14.3|
    //      | 3.3| 7.3|11.3|15.3|
    //      | 4.3| 8.3|12.3|16.3|
    //
    //   3: | 1.4| 5.4| 9.4|13.4|
    //      | 2.4| 6.4|10.4|14.4|
    //      | 3.4| 7.4|11.4|15.4|
    //      | 4.4| 8.4|12.4|16.4|
    //
    // Each rank accesses the mype+1 column from each other rank corresponding to
    // mype.
    //
    // This test builds a token object corresponding to this requirement,
    // retrieves the data from the remote ranks, validates it receives the
    // correct data, multiplies by (mype+1), and returns it to the remote ranks.
    //
    // The received data in the case of rank 0 is:
    //
    //   0: | 1.1| 1.2| 1.3| 1.4|
    //      | 2.1| 2.2| 2.3| 2.4|
    //      | 3.1| 3.2| 3.3| 3.4|
    //      | 4.1| 4.2| 4.3| 4.4|
    //
    // The final values in the grid should be:
    //
    //   0: | 1.1|10.2|27.3|52.4|
    //      | 2.1|12.2|30.3|56.4|
    //      | 3.1|14.2|33.3|60.4|
    //      | 4.1|16.2|36.3|64.4|
    //
    //   1: | 1.2|10.4|27.6|52.8|
    //      | 2.2|12.4|30.6|56.8|
    //      | 3.2|14.4|33.6|60.8|
    //      | 4.2|16.4|36.6|64.8|
    //
    //   2: | 1.3|10.6|27.9|53.2|
    //      | 2.3|12.6|30.9|57.2|
    //      | 3.3|14.6|33.9|61.2|
    //      | 4.3|16.6|36.9|65.2|
    //
    //   3: | 1.4|10.8|28.2|53.6|
    //      | 2.4|12.8|31.2|57.6|
    //      | 3.4|14.8|34.2|61.6|
    //      | 4.4|16.8|37.2|65.6|
    //

    auto world = mpi::Comm::world();

    // Test for all comm sizes from 1 to max
    for (rank_t last = 0; last < world.size() && !world.all_reduce(logical_or(), HasFatalFailure());
         last++) {
        auto comm = world.create(world.group().range_incl(0, last));
        if (!comm) continue;

        vector<OptionalFortranGlobalIndex> global_needed;
        for (rank_t i = 0; i < comm.size(); i++) {
            global_needed.push_back(OptionalFortranGlobalIndex(comm.size() * i + comm.rank()));
        }

        vector<FortranLocalIndex> home_mapping(global_needed.size());
        std::iota(home_mapping.begin(), home_mapping.end(), 0);

        View<double **, eap::HostMemorySpace> my_data("my_data", comm.size(), comm.size());
        for (rank_t i = 0; i < comm.size(); i++) {
            for (rank_t j = 0; j < comm.size(); j++) {
                my_data(i, j) = j * comm.size() + i + 1 + ((comm.rank() + 1) * 0.1);
            }
        }

        View<double **, eap::HostMemorySpace> get_ans("get_ans", comm.size(), comm.size());
        for (rank_t i = 0; i < comm.size(); i++) {
            for (rank_t j = 0; j < comm.size(); j++) {
                get_ans(i, j) = comm.rank() * comm.size() + i + 1 + (j + 1) * 0.1;
            }
        }

        auto builder = TokenBuilder::FromComm(comm.deref());
        builder.SetNumCells(comm.size());
        auto token = builder.BuildGlobal(home_mapping, global_needed);

        auto recv_data = token.GetVInv(TokenOperation::Copy, my_data);

        EXPECT_TRUE(views_are_similar(get_ans, recv_data, 0.01));

        for (size_t i = 0; i < recv_data.extent(0); i++) {
            for (size_t j = 0; j < recv_data.extent(1); j++) {
                recv_data(i, j) *= comm.rank() + 1;
            }
        }

        View<double **, eap::HostMemorySpace> put_ans("put_ans", comm.size(), comm.size());
        Kokkos::deep_copy(put_ans, my_data);
        for (size_t i = 0; i < put_ans.extent(0); i++) {
            for (size_t j = 0; j < put_ans.extent(1); j++) {
                put_ans(i, j) *= (j + 1);
            }
        }

        token.PutVInv(TokenOperation::Copy, recv_data, my_data);

        EXPECT_TRUE(views_are_similar(put_ans, my_data, 0.01));
    }
}
