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
int csv = false;
int debug = false;

// function definitions
bool cli_parameters(int argc, char *argv[], int &L, int &S);
void *receive_loop(void *ptr);
void enable_thread(int *argc, char ***argv);

// global variable
int rank = 0;    // id of this thread
int lclock = 0;  // lamport clock
int size = 0;    // number of processes

int *doctor_ack;
std::vector<crit_sruct> *doctor_arr;
std::vector<pthread_mutex_t> doctor_mutex;

// implementations
int main(int argc, char *argv[]) {
  int L = 0;  // number of doctor
  int S = 0;  // capacity of salon
  int M = 0;  // number of models

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

  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* get current process id */
  MPI_Comm_size(MPI_COMM_WORLD, &size); /* get number of processes */

  srand(rank + (int)time(0));  // random seed
  // rand from 1 to capacity of Salon
  M = (int)((rand() / (RAND_MAX + 1.0)) * S) + 1;

  // create doctor critical section mutex and ack array
  doctor_ack = new int[size];
  doctor_arr = new std::vector<crit_sruct>[size];
  for (int i = 0; i < size; i++) {
    doctor_ack[i] = 0;
    pthread_mutex_t tmp_mutex = PTHREAD_MUTEX_INITIALIZER;
    doctor_mutex.push_back(tmp_mutex);
  }
  // synchronize
  MPI_Barrier(MPI_COMM_WORLD);

  // starts proper compute

  // want critical section for doctor
  int which_doctor = rank % L;
  want_crit_sec(doctor_mutex[which_doctor], doctor_arr[which_doctor], lclock,
                which_doctor, TAG_WANT_DOCTOR, rank, size);

  // for receive all message (0.5 second)
  usleep(500000);
  print_crit_section(doctor_arr[0], rank);
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
    int recv[3];
    myRecv(lclock, recv, MPI_ANY_SOURCE, MPI_ANY_TAG, status, rank);
    switch (status.MPI_TAG) {
      case TAG_END:
        is_compute = false;
        break;

      case TAG_WANT_DOCTOR:
        receive_want_doctor(doctor_mutex[recv[1]], doctor_arr[recv[1]], recv,
                            status.MPI_SOURCE, rank);
        break;

      case TAG_ACK_DOCTOR:
        receive_ack_doctor();
        break;

      case TAG_RLS_DOCTOR:
        receive_rls_doctor();
        break;

      case TAG_WANT_SALON:
        receive_want_salon();
        break;

      case TAG_ACK_SALON:
        receive_ack_salon();
        break;

      case TAG_RLS_SALON:
        receive_rls_salon();
        break;

      default:
        printf("WRONG TAG %d", status.MPI_TAG);
        exit(EXIT_FAILURE);
    }
  }
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
