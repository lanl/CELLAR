#include <numeric>

#include <gtest/gtest.h>

#include <comm-patterns.hpp>

using mpi::Comm;
using mpi::rank_t;

TEST(Patterns, AllToAll) {
    auto comm = Comm::world().dup();

    auto rma = eap::comm::RmaAllToAll<rank_t>(comm.deref());

    std::vector<rank_t> const my_ranks(comm.size(), comm.rank());

    auto ranks = rma.AllToAll(my_ranks);

    EXPECT_EQ(ranks.size(), (size_t)comm.size());
    for (size_t i = 0; i < ranks.size(); i++) {
        EXPECT_EQ(i, ranks[i]);
    }
}

TEST(Patterns, MoveSimple) {
    using eap::FortranLocalIndex;
    using eap::local_index_t;
    using eap::comm::Move;
    using mpi::rank_t;

    auto comm = Comm::world().dup();

    std::vector<FortranLocalIndex> send_start(comm.size());
    std::vector<local_index_t> send_length(comm.size());

    std::vector<local_index_t> send_data;

    std::vector<FortranLocalIndex> recv_start(comm.size());
    std::vector<local_index_t> recv_length(comm.size());

    recv_start[0] = 0;
    recv_length[0] = comm.rank() + 1;

    std::vector<local_index_t> recv_data(comm.rank() + 1);

    if (comm.rank() == 0) {
        send_data.resize(comm.size() * (comm.size() + 1) / 2);

        std::iota(send_length.begin(), send_length.end(), 1);

        for (rank_t i = 0; i < comm.size(); i++) {
            send_start[i] = i * (i + 1) / 2;

            for (size_t j = 0; j < send_length[i]; j++) {
                send_data[send_start[i] + j] = i;
            }
        }
    }

    Move(comm.deref(),
         span<FortranLocalIndex>(send_start),
         span<local_index_t>(send_length),
         span<local_index_t const>(send_data),
         span<FortranLocalIndex>(recv_start),
         span<local_index_t>(recv_length),
         span<local_index_t>(recv_data));

    std::vector<local_index_t> expected_recv(comm.rank() + 1, comm.rank());
    ASSERT_EQ(expected_recv, recv_data);
}
