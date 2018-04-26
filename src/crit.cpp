#include "crit.h"
#include "utils.h"

bool compare_crit_struct(const crit_sruct &a, const crit_sruct &b) {
  if (a.clock < b.clock) {
    return true;
  } else if (a.clock == b.clock) {
    if (a.proces_id < b.proces_id) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

void sort_section(std::vector<crit_sruct> &crit_vec) {
  std::sort(crit_vec.begin(), crit_vec.end(), compare_crit_struct);
}

void want_crit_sec(pthread_mutex_t &crit_mut, std::vector<crit_sruct> &crit_vec,
                   int &lclock, int message, int tag, int myID, int sizePool) {
  crit_sruct tmp_struct;
  tmp_struct.clock = lclock;
  tmp_struct.proces_id = myID;

  pthread_mutex_lock(&crit_mut);
  crit_vec.push_back(tmp_struct);
  sort_section(crit_vec);
  pthread_mutex_unlock(&crit_mut);

  myBroadCast(lclock, message, tmp_struct.clock, tag, myID, sizePool);
}

void rls_crit_sec(pthread_mutex_t &crit_mut, std::vector<crit_sruct> &crit_vec,
                  int &lclock, int message, int tag, int myID, int sizePool) {
  pthread_mutex_lock(&crit_mut);
  remove_from_crit_vec(crit_vec, myID);
  pthread_mutex_unlock(&crit_mut);

  myBroadCast(lclock, message, -1, tag, myID, sizePool);
}