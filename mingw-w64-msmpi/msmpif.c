// GFortran does not support DLLIMPORT on COMMON block variables. And even if
// it did, the COMMON block variables that are exported from msmpi.dll have
// Intel Fortran layout (which would be incompatible with GFortran anyway).
//
// To still support using the sentinel variables that msmpi.dll exports in Intel
// Fortran COMMON blocks, replace them in a wrapper using sentinel variables that
// are exported from a small helper DLL.

// global variables for mpif.h (in GFortran name mangling
__attribute__ ((dllexport)) int mpi_bottom;
__attribute__ ((dllexport)) int mpi_in_place;
__attribute__ ((dllexport)) int mpi_status_ignore[5];
__attribute__ ((dllexport)) int mpi_statuses_ignore[5][1];
__attribute__ ((dllexport)) int mpi_errcodes_ignore[1];
__attribute__ ((dllexport)) int mpi_unweighted;
__attribute__ ((dllexport)) int mpi_weights_empty;
__attribute__ ((dllexport)) int mpi_argvs_null;
__attribute__ ((dllexport)) int mpi_argv_null;
