#ifndef CRIT_H
#define CRIT_H

#include <pthread.h>
#include <algorithm>
#include <vector>
#include "packet_communication.h"
#include "tags.h"

/*
 * Structure to handle proper order to critical section
 * clock - is lamport clock (first sort key)
 * proces_id - (second sort key)
 */
struct crit_sruct {
  int clock;
  int proces_id;
};

/*
 * Function to compare two vector by first value in structure and then by second
 */
bool compare_crit_struct(const crit_sruct &a, const crit_sruct &b);

/*
 * Function that do sort on vector in place
 * @p crit_vec - vector to sort
 */
void sort_section(std::vector<crit_sruct> &crit_vec);

/*
 * Function to broadcast everyone that process want access to critical section
 * and update proper queue
 * @p crit_mut - mutex to handle operation on crit_vec
 * @p crit_vec - vector with queue to proper critical section
 * @p lclock - lamport clock
 * @p message - message sent to other process (probably -1 for salon or doctor's
 * number)
 * @p tag - proper tag to determinate of operations
 * @p myID - ID of process that call this function
 * @p sizePool - number of all process in environment
 */
void want_crit_sec(pthread_mutex_t &crit_mut, std::vector<crit_sruct> &crit_vec,
                   int &lclock, int message, int tag, int myID, int sizePool);

/*
 * Function to broadcast everyone that process want access to critical section
 * and update proper queue
 * @p crit_mut - mutex to handle operation on crit_vec
 * @p crit_vec - vector with queue to proper critical section
 * @p lclock - lamport clock
 * @p message - message sent to other process (probably -1 for salon or doctor's
 * number)
 * @p tag - proper tag to determinate of operations
 * @p myID - ID of process that call this function
 * @p sizePool - number of all process in environment
 */
void rls_crit_sec(pthread_mutex_t &crit_mut, std::vector<crit_sruct> &crit_vec,
                  int &lclock, int message, int tag, int myID, int sizePool);

/*
 * Function to find position in queue to critical section
 * In 'position' will be on 0 index position of process with 'myID'
 * In 'position' will be on 1 index position of process with 'youID'
 * If process is not in queue, return INT_MAX
 * @p crit_mut - mutex to handle operation on crit_vec
 * @p crit_vec - vector with queue to critical section
 * @p myID - ID of process that call this function
 * @p youID - ID of process that send message to us (we want to know his
 * position to compare places in the queue)
 * @p position - array of two int
 */
void find_me_crit_sec(pthread_mutex_t &crit_mut,
                      std::vector<crit_sruct> &crit_vec, int myID, int youID,
                      int position[]);
/*
 * Function to find position in queue to critical section and remove first
 * occurrence
 * @p crit_mut - mutex to handle operation on crit_vec
 * @p crit_vec - vector with queue to critical section
 * @p myID - ID of process that call this function - first entry with this ID
 * will be remove
 */
void remove_from_crit_vec(std::vector<crit_sruct> &crit_vec, int myID);

/*
 * Function to print proper critical section with iformation about which process
 * it is It is not in one 'printf' so may be mixing with printf from other
 * threads
 * @p crit_mut - mutex to handle operation on crit_vec
 * @p c_vec - vector with queue to critical section
 * @p myID - ID of process that call this function - first entry with this ID
 * will be remove
 */
void print_crit_section(pthread_mutex_t &c_mut, std::vector<crit_sruct> &c_vec,
                        int myID);

#endif  // CRIT_H
