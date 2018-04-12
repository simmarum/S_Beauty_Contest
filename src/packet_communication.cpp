#include "packet_communication.h"
#include <mpi.h>

int mySend(int &lclock, int message, int where, int tag) {
  packet_s send;
  lclock += 1;
  send.clock = lclock;
  send.message = message;
  MPI_Send(&send, 1, MPI_2INT, where, tag, MPI_COMM_WORLD);
}

int myRecv(packet_s &recv, int from, int tag, MPI_Status &status) {
  MPI_Recv(&recv, 1, MPI_2INT, from, tag, MPI_COMM_WORLD, &status);
}