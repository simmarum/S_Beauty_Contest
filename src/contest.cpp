/* --------------------------------*/
/* Beauty contest                  */
/* Authors:                        */
/*         Mateusz Kruszyna        */
/*         Mikołaj Stankowiak      */
/* --------------------------------*/

#include <math.h>
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

// #######################################################
// ########## FUNCTION DEFINITION ########################
// #######################################################
/*
 * Check program arguments for correctnes
 * @p argc - number of argument
 * @p argv - array of parameters
 * @p L - number of doctors
 * @p S - capacity of salon
 */
bool cli_parameters(int argc, char *argv[], int &L, int &S);

/*
 * Function to enable multithreading in MPI (copy paste from stackoverflow)
 * @p argc - number of argument
 * @p argv - array of parameters
 */
void enable_thread(int *argc, char ***argv);

/*
 * Function to receive message from others processes without impact on main
 * thread. It is run as new thread.
 */
void *receive_loop(void *ptr);
/*
 * Special function to send doctors ack after receive message (new thread to not
 * nest RECEIVE and SEND). It is run as new thread.
 */
void *ack_doctor_fun(void *args);
/*
 * Special function to send salon ack after receive message (new thread to not
 * nest RECEIVE and SEND). It is run as new thread.
 */
void *ack_salon_fun(void *args);

// #######################################################
// ########## GLOBAL VARIABLE ############################
// #######################################################
bool raw_s_r = true;  // printf message on send and receive
bool debug = false;   // to avoid unnecessary print
bool p_print = true;  // print useful information about section

pthread_mutex_t l_clock_mutex = PTHREAD_MUTEX_INITIALIZER;
extern pthread_mutex_t send_clock_mutex;
extern pthread_mutex_t recv_clock_mutex;

int rank = 0;    // id of this thread
int lclock = 0;  // lamport clock
int size = 0;    // number of processes
int k = 0;  // number of group to salon (how many manager may enter to salon
            // (not S because sometimes manager may enter with 2 or 3 models in
            // one entry))

std::atomic<bool> isThreadSend(
    false);  // to prevent send from thread after finalize
std::atomic<bool> isReceiveWait(false);  

std::atomic<int> end_ack(0);            // how many ack for end compute receive
std::atomic<bool> isEndCompute(false);  // condition for end compute
std::atomic<bool> is_compute(true);     // condition for end receive thread

std::atomic<bool> isDoctorFree(
    false);      // condition for doctor's critical section
int doctor_ack;  // how many ack for doctor's critical section receive
std::vector<crit_sruct>
    *doctor_arr;  // array of queues for critical section for doctor
std::vector<pthread_mutex_t> doctor_mutex;  // mutex for queue above
int *doctor_ack_tab;  // array of int with process id for additional send in
                      // receive loop

std::atomic<bool> isSalonFree(false);  // condition for salon critical section
int salon_ack;  // how many ack for salon critical section receive
std::vector<crit_sruct> salon_vec;  // queue for salon critical section
pthread_mutex_t salon_mutex =
    PTHREAD_MUTEX_INITIALIZER;  // mutex fot queue above
int *salon_ack_tab;  // array of int with process id for additional send in
                     // receive loop

// #######################################################
// ########## FUNCTION IMPLEMENTATION ####################
// #######################################################
int main(int argc, char *argv[]) {
  // disable buffering on stdout
  setbuf(stdout, NULL);

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

  // get process id and number of all process
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* get current process id */
  MPI_Comm_size(MPI_COMM_WORLD, &size); /* get number of processes */

  // random seed depends on process id
  srand(rank + (int)time(0));

  // rand from 1 to capacity of Salon
  M = 1 + rand() / (RAND_MAX / (S - 1 + 1) + 1);

  // initialize variable for critical sections
  doctor_ack = 0;
  salon_ack = 0;
  doctor_ack_tab = new int[size];
  salon_ack_tab = new int[size];
  doctor_arr = new std::vector<crit_sruct>[size];
  for (int i = 0; i < size; i++) {
    doctor_ack_tab[i] = i;
    salon_ack_tab[i] = i;
    pthread_mutex_t tmp_mutex = PTHREAD_MUTEX_INITIALIZER;
    doctor_mutex.push_back(tmp_mutex);
  }

  // synchronize
  MPI_Barrier(MPI_COMM_WORLD);

  /// starts proper compute

  // evenly allocate doctors
  int which_doctor = rank % L;

  // want critical section for doctor
  want_crit_sec(doctor_mutex[which_doctor], doctor_arr[which_doctor], lclock,
                which_doctor, TAG_WANT_DOCTOR, rank, size);

  while (!isDoctorFree) {
    // check for critical section
  }
  // now in critical section

  // get good model from doctor
  int good_models = 1 + rand() / (RAND_MAX / (M - 1 + 1) + 1);

  // print information about visit doctor
  pthread_mutex_lock(&l_clock_mutex);
  printf("[%d:%d] GOOD MODELS: %d/%d from %d doctor\n", lclock, rank,
         good_models, M, which_doctor);
  pthread_mutex_unlock(&l_clock_mutex);

  // clear doctor ack
  doctor_ack = 0;

  // rls doctor
  rls_crit_sec(doctor_mutex[which_doctor], doctor_arr[which_doctor], lclock,
               which_doctor, TAG_RLS_DOCTOR, rank, size);

  // salon section

  // count how many models was in salon
  int how_many_models_was_in_salon = 0;

  // compute k fractor
  int x = (int)std::ceil((float)S / (float)size);
  k = (int)std::floor(S / x);

  // as long as is good_models
  while (how_many_models_was_in_salon < good_models) {
    // clear salon ack counter
    salon_ack = 0;

    want_crit_sec(salon_mutex, salon_vec, lclock, -1, TAG_WANT_SALON, rank,
                  size);
    if (size <= k) {  // case when number of manager is smaller or equal than
                      // compute capacity of salon (no need to check ACK from
                      // other because for all will be place in salon)
      isSalonFree = true;
    }

    while (!isSalonFree) {
      // wait for critical section
    }

    // now in critical section
    how_many_models_was_in_salon += x;  // x models was in salon
    if (how_many_models_was_in_salon > good_models)
      how_many_models_was_in_salon = good_models;  // edge case with overload

    // print information about visit in salon
    pthread_mutex_lock(&l_clock_mutex);
    printf("[%d:%d] SO FAR %d/%d WAS IN SALON\n", lclock, rank,
           how_many_models_was_in_salon, good_models);
    pthread_mutex_unlock(&l_clock_mutex);

    // rls salon
    isSalonFree = false;
    rls_crit_sec(salon_mutex, salon_vec, lclock, -1, TAG_RLS_SALON, rank, size);
  }

  // clear salon ack
  salon_ack = 0;

  // send end compute and exit process
  send_end_compute(lclock, rank, size);
  while (!isEndCompute) {
    // wait for other process
  }

  // print information that everyone is done his work and start contest
  printf("GET ALL END ACK FROM OTHERS - START CONTEST FROM %d\n", rank);

  while(!isReceiveWait){
    // wait until receive loop is inside while loop
  }
  usleep(500000); // for wait after all message will be delivered
  // clean up after work
  is_compute = false;
  mySend(lclock, -1, -1, rank, TAG_END, rank);
  pthread_join(receive_thread, NULL); 

  while (isThreadSend) {
    // check if some thread is in send mode
  }
  MPI_Finalize();

  return EXIT_SUCCESS;
}

void *receive_loop(void *ptr) {
  while (is_compute) {
    isReceiveWait = true;
    MPI_Status status;
    int recv[3];
    myRecv(lclock, recv, MPI_ANY_SOURCE, MPI_ANY_TAG, status, rank);
    isReceiveWait = false;
    int position[2];

    switch (status.MPI_TAG) {
      case TAG_END:
        end_ack++;
        if (end_ack == size - 1) {
          isEndCompute = true;
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

        if (p_print) {
          pthread_mutex_lock(&l_clock_mutex);
          printf("[%d:%d] %d/%d ACK FOR DOCTOR (ACK)\n", lclock, rank,
                 doctor_ack, size - 1);
          pthread_mutex_unlock(&l_clock_mutex);
        }
        if (doctor_ack == size - 1) {
          isDoctorFree = true;
        }
        break;

      case TAG_RLS_DOCTOR:
        receive_rls_doctor(doctor_mutex[recv[1]], doctor_arr[recv[1]],
                           status.MPI_SOURCE, rank, doctor_ack);
        if (p_print) {
          pthread_mutex_lock(&l_clock_mutex);
          printf("[%d:%d] %d/%d ACK FOR DOCTOR (RLS)\n", lclock, rank,
                 doctor_ack, size - 1);
          pthread_mutex_unlock(&l_clock_mutex);
        }
        if (!isDoctorFree) {
          if (doctor_ack == size - 1) {
            isDoctorFree = true;
          }
        }
        break;

      case TAG_WANT_SALON:
        receive_want_salon(salon_mutex, salon_vec, recv, status.MPI_SOURCE,
                           rank);
        find_me_crit_sec(salon_mutex, salon_vec, rank, status.MPI_SOURCE,
                         position);
        if (position[0] > position[1] && position[0] >= k) {
          pthread_t ack_salon_thread;
          int tmp_to = status.MPI_SOURCE;
          pthread_create(&ack_salon_thread, NULL, ack_salon_fun,
                         &salon_ack_tab[tmp_to]);
        }
        break;

      case TAG_ACK_SALON:
        salon_ack++;
        if (p_print) {
          pthread_mutex_lock(&l_clock_mutex);
          printf("[%d:%d] %d/%d ACK FOR SALON (ACK)\n", lclock, rank, salon_ack,
                 size - k);
          pthread_mutex_unlock(&l_clock_mutex);
        }
        if (salon_ack == size - k) {
          isSalonFree = true;
        }
        break;

      case TAG_RLS_SALON:
        receive_rls_salon(salon_mutex, salon_vec, status.MPI_SOURCE, rank,
                          salon_ack);
        if (p_print) {
          pthread_mutex_lock(&l_clock_mutex);
          printf("[%d:%d] %d/%d ACK FOR SALON (RLS)\n", lclock, rank, salon_ack,
                 size - k);
          pthread_mutex_unlock(&l_clock_mutex);
        }
        if (!isSalonFree) {
          if (salon_ack == size - k) {
            isSalonFree = true;
          }
        }
        break;

      default:
        printf("WRONG TAG %d", status.MPI_TAG);
        exit(EXIT_FAILURE);
    }
  }
  return EXIT_SUCCESS;
}

void *ack_doctor_fun(void *args) {
  isThreadSend = true;
  int to = *(int *)args;
  if (!isEndCompute) {
    mySend(lclock, -1, -1, to, TAG_ACK_DOCTOR, rank);
  }
  isThreadSend = false;
  return EXIT_SUCCESS;
}

void *ack_salon_fun(void *args) {
  isThreadSend = true;
  int to = *(int *)args;
  if (!isEndCompute) {
    mySend(lclock, -1, -1, to, TAG_ACK_SALON, rank);
  }
  isThreadSend = false;
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
