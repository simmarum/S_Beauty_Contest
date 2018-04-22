/* --------------------------------*/
/* Beauty contest                  */
/* Authors:                        */
/*         Mateusz Kruszyna        */
/*         Mikołaj Stankowiak      */
/* --------------------------------*/

#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "error_code.h"
#include "packet_communication.h"

// function definitions
bool cli_parameters(int argc, char *argv[], int &L, int &S);
void *receive_loop(void *ptr);
void enable_thread(int *argc, char ***argv);
// global variable
int rank;        // id of this thread
int lclock = 0;  // lamport clock

// implementations
int main(int argc, char *argv[]) {
  int L = 0;  // number of doctor
  int S = 0;  // capacity of salon

  // check parameters
  if (!cli_parameters(argc, argv, L, S)) {
    return ERROR_WRONG_PARAMETERS;
  }

  // call MPI for threads mechanism
  enable_thread(&argc, &argv);

  // create and run thread for receive
  pthread_t receive_thread;
  pthread_create(&receive_thread, NULL, receive_loop, 0);

  // initialize everything
  int size;

  // MPI_Init(&argc, &argv);               /* starts MPI */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* get current process id */
  MPI_Comm_size(MPI_COMM_WORLD, &size); /* get number of processes */
  printf("Hello world from processs %d of %d\n", rank, size);

  // starts proper compute
  if (rank == 0) {
    for (int i = 1; i < size; i++) {
      mySend(lclock, 1, i, 100);
      printf("Rank %d: sent structure to %d\n", rank, i);
    }
    for (int i = 0; i < size; i++) {
      mySend(lclock, 5, i, 100);
      printf("Rank %d: sent structure to %d\n", rank, i);
    }
  }

  pthread_join(receive_thread, NULL);
  printf("END %d\n", rank);
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

void enable_thread(int *argc, char ***argv) {
  int provided = 0;
  MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);
  switch (provided) {
    case MPI_THREAD_SINGLE:
      printf("thread level supported: MPI_THREAD_SINGLE\n");
      break;
    case MPI_THREAD_FUNNELED:
      printf("thread level supported: MPI_THREAD_FUNNELED\n");
      break;
    case MPI_THREAD_SERIALIZED:
      printf("thread level supported: MPI_THREAD_SERIALIZED\n");
      break;
    case MPI_THREAD_MULTIPLE:
      printf("thread level supported: MPI_THREAD_MULTIPLE\n");
      break;
    default:
      printf("thread level supported: UNRECOGNIZED\n");
      exit(EXIT_FAILURE);
  }
  if (provided != MPI_THREAD_MULTIPLE) {
    fprintf(stderr, "There is not enough support for threads - I'm leaving!\n");
    MPI_Finalize();
    exit(EXIT_FAILURE);
  }
}

void *receive_loop(void *ptr) {
  while (1) {
    MPI_Status status;
    packet_s recv;
    myRecv(lclock, recv, MPI_ANY_SOURCE, MPI_ANY_TAG, status);
    printf("Rank %d: Received from %d: clock = %d message = %d\n", rank,
           status.MPI_SOURCE, recv.clock, recv.message);
    if (recv.message == 5) {
      break;
    }
  }
}