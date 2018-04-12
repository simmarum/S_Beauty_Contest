/* C Example */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

bool cli_parameters(int argc, char *argv[], int &L, int &S);

int main(int argc, char *argv[]) {
  int L = 0, S = 0;

  if (!cli_parameters(argc, argv, L, S)) {
    return 11;
  }
  printf("%d %d \n", L, S);
  int rank, size;

  MPI_Init(&argc, &argv);               /* starts MPI */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* get current process id */
  MPI_Comm_size(MPI_COMM_WORLD, &size); /* get number of processes */
  printf("Hello world from process %d of %d\n", rank, size);
  MPI_Finalize();
  return 0;
}

bool cli_parameters(int argc, char *argv[], int &L, int &S) {
  if (argc != 3) {
    return false;
  } else {
    L = atoi(argv[1]);
    S = atoi(argv[2]);
    if (L == 0 || S == 0) {
      return false;
    }
  }
  return true;
}