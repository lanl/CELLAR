#include <comm-token.hpp>
#include <gtest/gtest.h>

using eap::FortranLocalIndex;
using eap::OptionalFortranGlobalIndex;
using eap::OptionalFortranLocalIndex;
using eap::comm::TokenBuilder;
using eap::utility::NonNegativeInteger;
using Kokkos::View;
using std::vector;

TEST(Timer, CheckTimer) {
    auto world = mpi::Comm::world();
    auto builder = TokenBuilder::FromComm(world);

    builder.SetNumCells(0);

    builder.BuildLocal(nonstd::span<FortranLocalIndex>(),
                       nonstd::span<NonNegativeInteger<int>>(),
                       nonstd::span<OptionalFortranLocalIndex>());

    auto token =
        builder.BuildGlobal(vector<FortranLocalIndex>{}, vector<OptionalFortranGlobalIndex>{});

    token.Get(eap::comm::TokenOperation::Copy, vector<std::int32_t>{});
    token.Get(eap::comm::TokenOperation::Copy, vector<double>{});
    // This one intentionally not a stringified type.
    token.GetV(eap::comm::TokenOperation::Copy,
               View<std::uint16_t **, eap::HostMemorySpace>("GetV timer test", 0, 0));
    token.GetVInv(eap::comm::TokenOperation::Copy,
                  View<std::int64_t **, eap::HostMemorySpace>("GetVInv timer test", 0, 0));
    token.Put(eap::comm::TokenOperation::Copy, vector<float>{});
    token.PutV(eap::comm::TokenOperation::Copy,
               View<char **, eap::HostMemorySpace>("PutV timer test", 0, 0));
    token.PutVInv(eap::comm::TokenOperation::Copy,
                  View<char **, eap::HostMemorySpace>("PutVInv timer test", 0, 0));

    constexpr nonstd::string_view expected_timers[] = {
        nonstd::string_view{"eap::comm::TokenBuilder::BuildGlobal"},
        nonstd::string_view{"eap::comm::TokenBuilder::BuildLocal"},
        nonstd::string_view{"eap::comm::Token::Get<std::int32_t>"},
        nonstd::string_view{"eap::comm::Token::Get<double>"},
        nonstd::string_view{"eap::comm::Token::GetV<T>"},
        nonstd::string_view{"eap::comm::Token::GetVInv<std::int64_t>"},
        nonstd::string_view{"eap::comm::Token::Put<float>"},
        nonstd::string_view{"eap::comm::Token::PutV<char>"},
        nonstd::string_view{"eap::comm::Token::PutVInv<char>"}};

    for (auto const expected : expected_timers) {
        auto const &timer = eap::comm::GetTimerRegistry()->GetTimer(
            eap::comm::GetTimerRegistry()->InsertOrLookupTimer(expected));

        ASSERT_EQ(1, timer.TimerCount());
    }
}