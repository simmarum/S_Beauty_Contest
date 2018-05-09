#ifndef UTILS_H
#define UTILS_H

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
// TODO - add mutex!
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
// TODO - add mutex!
void print_crit_section(std::vector<crit_sruct> &c_vec, int myID);

#endif  // UTILS_H
