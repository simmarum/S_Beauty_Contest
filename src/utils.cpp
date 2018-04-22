#include "utils.h"

void send_end_compute(int &lclock, int myID, int sizePool) {
  myBroadCast(lclock, -1, -1, TAG_END, myID, sizePool);
}

void receive_want_doctor(std::vector<crit_sruct> &doct_vec, int recv[],
                         int from, int myID) {
  crit_sruct tmp_struct;
  tmp_struct.clock = recv[3];
  tmp_struct.proces_id = from;
  doct_vec.push_back(tmp_struct);
  sort_section(doct_vec);
};
void receive_ack_doctor(){};
void receive_rls_doctor(){};

void receive_want_salon(){};
void receive_ack_salon(){};
void receive_rls_salon(){};

void print_crit_section(std::vector<crit_sruct> &c_vec, int myID) {
  printf("---CRIT START--- (for %d process):\n", myID);
  for (size_t i = 0; i < c_vec.size(); i++) {
    printf("[%d] %zu: %d and %d\n", myID, i, c_vec[i].clock,
           c_vec[i].proces_id);
  }
  printf("---CRIT END---\n");
}