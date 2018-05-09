#ifndef UTILS_H
#define UTILS_H

#include <limits.h>
#include <stdlib.h>
#include <vector>
#include "crit.h"
#include "packet_communication.h"
#include "tags.h"

/*
 * Function to broadcast end computation to other process (for synchronize to
 * start contest)
 * @p lclock - lamport clock
 * @p myID - ID of process that call this function
 * @p sizePool - number of all process in environment
 */
void send_end_compute(int &lclock, int myID, int sizePool);

/*
 * Function to handle receive with TAG_WANT_DOCTOR
 * Create new entry in queue to critical section with (mem_clock - for remember
 * when process call for critical section not when receive this message)
 * @p crit_mut - mutex to handle operation on doct_vec
 * @p doct_vec - vector with queue to proper doctor critical section
 * @p recv - array of three int where are lamport clock,message and mem_clock
 * @p from - ID of sender (from whom we got the message)
 * @p myID - ID of process that call this function
 */
void receive_want_doctor(pthread_mutex_t &crit_mut,
                         std::vector<crit_sruct> &doct_vec, int recv[],
                         int from, int myID);

/*
 * Function to handle receive with TAG_RLS_DOCTOR
 * Remove proper entry from queue to critical section and update status of
 * acknowledgement (increment variable)
 * @p crit_mut - mutex to handle operation on doct_vec
 * @p doct_vec - vector with queue to proper doctor critical section
 * @p from - ID of sender (from whom we got the message)
 * @p myID - ID of process that call this function
 * @p doct_ack - is variable to store how many acknowledgement we have got
 */
void receive_rls_doctor(pthread_mutex_t &crit_mut,
                        std::vector<crit_sruct> &doct_vec, int from, int myID,
                        int &doct_ack);
/*
 * Function to handle receive with TAG_WANT_SALON
 * Create new entry in queue to critical section with (mem_clock - for remember
 * when process call for critical section not when receive this message)
 * @p crit_mut - mutex to handle operation on sal_vec
 * @p sal_vec - vector with queue to salon critical section
 * @p recv - array of three int where are lamport clock,message and mem_clock
 * @p from - ID of sender (from whom we got the message)
 * @p myID - ID of process that call this function
 */
void receive_want_salon(pthread_mutex_t &crit_mut,
                        std::vector<crit_sruct> &sal_vec, int recv[], int from,
                        int myID);

/*
 * Function to handle receive with TAG_RLS_SALON
 * Remove proper entry from queue to critical section and update status of
 * acknowledgement (increment variable)
 * @p crit_mut - mutex to handle operation on sal_vec
 * @p sal_vec - vector with queue to salon critical section
 * @p from - ID of sender (from whom we got the message)
 * @p myID - ID of process that call this function
 * @p doct_ack - is variable to store how many acknowledgement we have got
 */
void receive_rls_salon(pthread_mutex_t &crit_mut,
                       std::vector<crit_sruct> &sal_vec, int from, int myID,
                       int &sal_ack);

#endif  // UTILS_H
