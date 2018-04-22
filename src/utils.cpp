#include "utils.h"

void send_end_compute(int &lclock, int myID, int sizePool) {
  myBroadCast(lclock, -1, TAG_END, myID, sizePool);
}

void receive_want_doctor(){};
void receive_ack_doctor(){};
void receive_rls_doctor(){};

void receive_want_salon(){};
void receive_ack_salon(){};
void receive_rls_salon(){};