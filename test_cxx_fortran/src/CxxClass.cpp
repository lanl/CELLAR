#include <test_cxx_fortran/CxxClass.hpp>

CxxClass::CxxClass() : some_int_(0) {
    if (!Kokkos::is_initialized()) Kokkos::initialize();
    printf("CxxClass created\n");
}

CxxClass::~CxxClass() { printf("CxxClass destroyed\n"); }

void CxxClass::resize(int n0, int n1) {
    printf("resizing CxxClass to %i x %i\n", n0, n1);
    Kokkos::resize(matrix_, n0, n1);
}

void CxxClass::print() const {
    printf("Printing CxxClass:\n");
    printf("  some_int_ = %i\n", some_int_);
    printf("  matrix size: %lu x %lu\n", matrix_.extent(0), matrix_.extent(1));
    printf("\n");

    for (size_t row = 0; row < matrix_.extent(0); ++row) {
        printf("  ");

        for (size_t col = 0; col < matrix_.extent(1); ++col)
            printf("%8.2f", matrix_(row, col));

        printf("\n");
    }

    printf("\n\n");
}