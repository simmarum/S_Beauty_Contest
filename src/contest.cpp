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
#include <vector>
#include "crit.h"
#include "error_code.h"
#include "packet_communication.h"
#include "tags.h"
#include "utils.h"

int raw_s_r = true;
int csv = true;
int debug = false;

// function definitions
bool cli_parameters(int argc, char *argv[], int &L, int &S);
void *receive_loop(void *ptr);
void enable_thread(int *argc, char ***argv);
// global variable
int rank;        // id of this thread
int lclock = 0;  // lamport clock
std::vector<crit_sruct> *doctor_arr;

// implementations
int main(int argc, char *argv[]) {
  int L = 0;  // number of doctor
  int S = 0;  // capacity of salon

  // check parameters
  if (!cli_parameters(argc, argv, L, S)) {
    printf("WRONG PARAMETERS %d %d\n", L, S);
    return ERROR_WRONG_PARAMETERS;
  }

  // call MPI for threads mechanism
  enable_thread(&argc, &argv);

  // create and run thread for receive
  pthread_t receive_thread;
  pthread_create(&receive_thread, NULL, receive_loop, 0);

  // initialize everything
  int size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* get current process id */
  MPI_Comm_size(MPI_COMM_WORLD, &size); /* get number of processes */

  doctor_arr = new std::vector<crit_sruct>[size];

  // starts proper compute
  if (rank == 0 or rank == 1) {
    want_crit_sec(doctor_arr[0], lclock, 0, TAG_WANT_DOCTOR, rank, size);
  }

  // send end compute and exit process
  send_end_compute(lclock, rank, size);
  pthread_join(receive_thread, NULL);
  MPI_Finalize();
  return EXIT_SUCCESS;
}

void *receive_loop(void *ptr) {
  bool is_compute = true;
  while (is_compute) {
    MPI_Status status;
    packet_s recv;
    myRecv(lclock, recv, MPI_ANY_SOURCE, MPI_ANY_TAG, status, rank);
    switch (status.MPI_TAG) {
      case TAG_END:
        is_compute = false;
        break;
      case TAG_WANT_DOCTOR:
        break;
      case TAG_ACK_DOCTOR:
        break;
      case TAG_RLS_DOCTOR:
        break;
      case TAG_WANT_SALON:
        break;
      case TAG_ACK_SALON:
        break;
      case TAG_RLS_SALON:
        break;
      default:
        printf("WRONG TAG %d", status.MPI_TAG);
        exit(EXIT_FAILURE);
    }
  }
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
  if (debug) {
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
  }
  if (provided != MPI_THREAD_MULTIPLE) {
    fprintf(stderr, "There is not enough support for threads - I'm leaving!\n");
    MPI_Finalize();
    exit(EXIT_FAILURE);
  }
}
