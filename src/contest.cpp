/* --------------------------------*/
/* Beauty contest                  */
/* Authors:                        */
/*         Mateusz Kruszyna        */
/*         Mikołaj Stankowiak      */
/* --------------------------------*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "error_code.h"
#include "packet_communication.h"

bool cli_parameters(int argc, char *argv[], int &L, int &S);

int main(int argc, char *argv[]) {
  int L = 0;       // number of doctor
  int S = 0;       // capacity of salon
  int lclock = 0;  // lamport clock

  if (!cli_parameters(argc, argv, L, S)) {
    return ERROR_WRONG_PARAMETERS;
  }

  int rank, size;

  MPI_Init(&argc, &argv);               /* starts MPI */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* get current process id */
  MPI_Comm_size(MPI_COMM_WORLD, &size); /* get number of processes */
  printf("Hello world from processs %d of %d\n", rank, size);
  if (rank == 0) {
    for (int i = 0; i < 10; i++) {
      mySend(lclock, i, 1, 100);
      printf("Rank %d: sent structure\n", rank);
    }
  }
  if (rank == 1) {
    MPI_Status status;
    packet_s recv;
    for (int i = 0; i < 10; i++) {
      myRecv(lclock, recv, 0, 100, status);
      printf("Rank %d: Received from %d: clock = %d message = %d\n", rank,
             status.MPI_SOURCE, recv.clock, recv.message);
    }
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
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