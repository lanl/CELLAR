#include <gtest/gtest.h>
#include <mesh.hpp>

using namespace eap::mesh;

// TEST(TwoDimensional, Basic) {
//     auto world = mpi::Comm::world();
//     auto mesh = Mesh::from_global_2d_dimensions(&world, 10, 10);

//     ASSERT_EQ(10, mesh.extent_0());
//     ASSERT_EQ(10, mesh.extent_1());
//     ASSERT_EQ(0, mesh.extent_2());
// }