#ifndef EAP_TEST_CXX_FORTRAN_CXXCLASS_HPP
#define EAP_TEST_CXX_FORTRAN_CXXCLASS_HPP

#include <utility-kokkos.hpp>

template <typename T>
using Array = Kokkos::View<T, Kokkos::LayoutLeft>;

class CxxClass {
  public:
    CxxClass();

    ~CxxClass();

    void resize(int n0, int n1);

    void print() const;

    int &some_int() { return some_int_; }
    const int &some_int() const { return some_int_; }

    Array<double **> &matrix() { return matrix_; }
    const Array<double **> &matrix() const { return matrix_; }

    Array<double ***> &view3d() { return view3d_; }
    const Array<double ***> &view3d() const { return view3d_; }

  private:
    int some_int_;

    Array<double **> matrix_;
    Array<double ***> view3d_;
};

#endif // EAP_TEST_CXX_FORTRAN_CXXCLASS_HPP