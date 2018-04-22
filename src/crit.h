#ifndef CRIT_H
#define CRIT_H

#include <algorithm>
#include <vector>
#include "packet_communication.h"
#include "tags.h"

struct crit_sruct {
  int clock;
  int proces_id;
};

bool compare_crit_struct(const crit_sruct &a, const crit_sruct &b);
void sort_section(std::vector<crit_sruct> &crit_vec);
void want_crit_sec(std::vector<crit_sruct> &crit_vec, int &lclock, int message,
                   int tag, int myID, int sizePool);

#endif  // CRIT_H
