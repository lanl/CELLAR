
// STL Includes
#include <cstdint>

// Third Party Includes
#include <Kokkos_Core.hpp>

int main(int argc, char **argv) {
    Kokkos::initialize(argc, argv);

    {
        Kokkos::View<double *> v("name", 100);
        for (size_t i = 0; i < 100; i++) {
            v(i) = double(i) + 0.1;
        }

        Kokkos::View<std::int32_t **, Kokkos::LayoutLeft> v2d("v2d", 32, 3);
        for (auto j = 0; j < 3; j++) {
            for (auto i = 0; i < 32; i++) {
                v2d(i, j) = i * 3 + j;
            }
        }
    }

    Kokkos::finalize();
    return 0;
}