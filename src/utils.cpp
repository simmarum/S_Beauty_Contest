#include "utils.h"

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

void receive_rls_salon(pthread_mutex_t &crit_mut,
                       std::vector<crit_sruct> &sal_vec, int from, int myID,
                       int &sal_ack) {
  pthread_mutex_lock(&crit_mut);
  sal_ack++;
  remove_from_crit_vec(sal_vec, from);
  pthread_mutex_unlock(&crit_mut);
};
