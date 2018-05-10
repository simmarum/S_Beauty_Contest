#ifndef PACKET_COMMUNICATION_H
#define PACKET_COMMUNICATION_H

#include <mpi.h>

extern bool raw_s_r;

/*
 * To send and receive is use array of three int [lamport_clock, message,
 * memory_clock]
 * @v lamport_clock - value of lamport clock to handle with time
 * @v message - message send and receive (probably -1 for salon section and
 * doctor's number for doctor section)
 * @v memory_clock - special variable use only
 * if process want critical section. In this variable is store lamport clock
 * when process demand critical section send that message to other (broadcast
 * increment lamport clock after every send)
 */

/*
 * Function to send whole message to other process with lamport clock, message
 * and memory_clock. Increment lamport clock.
 * @p lclock - lamport clock
 * @p message - message sent to other process (probably -1 for salon or doctor's
 * number)
 * @p mem_clock - lamport clock when process demand critical section
 * @p where - ID of process to send whole message
 * @p tag - proper tag to determinate of operations
 * @p myID - ID of process that call this function
 */
void mySend(int &lclock, int message, int mem_clock, int where, int tag,
            int myID);
/*
 * Function to receive whole message from other process with lamport clock,
 * message and memory_clock. Increment lamport clock.
 * @p lclock - lamport clock
 * @p recv - array of three int where will be store receive information
 * (lamport_clock, message, memory_clock)
 * @p from - ID of process which send whole message
 * @p tag - proper tag to determinate of operations
 * @p myID - ID of process that call this function
 */
void myRecv(int &lclock, int recv[], int from, int tag, MPI_Status &status,
            int myID);

/*
 * Function to send whole message to all process except sender with lamport
 * clock, message and memory_clock. Increment lamport clock.
 * @p lclock - lamport clock
 * @p message - message sent to other process (probably -1 for salon or doctor's
 * number)
 * @p mem_clock - lamport clock when process demand critical section
 * @p tag - proper tag to determinate of operations
 * @p myID - ID of process that call this function
 * @p sizePool - number of all process in environment
 */
void myBroadCast(int &lclock, int message, int mem_clock, int tag, int myID,
                 int sizePool);

#endif  // PACKET_COMMUNICATION_H
