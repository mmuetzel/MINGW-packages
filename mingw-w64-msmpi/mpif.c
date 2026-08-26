// mpif.c – minimal gfortran-compatible Fortran bindings for MS-MPI
//
// GFortran does not support DLLIMPORT on COMMON block variable. And even if it
// did, the COMMON block variables that are exported from msmpi.dll have Intel
// Fortran layout (which would be incompatible with GFortran anyway).
//
// To still support using the sentinel variables that msmpi.dll exports in Intel
// Fortran COMMON blocks, replace them in a wrapper using sentinel variables
// that are dllimported from a small helper DLL. These wrappers need to be
// linked with all (Fortran) projects linking to msmpi.dll in MSYS2. Ensure that
// by including them in the import library.

#include <windows.h>
#include <mpi.h>

// ---------------------------------------------------------------------
// Global variables (Fortran module + mpif.h)
// ---------------------------------------------------------------------

// global variables from Fortran module
#if defined(__clang__)
// LLVM Flang name mangling
__attribute__ ((dllimport)) int _QMmpi_constantsEmpi_bottom;
__attribute__ ((dllimport)) int _QMmpi_constantsEmpi_in_place;
__attribute__ ((dllimport)) int _QMmpi_constantsEmpi_status_ignore[5];
__attribute__ ((dllimport)) int _QMmpi_constantsEmpi_statuses_ignore[5][1];
__attribute__ ((dllimport)) int _QMmpi_constantsEmpi_errcodes_ignore[1];
__attribute__ ((dllimport)) int _QMmpi_constantsEmpi_unweighted;
__attribute__ ((dllimport)) int _QMmpi_constantsEmpi_weights_empty;
__attribute__ ((dllimport)) int _QMmpi_constantsEmpi_argvs_null;
__attribute__ ((dllimport)) int _QMmpi_constantsEmpi_argv_null;

#define mod_mpi_bottom _QMmpi_constantsEmpi_bottom
#define mod_mpi_in_place _QMmpi_constantsEmpi_in_place
#define mod_mpi_status_ignore _QMmpi_constantsEmpi_status_ignore
#define mod_mpi_statuses_ignore _QMmpi_constantsEmpi_statuses_ignore
#define mod_mpi_errcodes_ignore _QMmpi_constantsEmpi_errcodes_ignore
#define mod_mpi_unweighted _QMmpi_constantsEmpi_unweighted
#define mod_mpi_weights_empty _QMmpi_constantsEmpi_weights_empty
#define mod_mpi_argvs_null _QMmpi_constantsEmpi_argvs_null
#define mod_mpi_argv_null _QMmpi_constantsEmpi_argv_null

#else
// GFortran name mangling
__attribute__ ((dllimport)) int __mpi_constants_MOD_mpi_bottom;
__attribute__ ((dllimport)) int __mpi_constants_MOD_mpi_in_place;
__attribute__ ((dllimport)) int __mpi_constants_MOD_mpi_status_ignore[5];
__attribute__ ((dllimport)) int __mpi_constants_MOD_mpi_statuses_ignore[5][1];
__attribute__ ((dllimport)) int __mpi_constants_MOD_mpi_errcodes_ignore[1];
__attribute__ ((dllimport)) int __mpi_constants_MOD_mpi_unweighted;
__attribute__ ((dllimport)) int __mpi_constants_MOD_mpi_weights_empty;
__attribute__ ((dllimport)) int __mpi_constants_MOD_mpi_argvs_null;
__attribute__ ((dllimport)) int __mpi_constants_MOD_mpi_argv_null;

#define mod_mpi_bottom __mpi_constants_MOD_mpi_bottom
#define mod_mpi_in_place __mpi_constants_MOD_mpi_in_place
#define mod_mpi_status_ignore __mpi_constants_MOD_mpi_status_ignore
#define mod_mpi_statuses_ignore __mpi_constants_MOD_mpi_statuses_ignore
#define mod_mpi_errcodes_ignore __mpi_constants_MOD_mpi_errcodes_ignore
#define mod_mpi_unweighted __mpi_constants_MOD_mpi_unweighted
#define mod_mpi_weights_empty __mpi_constants_MOD_mpi_weights_empty
#define mod_mpi_argvs_null __mpi_constants_MOD_mpi_argvs_null
#define mod_mpi_argv_null __mpi_constants_MOD_mpi_argv_null

#endif

// global variables from "mpif.h"
__attribute__ ((dllimport)) int mpi_bottom;
__attribute__ ((dllimport)) int mpi_in_place;
__attribute__ ((dllimport)) int mpi_status_ignore[5];
__attribute__ ((dllimport)) int mpi_statuses_ignore[5][1];
__attribute__ ((dllimport)) int mpi_errcodes_ignore[1];
__attribute__ ((dllimport)) int mpi_unweighted;
__attribute__ ((dllimport)) int mpi_weights_empty;
__attribute__ ((dllimport)) int mpi_argvs_null;
__attribute__ ((dllimport)) int mpi_argv_null;

// ---------------------------------------------------------------------
// Pointers to C sentinel values
// ---------------------------------------------------------------------

static int *C_MPI_BOTTOM = NULL;
static int *C_MPI_IN_PLACE = NULL;
static MPI_Status *C_MPI_STATUS_IGNORE = NULL;
static MPI_Status *C_MPI_STATUSES_IGNORE = NULL;
static int *C_MPI_ERRCODES_IGNORE = NULL;
static int *C_MPI_UNWEIGHTED = NULL;
static int *C_MPI_WEIGHTS_EMPTY = NULL;
static char ***C_MPI_ARGVS_NULL = NULL;
static char **C_MPI_ARGV_NULL = NULL;

// ---------------------------------------------------------------------
// Sentinel mapping helpers
// ---------------------------------------------------------------------

static int need_sentinels = 1;

static void register_sentinels(void)
{
  // Use sentinel addresses from C header for variables that are in dllimported
  // COMMON blocks for Intel Fortran.
  C_MPI_BOTTOM = MPI_BOTTOM;
  C_MPI_IN_PLACE = MPI_IN_PLACE;
  C_MPI_STATUS_IGNORE = (MPI_Status*)MPI_STATUS_IGNORE;
  C_MPI_STATUSES_IGNORE = (MPI_Status*)MPI_STATUSES_IGNORE;
  C_MPI_ERRCODES_IGNORE = MPI_ERRCODES_IGNORE;
  C_MPI_UNWEIGHTED = MPI_UNWEIGHTED;
  C_MPI_WEIGHTS_EMPTY = MPI_WEIGHTS_EMPTY;
  C_MPI_ARGVS_NULL = MPI_ARGVS_NULL;
  C_MPI_ARGV_NULL = MPI_ARGV_NULL;

  need_sentinels = 0;
}

// Replace the Fortran sentinel values with their C counterpart.

static void* map_buf(void* arg)
{
  if (need_sentinels)
    register_sentinels();

  if (arg == &mod_mpi_in_place
      || arg == &mpi_in_place)
    return MPI_IN_PLACE;

  if (arg == &mod_mpi_bottom
      || arg == &mpi_bottom)
    return MPI_BOTTOM;

  return arg;
}

static MPI_Status* map_status(MPI_Status* arg)
{
  if (need_sentinels)
    register_sentinels();

  if ((int (*)[5])arg == &mod_mpi_status_ignore
      || (int (*)[5])arg == &mpi_status_ignore)
    return C_MPI_STATUS_IGNORE;

  return arg;
}

static MPI_Status* map_statuses(MPI_Status* arg)
{
  if (need_sentinels)
    register_sentinels();

  if ((int (*)[5][1])arg == &mod_mpi_statuses_ignore
      || (int (*)[5][1])arg == &mpi_statuses_ignore)
    return C_MPI_STATUSES_IGNORE;

  return arg;
}

static int *map_errcodes(int *arg)
{
  if (need_sentinels)
    register_sentinels();

  if ((int (*)[1])arg == &mod_mpi_errcodes_ignore
      || (int (*)[1])arg == &mpi_errcodes_ignore)
    return C_MPI_ERRCODES_IGNORE;

  return arg;
}

static int* map_weights(int* arg)
{
  if (need_sentinels)
    register_sentinels();

  if (arg == &mod_mpi_unweighted
      || arg == &mpi_unweighted)
    return C_MPI_UNWEIGHTED;

  if (arg == &mod_mpi_weights_empty
      || arg == &mpi_weights_empty)
    return C_MPI_WEIGHTS_EMPTY;

  return arg;
}

static char ***map_argvs(char ***arg)
{
  if (need_sentinels)
    register_sentinels();

  // Fortran passes the address of the module variable; compare that
  if ((int*)arg == &mod_mpi_argvs_null
      || (int*)arg == &mpi_argvs_null)
    return C_MPI_ARGVS_NULL;

  return arg;
}

static char **map_argv(char **arg)
{
  if (need_sentinels)
      register_sentinels();

  if ((int*)arg == &mod_mpi_argv_null
      || (int*)arg == &mpi_argv_null)
    return C_MPI_ARGV_NULL;

  return arg;
}

// ---------------------------------------------------------------------
// Dynamic loading of msmpi.dll
// ---------------------------------------------------------------------

static HMODULE hMsmpi = NULL;

static int load_msmpi(void)
{
  if (!hMsmpi)
    hMsmpi = GetModuleHandleA("msmpi.dll");
  if (!hMsmpi)
    hMsmpi = LoadLibraryA("msmpi.dll");
  return hMsmpi != NULL;
}

#define LOAD_MPI(name) \
  do { \
    if (! p##name) { \
      if (! load_msmpi()) return; \
      p##name = (name##_type) GetProcAddress(hMsmpi, #name); \
      if (!p##name) return; \
    } \
  } while (0)

// ---------------------------------------------------------------------
// C MPI function pointer typedefs
// ---------------------------------------------------------------------

typedef int (MPIAPI *MPI_Allreduce_type)(void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm);
typedef int (MPIAPI *MPI_Reduce_type)(void*, void*, int, MPI_Datatype, MPI_Op, int, MPI_Comm);
typedef int (MPIAPI *MPI_Reduce_scatter_type)(void*, void*, int*, MPI_Datatype, MPI_Op, MPI_Comm);
typedef int (MPIAPI *MPI_Reduce_scatter_block_type)(void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm);
typedef int (MPIAPI *MPI_Reduce_local_type)(void*, void*, int, MPI_Datatype, MPI_Op);
typedef int (MPIAPI *MPI_Scan_type)(void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm);
typedef int (MPIAPI *MPI_Exscan_type)(void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm);

typedef int (MPIAPI *MPI_Gather_type)(void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm);
typedef int (MPIAPI *MPI_Gatherv_type)(void*, int, MPI_Datatype, void*, int*, int*, MPI_Datatype, int, MPI_Comm);
typedef int (MPIAPI *MPI_Allgather_type)(void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm);
typedef int (MPIAPI *MPI_Allgatherv_type)(void*, int, MPI_Datatype, void*, int*, int*, MPI_Datatype, MPI_Comm);
typedef int (MPIAPI *MPI_Scatter_type)(void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm);
typedef int (MPIAPI *MPI_Scatterv_type)(void*, int*, int*, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm);
typedef int (MPIAPI *MPI_Alltoall_type)(void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm);
typedef int (MPIAPI *MPI_Alltoallv_type)(void*, int*, int*, MPI_Datatype, void*, int*, int*, MPI_Datatype, MPI_Comm);
typedef int (MPIAPI *MPI_Alltoallw_type)(void*, int*, int*, MPI_Datatype*, void*, int*, int*, MPI_Datatype*, MPI_Comm);

typedef int (MPIAPI *MPI_Wait_type)(MPI_Request*, MPI_Status*);
typedef int (MPIAPI *MPI_Test_type)(MPI_Request*, int*, MPI_Status*);

typedef int (MPIAPI *MPI_Waitall_type)(int, MPI_Request*, MPI_Status*);
typedef int (MPIAPI *MPI_Testall_type)(int, MPI_Request*, int*, MPI_Status*);

typedef int (MPIAPI *MPI_Waitany_type)(int, MPI_Request*, int*, MPI_Status*);
typedef int (MPIAPI *MPI_Testany_type)(int, MPI_Request*, int*, int*, MPI_Status*);

typedef int (MPIAPI *MPI_Waitsome_type)(int, MPI_Request*, int*, int*, MPI_Status*);
typedef int (MPIAPI *MPI_Testsome_type)(int, MPI_Request*, int*, int*, MPI_Status*);

typedef int (MPIAPI *MPI_Comm_spawn_type)(
    const char *command,
    char *argv[],
    int maxprocs,
    MPI_Info info,
    int root,
    MPI_Comm comm,
    MPI_Comm *intercomm,
    int array_of_errcodes[]
);
typedef int (MPIAPI *MPI_Comm_spawn_multiple_type)(
    int count,
    char *array_of_commands[],
    char **array_of_argv[],
    int array_of_maxprocs[],
    MPI_Info array_of_info[],
    int root,
    MPI_Comm comm,
    MPI_Comm *intercomm,
    int array_of_errcodes[]
);

typedef int (MPIAPI *MPI_Cart_weighted_create_type)(
    MPI_Comm, int, const int*, const int*, MPI_Comm*
);

typedef int (MPIAPI *MPI_Dist_graph_neighbors_type)(
    MPI_Comm comm,
    int maxindegree,
    int sources[],
    int sourceweights[],
    int maxoutdegree,
    int destinations[],
    int destweights[]
);

// (Nonblocking variants – prototypes simplified to match MS-MPI C API)
typedef int (MPIAPI *MPI_Ireduce_type)(void*, void*, int, MPI_Datatype, MPI_Op, int, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Iallreduce_type)(void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Ireduce_scatter_type)(void*, void*, int*, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Ireduce_scatter_block_type)(void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Iscan_type)(void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Iexscan_type)(void*, void*, int, MPI_Datatype, MPI_Op, MPI_Comm, MPI_Request*);

typedef int (MPIAPI *MPI_Igather_type)(void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Igatherv_type)(void*, int, MPI_Datatype, void*, int*, int*, MPI_Datatype, int, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Iallgather_type)(void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Iallgatherv_type)(void*, int, MPI_Datatype, void*, int*, int*, MPI_Datatype, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Iscatter_type)(void*, int, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Iscatterv_type)(void*, int*, int*, MPI_Datatype, void*, int, MPI_Datatype, int, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Ialltoall_type)(void*, int, MPI_Datatype, void*, int, MPI_Datatype, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Ialltoallv_type)(void*, int*, int*, MPI_Datatype, void*, int*, int*, MPI_Datatype, MPI_Comm, MPI_Request*);
typedef int (MPIAPI *MPI_Ialltoallw_type)(void*, int*, int*, MPI_Datatype*, void*, int*, int*, MPI_Datatype*, MPI_Comm, MPI_Request*);

// ---------------------------------------------------------------------
// Function pointers
// ---------------------------------------------------------------------

static MPI_Allreduce_type pMPI_Allreduce = NULL;
static MPI_Reduce_type pMPI_Reduce = NULL;
static MPI_Reduce_scatter_type pMPI_Reduce_scatter = NULL;
static MPI_Reduce_scatter_block_type pMPI_Reduce_scatter_block = NULL;
static MPI_Reduce_local_type pMPI_Reduce_local = NULL;
static MPI_Scan_type pMPI_Scan = NULL;
static MPI_Exscan_type pMPI_Exscan = NULL;

static MPI_Gather_type pMPI_Gather = NULL;
static MPI_Gatherv_type pMPI_Gatherv = NULL;
static MPI_Allgather_type pMPI_Allgather = NULL;
static MPI_Allgatherv_type pMPI_Allgatherv = NULL;
static MPI_Scatter_type pMPI_Scatter = NULL;
static MPI_Scatterv_type pMPI_Scatterv = NULL;
static MPI_Alltoall_type pMPI_Alltoall = NULL;
static MPI_Alltoallv_type pMPI_Alltoallv = NULL;
static MPI_Alltoallw_type pMPI_Alltoallw = NULL;

static MPI_Wait_type pMPI_Wait = NULL;
static MPI_Test_type pMPI_Test = NULL;

static MPI_Waitall_type pMPI_Waitall = NULL;
static MPI_Testall_type pMPI_Testall = NULL;

static MPI_Waitany_type pMPI_Waitany = NULL;
static MPI_Testany_type pMPI_Testany = NULL;

static MPI_Waitsome_type pMPI_Waitsome = NULL;
static MPI_Testsome_type pMPI_Testsome = NULL;

static MPI_Comm_spawn_type pMPI_Comm_spawn = NULL;
static MPI_Comm_spawn_multiple_type pMPI_Comm_spawn_multiple = NULL;

static MPI_Cart_weighted_create_type pMPI_Cart_weighted_create = NULL;
static MPI_Dist_graph_neighbors_type pMPI_Dist_graph_neighbors = NULL;

static MPI_Ireduce_type pMPI_Ireduce = NULL;
static MPI_Iallreduce_type pMPI_Iallreduce = NULL;
static MPI_Ireduce_scatter_type pMPI_Ireduce_scatter = NULL;
static MPI_Ireduce_scatter_block_type pMPI_Ireduce_scatter_block = NULL;
static MPI_Iscan_type pMPI_Iscan = NULL;
static MPI_Iexscan_type pMPI_Iexscan = NULL;

static MPI_Igather_type pMPI_Igather = NULL;
static MPI_Igatherv_type pMPI_Igatherv = NULL;
static MPI_Iallgather_type pMPI_Iallgather = NULL;
static MPI_Iallgatherv_type pMPI_Iallgatherv = NULL;
static MPI_Iscatter_type pMPI_Iscatter = NULL;
static MPI_Iscatterv_type pMPI_Iscatterv = NULL;
static MPI_Ialltoall_type pMPI_Ialltoall = NULL;
static MPI_Ialltoallv_type pMPI_Ialltoallv = NULL;
static MPI_Ialltoallw_type pMPI_Ialltoallw = NULL;

// ---------------------------------------------------------------------
// Fortran wrappers (gfortran: lowercase + trailing underscore)
// ---------------------------------------------------------------------
//
// These functions replace the original ones from msmpi.dll when called from
// Fortran code compiled with GFortran.

// -------------------- Collective ops with IN_PLACE/BOTTOM --------------------

void mpi_allreduce_(void *sendbuf, void *recvbuf,
                    int *count, int *datatype, int *op, int *comm, int *ierr)
{
  LOAD_MPI(MPI_Allreduce);

  *ierr = pMPI_Allreduce(
    map_buf(sendbuf), recvbuf,
    *count, (MPI_Datatype)*datatype, (MPI_Op)*op, (MPI_Comm)*comm);
}

void mpi_reduce_(void *sendbuf, void *recvbuf,
                 int *count, int *datatype, int *op, int *root, int *comm, int *ierr)
{
  LOAD_MPI(MPI_Reduce);

  *ierr = pMPI_Reduce(
    map_buf(sendbuf), recvbuf,
    *count, (MPI_Datatype)*datatype, (MPI_Op)*op, *root, (MPI_Comm)*comm);
}

void mpi_reduce_scatter_(void *sendbuf, void *recvbuf,
                         int *recvcounts, int *datatype, int *op, int *comm, int *ierr)
{
  LOAD_MPI(MPI_Reduce_scatter);

  *ierr = pMPI_Reduce_scatter(
    map_buf(sendbuf), recvbuf,
    recvcounts, (MPI_Datatype)*datatype, (MPI_Op)*op, (MPI_Comm)*comm);
}

void mpi_reduce_scatter_block_(void *sendbuf, void *recvbuf,
                               int *recvcount, int *datatype, int *op, int *comm, int *ierr)
{
  LOAD_MPI(MPI_Reduce_scatter_block);

  *ierr = pMPI_Reduce_scatter_block(
    map_buf(sendbuf), recvbuf,
    *recvcount, (MPI_Datatype)*datatype, (MPI_Op)*op, (MPI_Comm)*comm);
}

void mpi_reduce_local_(void *inbuf, void *inoutbuf,
                       int *count, int *datatype, int *op,
                       int *ierr)
{
  LOAD_MPI(MPI_Reduce_local);

  *ierr = pMPI_Reduce_local(
      map_buf(inbuf),
      map_buf(inoutbuf),
      *count,
      (MPI_Datatype)*datatype,
      (MPI_Op)*op
  );
}

void mpi_scan_(void *sendbuf, void *recvbuf,
               int *count, int *datatype, int *op, int *comm, int *ierr)
{
  LOAD_MPI(MPI_Scan);

  *ierr = pMPI_Scan(
    map_buf(sendbuf), recvbuf,
    *count, (MPI_Datatype)*datatype, (MPI_Op)*op, (MPI_Comm)*comm);
}

void mpi_exscan_(void *sendbuf, void *recvbuf,
                 int *count, int *datatype, int *op, int *comm, int *ierr)
{
  LOAD_MPI(MPI_Exscan);

  *ierr = pMPI_Exscan(
    map_buf(sendbuf), recvbuf,
    *count, (MPI_Datatype)*datatype, (MPI_Op)*op, (MPI_Comm)*comm);
}

// -------------------- Gather/scatter/alltoall with IN_PLACE --------------------

void mpi_gather_(void *sendbuf, int *sendcount, int *sendtype,
                 void *recvbuf, int *recvcount, int *recvtype,
                 int *root, int *comm, int *ierr)
{
  LOAD_MPI(MPI_Gather);

  *ierr = pMPI_Gather(
    map_buf(sendbuf), *sendcount, (MPI_Datatype)*sendtype,
    recvbuf, *recvcount, (MPI_Datatype)*recvtype,
    *root, (MPI_Comm)*comm);
}

void mpi_gatherv_(void *sendbuf, int *sendcount, int *sendtype,
                  void *recvbuf, int *recvcounts, int *displs, int *recvtype,
                  int *root, int *comm, int *ierr)
{
  LOAD_MPI(MPI_Gatherv);

  *ierr = pMPI_Gatherv(
    map_buf(sendbuf), *sendcount, (MPI_Datatype)*sendtype,
    recvbuf, recvcounts, displs, (MPI_Datatype)*recvtype,
    *root, (MPI_Comm)*comm);
}

void mpi_allgather_(void *sendbuf, int *sendcount, int *sendtype,
                    void *recvbuf, int *recvcount, int *recvtype,
                    int *comm, int *ierr)
{
  LOAD_MPI(MPI_Allgather);

  *ierr = pMPI_Allgather(
    map_buf(sendbuf), *sendcount, (MPI_Datatype)*sendtype,
    recvbuf, *recvcount, (MPI_Datatype)*recvtype,
    (MPI_Comm)*comm);
}

void mpi_allgatherv_(void *sendbuf, int *sendcount, int *sendtype,
                     void *recvbuf, int *recvcounts, int *displs, int *recvtype,
                     int *comm, int *ierr)
{
  LOAD_MPI(MPI_Allgatherv);

  *ierr = pMPI_Allgatherv(
    map_buf(sendbuf), *sendcount, (MPI_Datatype)*sendtype,
    recvbuf, recvcounts, displs, (MPI_Datatype)*recvtype,
    (MPI_Comm)*comm);
}

void mpi_scatter_(void *sendbuf, int *sendcount, int *sendtype,
                  void *recvbuf, int *recvcount, int *recvtype,
                  int *root, int *comm, int *ierr)
{
  LOAD_MPI(MPI_Scatter);

  *ierr = pMPI_Scatter(
    map_buf(sendbuf), *sendcount, (MPI_Datatype)*sendtype,
    recvbuf, *recvcount, (MPI_Datatype)*recvtype,
    *root, (MPI_Comm)*comm);
}

void mpi_scatterv_(void *sendbuf, int *sendcounts, int *displs, int *sendtype,
                   void *recvbuf, int *recvcount, int *recvtype,
                   int *root, int *comm, int *ierr)
{
  LOAD_MPI(MPI_Scatterv);

  *ierr = pMPI_Scatterv(
    map_buf(sendbuf), sendcounts, displs, (MPI_Datatype)*sendtype,
    recvbuf, *recvcount, (MPI_Datatype)*recvtype,
    *root, (MPI_Comm)*comm);
}

void mpi_alltoall_(void *sendbuf, int *sendcount, int *sendtype,
                   void *recvbuf, int *recvcount, int *recvtype,
                   int *comm, int *ierr)
{
  LOAD_MPI(MPI_Alltoall);

  *ierr = pMPI_Alltoall(
    map_buf(sendbuf), *sendcount, (MPI_Datatype)*sendtype,
    recvbuf, *recvcount, (MPI_Datatype)*recvtype,
    (MPI_Comm)*comm);
}

void mpi_alltoallv_(void *sendbuf, int *sendcounts, int *sdispls, int *sendtype,
                    void *recvbuf, int *recvcounts, int *rdispls, int *recvtype,
                    int *comm, int *ierr)
{
  LOAD_MPI(MPI_Alltoallv);

  *ierr = pMPI_Alltoallv(
    map_buf(sendbuf), sendcounts, sdispls, (MPI_Datatype)*sendtype,
    recvbuf, recvcounts, rdispls, (MPI_Datatype)*recvtype,
    (MPI_Comm)*comm);
}

void mpi_alltoallw_(void *sendbuf, int *sendcounts, int *sdispls, MPI_Datatype *sendtypes,
                    void *recvbuf, int *recvcounts, int *rdispls, MPI_Datatype *recvtypes,
                    int *comm, int *ierr)
{
  LOAD_MPI(MPI_Alltoallw);

  *ierr = pMPI_Alltoallw(
    map_buf(sendbuf), sendcounts, sdispls, sendtypes,
    recvbuf, recvcounts, rdispls, recvtypes,
    (MPI_Comm)*comm);
}

// -------------------- Status-ignore wrappers --------------------

void mpi_wait_(MPI_Request *req, MPI_Status *status, int *ierr)
{
  LOAD_MPI(MPI_Wait);

  *ierr = pMPI_Wait(req, map_status(status));
}

void mpi_test_(MPI_Request *req, int *flag, MPI_Status *status, int *ierr)
{
  LOAD_MPI(MPI_Test);

  *ierr = pMPI_Test(req, flag, map_status(status));
}

void mpi_waitall_(int *n, MPI_Request *reqs, MPI_Status *stats, int *ierr)
{
  LOAD_MPI(MPI_Waitall);

  *ierr = pMPI_Waitall(*n, reqs, map_statuses(stats));
}

void mpi_testall_(int *n, MPI_Request *reqs, int *flag, MPI_Status *stats, int *ierr)
{
  LOAD_MPI(MPI_Testall);

  *ierr = pMPI_Testall(*n, reqs, flag, map_statuses(stats));
}

void mpi_waitany_(int *n, MPI_Request *reqs, int *index, MPI_Status *status, int *ierr)
{
  LOAD_MPI(MPI_Waitany);

  *ierr = pMPI_Waitany(*n, reqs, index, map_status(status));
}

void mpi_testany_(int *n, MPI_Request *reqs, int *flag, int *index, MPI_Status *status, int *ierr)
{
  LOAD_MPI(MPI_Testany);

  *ierr = pMPI_Testany(*n, reqs, flag, index, map_status(status));
}

void mpi_waitsome_(int *n, MPI_Request *reqs, int *outcount, int *indices, MPI_Status *stats, int *ierr)
{
  LOAD_MPI(MPI_Waitsome);

  *ierr = pMPI_Waitsome(*n, reqs, outcount, indices, map_statuses(stats));
}

void mpi_testsome_(int *n, MPI_Request *reqs, int *outcount, int *indices, MPI_Status *stats, int *ierr)
{
  LOAD_MPI(MPI_Testsome);

  *ierr = pMPI_Testsome(*n, reqs, outcount, indices, map_statuses(stats));
}

// -------------------- spawn wrappers (ARGV/ARGVS + ERR CODES) --------------------

void mpi_comm_spawn_(
    char *command,
    char **argv,
    int *maxprocs,
    int *info,
    int *root,
    int *comm,
    int *intercomm,
    int *array_of_errcodes,
    int *ierr,
    int d1,
    int d2)
{
  LOAD_MPI(MPI_Comm_spawn);

  *ierr = pMPI_Comm_spawn(
      command,
      map_argv(argv),
      *maxprocs,
      (MPI_Info)*info,
      *root,
      (MPI_Comm)*comm,
      (MPI_Comm*)intercomm,
      map_errcodes(array_of_errcodes)
  );
}

void mpi_comm_spawn_multiple_(
    int *count,
    char *array_of_commands[], // assumed-shape CHARACTER(*) array
    char ***array_of_argv,     // Fortran passes something that ends up as char***
    int *array_of_maxprocs,
    int *array_of_info,
    int *root,
    int *comm,
    int *intercomm,
    int *array_of_errcodes,
    int *ierr,
    int d2,
    int d3)
{
  LOAD_MPI(MPI_Comm_spawn_multiple);

  *ierr = pMPI_Comm_spawn_multiple(
      *count,
      array_of_commands,
      map_argvs(array_of_argv),
      array_of_maxprocs,
      (MPI_Info*)array_of_info,
      *root,
      (MPI_Comm)*comm,
      (MPI_Comm*)intercomm,
      map_errcodes(array_of_errcodes)
  );
}

// -------------------- cart_weighted_create wrapper (UNWEIGHTED/WEIGHTS_EMPTY) --------------------

void mpi_cart_weighted_create_(
    int *comm_old,
    int *ndims,
    int *dims,
    int *weights,
    int *comm_cart,
    int *ierr)
{
  LOAD_MPI(MPI_Cart_weighted_create);

  *ierr = pMPI_Cart_weighted_create(
      (MPI_Comm)*comm_old,
      *ndims,
      dims,
      map_weights(weights),
      (MPI_Comm*)comm_cart
  );
}

// -------------------- dist_graph_neighbors wrapper --------------------

void mpi_dist_graph_neighbors_(
    int *comm,
    int *maxindegree,
    int *sources,
    int *sourceweights,
    int *maxoutdegree,
    int *destinations,
    int *destweights,
    int *ierr)
{
  LOAD_MPI(MPI_Dist_graph_neighbors);

  *ierr = pMPI_Dist_graph_neighbors(
      (MPI_Comm)*comm,
      *maxindegree,
      sources,
      map_weights(sourceweights),
      *maxoutdegree,
      destinations,
      map_weights(destweights)
  );
}

// -------------------- nonblocking reduction-style collectives with IN_PLACE --------------------

void mpi_ireduce_(void *sendbuf, void *recvbuf,
                  int *count, int *datatype, int *op,
                  int *root, int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Ireduce);

  *ierr = pMPI_Ireduce(
      map_buf(sendbuf), recvbuf,
      *count, (MPI_Datatype)*datatype, (MPI_Op)*op,
      *root, (MPI_Comm)*comm, request);
}

void mpi_iallreduce_(void *sendbuf, void *recvbuf,
                     int *count, int *datatype, int *op,
                     int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Iallreduce);

  *ierr = pMPI_Iallreduce(
      map_buf(sendbuf), recvbuf,
      *count, (MPI_Datatype)*datatype, (MPI_Op)*op,
      (MPI_Comm)*comm, request);
}

void mpi_ireduce_scatter_(void *sendbuf, void *recvbuf,
                          int *recvcounts, int *datatype, int *op,
                          int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Ireduce_scatter);

  *ierr = pMPI_Ireduce_scatter(
      map_buf(sendbuf), recvbuf,
      recvcounts, (MPI_Datatype)*datatype, (MPI_Op)*op,
      (MPI_Comm)*comm, request);
}

void mpi_ireduce_scatter_block_(void *sendbuf, void *recvbuf,
                                int *recvcount, int *datatype, int *op,
                                int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Ireduce_scatter_block);

  *ierr = pMPI_Ireduce_scatter_block(
      map_buf(sendbuf), recvbuf,
      *recvcount, (MPI_Datatype)*datatype, (MPI_Op)*op,
      (MPI_Comm)*comm, request);
}

void mpi_iscan_(void *sendbuf, void *recvbuf,
                int *count, int *datatype, int *op,
                int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Iscan);

  *ierr = pMPI_Iscan(
      map_buf(sendbuf), recvbuf,
      *count, (MPI_Datatype)*datatype, (MPI_Op)*op,
      (MPI_Comm)*comm, request);
}

void mpi_iexscan_(void *sendbuf, void *recvbuf,
                  int *count, int *datatype, int *op,
                  int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Iexscan);

  *ierr = pMPI_Iexscan(
      map_buf(sendbuf), recvbuf,
      *count, (MPI_Datatype)*datatype, (MPI_Op)*op,
      (MPI_Comm)*comm, request);
}

// -------------------- nonblocking gather/scatter/alltoall with IN_PLACE --------------------

void mpi_igather_(void *sendbuf, int *sendcount, int *sendtype,
                  void *recvbuf, int *recvcount, int *recvtype,
                  int *root, int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Igather);

  *ierr = pMPI_Igather(
      map_buf(sendbuf), *sendcount, (MPI_Datatype)*sendtype,
      recvbuf, *recvcount, (MPI_Datatype)*recvtype,
      *root, (MPI_Comm)*comm, request);
}

void mpi_igatherv_(void *sendbuf, int *sendcount, int *sendtype,
                   void *recvbuf, int *recvcounts, int *displs, int *recvtype,
                   int *root, int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Igatherv);

  *ierr = pMPI_Igatherv(
      map_buf(sendbuf), *sendcount, (MPI_Datatype)*sendtype,
      recvbuf, recvcounts, displs, (MPI_Datatype)*recvtype,
      *root, (MPI_Comm)*comm, request);
}

void mpi_iallgather_(void *sendbuf, int *sendcount, int *sendtype,
                     void *recvbuf, int *recvcount, int *recvtype,
                     int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Iallgather);

  *ierr = pMPI_Iallgather(
      map_buf(sendbuf), *sendcount, (MPI_Datatype)*sendtype,
      recvbuf, *recvcount, (MPI_Datatype)*recvtype,
      (MPI_Comm)*comm, request);
}

void mpi_iallgatherv_(void *sendbuf, int *sendcount, int *sendtype,
                      void *recvbuf, int *recvcounts, int *displs, int *recvtype,
                      int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Iallgatherv);

  *ierr = pMPI_Iallgatherv(
      map_buf(sendbuf), *sendcount, (MPI_Datatype)*sendtype,
      recvbuf, recvcounts, displs, (MPI_Datatype)*recvtype,
      (MPI_Comm)*comm, request);
}

void mpi_iscatter_(void *sendbuf, int *sendcount, int *sendtype,
                   void *recvbuf, int *recvcount, int *recvtype,
                   int *root, int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Iscatter);

  *ierr = pMPI_Iscatter(
      map_buf(sendbuf), *sendcount, (MPI_Datatype)*sendtype,
      recvbuf, *recvcount, (MPI_Datatype)*recvtype,
      *root, (MPI_Comm)*comm, request);
}

void mpi_iscatterv_(void *sendbuf, int *sendcounts, int *displs, int *sendtype,
                    void *recvbuf, int *recvcount, int *recvtype,
                    int *root, int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Iscatterv);

  *ierr = pMPI_Iscatterv(
      map_buf(sendbuf), sendcounts, displs, (MPI_Datatype)*sendtype,
      recvbuf, *recvcount, (MPI_Datatype)*recvtype,
      *root, (MPI_Comm)*comm, request);
}

void mpi_ialltoall_(void *sendbuf, int *sendcount, int *sendtype,
                    void *recvbuf, int *recvcount, int *recvtype,
                    int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Ialltoall);

  *ierr = pMPI_Ialltoall(
      map_buf(sendbuf), *sendcount, (MPI_Datatype)*sendtype,
      recvbuf, *recvcount, (MPI_Datatype)*recvtype,
      (MPI_Comm)*comm, request);
}

void mpi_ialltoallv_(void *sendbuf, int *sendcounts, int *sdispls, int *sendtype,
                     void *recvbuf, int *recvcounts, int *rdispls, int *recvtype,
                     int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Ialltoallv);

  *ierr = pMPI_Ialltoallv(
      map_buf(sendbuf), sendcounts, sdispls, (MPI_Datatype)*sendtype,
      recvbuf, recvcounts, rdispls, (MPI_Datatype)*recvtype,
      (MPI_Comm)*comm, request);
}

void mpi_ialltoallw_(void *sendbuf, int *sendcounts, int *sdispls, MPI_Datatype *sendtypes,
                     void *recvbuf, int *recvcounts, int *rdispls, MPI_Datatype *recvtypes,
                     int *comm, MPI_Request *request, int *ierr)
{
  LOAD_MPI(MPI_Ialltoallw);

  *ierr = pMPI_Ialltoallw(
      map_buf(sendbuf), sendcounts, sdispls, sendtypes,
      recvbuf, recvcounts, rdispls, recvtypes,
      (MPI_Comm)*comm, request);
}
