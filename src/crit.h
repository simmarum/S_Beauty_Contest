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
#endif  // CRIT_H
