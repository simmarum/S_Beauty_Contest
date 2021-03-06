
#include "packet_communication.h"
#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include "tags.h"

extern pthread_mutex_t l_clock_mutex;
pthread_mutex_t send_clock_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recv_clock_mutex = PTHREAD_MUTEX_INITIALIZER;

void mySend(int &lclock, int message, int mem_clock, int where, int tag,
            int myID) {
  int send[3];
  pthread_mutex_lock(&send_clock_mutex);
  pthread_mutex_lock(&l_clock_mutex);
  lclock += 1;
  send[0] = lclock;
  send[1] = message;
  send[2] = mem_clock;
  MPI_Send(&send, 3, MPI_INT, where, tag, MPI_COMM_WORLD);
  if (raw_s_r) {
    printf("[%09d:%03d] Send '%d' and '%d' to process %d with tag %d [%s]\n",
           send[0], myID, send[1], send[2], where, tag, mymap[tag].c_str());
  }

  pthread_mutex_unlock(&l_clock_mutex);
  pthread_mutex_unlock(&send_clock_mutex);
}

void myRecv(int &lclock, int recv[], int from, int tag, MPI_Status &status,
            int myID) {
  MPI_Recv(recv, 3, MPI_INT, from, tag, MPI_COMM_WORLD, &status);
  pthread_mutex_lock(&recv_clock_mutex);
  pthread_mutex_lock(&l_clock_mutex);
  if (lclock < recv[0]) {
    lclock = recv[0] + 1;
  } else {
    lclock = lclock + 1;
  }
  if (raw_s_r) {
    printf(
        "[%09d:%03d] Receive '%d' and '%d' from process %d with tag %d [%s]\n",
        lclock, myID, recv[1], recv[2], status.MPI_SOURCE, status.MPI_TAG,
        mymap[status.MPI_TAG].c_str());
  }
  pthread_mutex_unlock(&l_clock_mutex);
  pthread_mutex_unlock(&recv_clock_mutex);
}

void myBroadCast(int &lclock, int message, int mem_clock, int tag, int myID,
                 int sizePool) {
  for (int i = 0; i < sizePool; i++) {
    if (i != myID) {
      mySend(lclock, message, mem_clock, i, tag, myID);
    }
  }
}
