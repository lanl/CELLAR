#if defined(TOKEN_USE_MPI_F08)
#define MPI_USE use mpi_f08
#elif defined(TOKEN_USE_MPI)
#define MPI_USE use mpi
#else
#define MPI_USE
#endif

#if defined(TOKEN_USE_MPI_H)
#define MPI_INCLUDE include "mpif.h"
#elif defined(EAP_COMM_USE_STUB)
#define MPI_INCLUDE include "mpif_stub.h"
#else
#define MPI_INCLUDE
#endif