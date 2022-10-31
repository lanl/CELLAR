#include <test_cxx_fortran/CxxClass.hpp>

int main(int, char **) {
    CxxClass c;
    c.print();

    c.some_int() = 42;

    c.resize(3, 5);

    for (int r = 0; r < 3; ++r) {
        for (int col = 0; col < 5; ++col) {
            c.matrix()(r, col) = r * 100.0 + col;
        }
    }

    c.print();

    c.matrix()(1, 2) = -42.42;
    c.print();

    return 0;
}
