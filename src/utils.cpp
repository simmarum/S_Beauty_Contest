#include "utils.h"
#include <limits.h>

void send_end_compute(int &lclock, int myID, int sizePool) {
  myBroadCast(lclock, -1, -1, TAG_END, myID, sizePool);
}

void receive_want_doctor(pthread_mutex_t &crit_mut,
                         std::vector<crit_sruct> &doct_vec, int recv[],
                         int from, int myID) {
  crit_sruct tmp_struct;
  tmp_struct.clock = recv[3];
  tmp_struct.proces_id = from;
  pthread_mutex_lock(&crit_mut);
  doct_vec.push_back(tmp_struct);
  sort_section(doct_vec);
  pthread_mutex_unlock(&crit_mut);
};
void receive_ack_doctor(){};
void receive_rls_doctor(pthread_mutex_t &crit_mut,
                        std::vector<crit_sruct> &doct_vec, int from, int myID,
                        int &doct_ack) {
  pthread_mutex_lock(&crit_mut);
  doct_ack++;
  remove_from_crit_vec(doct_vec, from);
  pthread_mutex_unlock(&crit_mut);
};

void receive_want_salon(pthread_mutex_t &crit_mut,
                        std::vector<crit_sruct> &sal_vec, int recv[], int from,
                        int myID) {
  crit_sruct tmp_struct;
  tmp_struct.clock = recv[3];
  tmp_struct.proces_id = from;
  pthread_mutex_lock(&crit_mut);
  sal_vec.push_back(tmp_struct);
  sort_section(sal_vec);
  pthread_mutex_unlock(&crit_mut);
};

void receive_ack_salon(){};
void receive_rls_salon(pthread_mutex_t &crit_mut,
                       std::vector<crit_sruct> &sal_vec, int from, int myID,
                       int &sal_ack) {
  pthread_mutex_lock(&crit_mut);
  sal_ack++;
  remove_from_crit_vec(sal_vec, from);
  pthread_mutex_unlock(&crit_mut);
};

int sum_table(int table[], int size) {
  int sum = 0;
  for (int i = 0; i < size; i++) {
    sum += table[i];
  }
  return sum;
}

void clear_table(int table[], int size) {
  for (int i = 0; i < size; i++) {
    table[i] = 0;
  }
}

void find_me_crit_sec(pthread_mutex_t &crit_mut,
                      std::vector<crit_sruct> &doct_vec, int myID, int youID,
                      int position[]) {
  position[0] = INT_MAX;
  position[1] = INT_MAX;
  pthread_mutex_lock(&crit_mut);
  for (size_t i = 0; i < doct_vec.size(); i++) {
    if (doct_vec[i].proces_id == myID) {
      position[0] = i;
    }
    if (doct_vec[i].proces_id == youID) {
      position[1] = i;
    }
    pthread_mutex_unlock(&crit_mut);
  }
}

void remove_from_crit_vec(std::vector<crit_sruct> &crit_vec, int myID) {
  for (size_t i = 0; i < crit_vec.size(); i++) {
    if (crit_vec[i].proces_id == myID) {
      crit_vec.erase(crit_vec.begin() + i);
      break;
    }
  }
}
void print_crit_section(std::vector<crit_sruct> &c_vec, int myID) {
  printf("---CRIT START--- (for %d process):\n", myID);
  for (size_t i = 0; i < c_vec.size(); i++) {
    printf("[%d] %zu: %d and %d\n", myID, i, c_vec[i].clock,
           c_vec[i].proces_id);
  }
  printf("---CRIT END---\n");
}