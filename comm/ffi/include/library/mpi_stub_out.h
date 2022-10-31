/**
 * @file mpi_stub_out.h
 *
 * @brief EAP requires a stub to swap out MPI
 * @date 2019-01-15
 *
 * @copyright Copyright (C) 2019 Triad National Security, LLC
 */

#ifdef EAP_COMM_USE_STUB
#include "mpi_stub.h"
#else
#include <mpi.h>
#endif