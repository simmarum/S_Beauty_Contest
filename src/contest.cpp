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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <atomic>
#include <vector>
#include "crit.h"
#include "error_code.h"
#include "packet_communication.h"
#include "tags.h"
#include "utils.h"

int raw_s_r = true;
int csv = false;
int debug = false;

pthread_mutex_t l_clock_mutex = PTHREAD_MUTEX_INITIALIZER;
extern pthread_mutex_t send_clock_mutex;
extern pthread_mutex_t recv_clock_mutex;

// function definitions
bool cli_parameters(int argc, char *argv[], int &L, int &S);
void enable_thread(int *argc, char ***argv);

void *receive_loop(void *ptr);
void *ack_doctor_fun(void *args);

// global variable
int rank = 0;    // id of this thread
int lclock = 0;  // lamport clock
int size = 0;    // number of processes

int end_ack;
std::atomic<bool> isEndCompute(false);

std::atomic<bool> isDoctorFree(false);
int doctor_ack;
std::vector<crit_sruct> *doctor_arr;
std::vector<pthread_mutex_t> doctor_mutex;
int *doctor_ack_tab;

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

  M = 1 + rand() / (RAND_MAX / (S - 1 + 1) + 1);

  // create doctor critical section mutex and ack array
  doctor_ack = 0;
  doctor_ack_tab = new int[size];
  doctor_arr = new std::vector<crit_sruct>[size];
  for (int i = 0; i < size; i++) {
    doctor_ack_tab[i] = i;
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

  while (!isDoctorFree) {
  }
  // critical section
  int good_models = 1 + rand() / (RAND_MAX / (M - 1 + 1) + 1);
  pthread_mutex_lock(&l_clock_mutex);
  printf("[%d:%d] GOOD MODELS: %d/%d from %d doctor\n", lclock, rank,
         good_models, M, which_doctor);
  pthread_mutex_unlock(&l_clock_mutex);
  doctor_ack = 0;

  // rls docor
  rls_crit_sec(doctor_mutex[which_doctor], doctor_arr[which_doctor], lclock,
               which_doctor, TAG_RLS_DOCTOR, rank, size);

  // send end compute and exit process
  send_end_compute(lclock, rank, size);

  while (!isEndCompute) {
  }
  printf("GET END ACK FROM OTHERS - START CONTEST FROM %d\n", rank);
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

    int position[2];

    switch (status.MPI_TAG) {
      case TAG_END:
        end_ack++;
        if (end_ack == size - 1) {
          isEndCompute = true;
          is_compute = false;
        }
        break;

      case TAG_WANT_DOCTOR:
        receive_want_doctor(doctor_mutex[recv[1]], doctor_arr[recv[1]], recv,
                            status.MPI_SOURCE, rank);
        find_me_crit_sec(doctor_mutex[recv[1]], doctor_arr[recv[1]], rank,
                         status.MPI_SOURCE, position);

        if (position[0] > position[1]) {
          pthread_t ack_doctor_thread;
          pthread_create(&ack_doctor_thread, NULL, ack_doctor_fun,
                         &doctor_ack_tab[status.MPI_SOURCE]);
        }
        break;

      case TAG_ACK_DOCTOR:
        doctor_ack++;
        if (doctor_ack == size - 1) {
          isDoctorFree = true;
        }
        break;

      case TAG_RLS_DOCTOR:

        receive_rls_doctor(doctor_mutex[recv[1]], doctor_arr[recv[1]],
                           status.MPI_SOURCE, rank, doctor_ack);
        if (!isDoctorFree) {
          if (doctor_ack == size - 1) {
            isDoctorFree = true;
          }
        }
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

void *ack_doctor_fun(void *args) {
  int to = *(int *)args;
  mySend(lclock, -1, -1, to, TAG_ACK_DOCTOR, rank);
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
