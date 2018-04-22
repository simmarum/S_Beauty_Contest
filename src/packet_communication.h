#ifndef PACKET_COMMUNICATION_H
#define PACKET_COMMUNICATION_H

#include <mpi.h>
extern int pretty;

// Struct for send message
// DON'T change because it corespond to MPI_2INT which require struct with only
// 2 int!
struct packet_s {
  int clock;  // lamport clock
  int message;
};

int mySend(int &lclock, int message, int where, int tag, int myID);
int myRecv(int &lclock, packet_s &recv, int from, int tag, MPI_Status &status,
           int myID);

int myBroadCast(int &lclock, int message, int tag, int myID, int sizePool);

#endif  // PACKET_COMMUNICATION_H
