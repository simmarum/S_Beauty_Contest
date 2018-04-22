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

pthread_mutex_t send_clock_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recv_clock_mutex = PTHREAD_MUTEX_INITIALIZER;

char filename[40];
struct tm *timenow;
time_t now = time(NULL);

int mySend(int &lclock, int message, int where, int tag, int myID) {
  packet_s send;
  pthread_mutex_lock(&send_clock_mutex);
  lclock += 1;
  send.clock = lclock;
  pthread_mutex_unlock(&send_clock_mutex);
  send.message = message;
  MPI_Send(&send, 1, MPI_2INT, where, tag, MPI_COMM_WORLD);
  if (pretty) {
    printf("[%d:%d] Send '%d' to process %d with tag %d\n", send.clock, myID,
           send.message, where, tag);
  }
  if (csv) {
    FILE *fp;
    sprintf(filename, "log/log_%d.csv", (int)now);
    if ((fp = fopen(filename, "a")) == NULL) {
      printf("Can't open output file %s!\n", filename);
      exit(EXIT_FAILURE);
    } else {
      fprintf(fp, "%d,%d,%c,%d,%d,%d\n", send.clock, myID, 'S', send.message,
              where, tag);
    }
  }
}

int myRecv(int &lclock, packet_s &recv, int from, int tag, MPI_Status &status,
           int myID) {
  MPI_Recv(&recv, 1, MPI_2INT, from, tag, MPI_COMM_WORLD, &status);
  pthread_mutex_lock(&recv_clock_mutex);
  if (lclock < recv.clock) {
    lclock = recv.clock + 1;
  } else {
    lclock = lclock + 1;
  }
  if (pretty) {
    printf("[%d:%d] Receive '%d' from process %d with tag %d\n", lclock, myID,
           recv.message, status.MPI_SOURCE, status.MPI_TAG);
  }
  if (csv) {
    FILE *fp;
    sprintf(filename, "log/log_%d.csv", (int)now);
    if ((fp = fopen(filename, "a")) == NULL) {
      printf("Can't open output file %s!\n", filename);
      exit(EXIT_FAILURE);
    } else {
      fprintf(fp, "%d,%d,%c,%d,%d,%d\n", lclock, myID, 'R', recv.message,
              status.MPI_SOURCE, status.MPI_TAG);
    }
  }
  pthread_mutex_unlock(&recv_clock_mutex);
}

int myBroadCast(int &lclock, int message, int tag, int myID, int sizePool) {
  for (int i = 0; i < sizePool; i++) {
    if (i != myID) {
      mySend(lclock, message, i, tag, myID);
    }
  }
}