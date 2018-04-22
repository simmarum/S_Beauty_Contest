#ifndef UTILS_H
#define UTILS_H

#include "packet_communication.h"
#include "tags.h"

void send_end_compute(int &lclock, int myID, int sizePool);

void receive_want_doctor();
void receive_ack_doctor();
void receive_rls_doctor();

void receive_want_salon();
void receive_ack_salon();
void receive_rls_salon();
#endif  // UTILS_H
