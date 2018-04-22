#ifndef PACKET_COMMUNICATION_H
#define PACKET_COMMUNICATION_H

#include <mpi.h>
extern int raw_s_r;
extern int csv;

// structure to send and receive is array of 3 int
// int send[3];
// int recv[3];

void mySend(int &lclock, int message, int mem_clock, int where, int tag,
            int myID);
void myRecv(int &lclock, int recv[], int from, int tag, MPI_Status &status,
            int myID);

void myBroadCast(int &lclock, int message, int mem_clock, int tag, int myID,
                 int sizePool);

#endif  // PACKET_COMMUNICATION_H
