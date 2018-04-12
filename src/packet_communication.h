#ifndef PACKET_COMMUNICATION_H
#define PACKET_COMMUNICATION_H

#include <mpi.h>

// Struct for send message
// DON'T change because it corespond to MPI_2INT which require struct with only
// 2 int!
struct packet_s {
  int clock;  // lamport clock
  int message;
};

int mySend(int &lclock, int message, int where, int tag);
int myRecv(packet_s &recv, int from, int tag, MPI_Status &status);

#endif  // PACKET_COMMUNICATION_H
