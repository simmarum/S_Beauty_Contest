#include "crit.h"

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

void want_crit_sec(std::vector<crit_sruct> &crit_vec, int &lclock, int message,
                   int tag, int myID, int sizePool) {
  crit_sruct tmp_struct;
  tmp_struct.clock = lclock;
  tmp_struct.proces_id = myID;

  myBroadCast(lclock, message, tmp_struct.clock, tag, myID, sizePool);

  crit_vec.push_back(tmp_struct);
  sort_section(crit_vec);
}