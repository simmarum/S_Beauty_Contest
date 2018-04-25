#ifndef TAGS_H
#define TAGS_H

#define TAG_END 999  // to end compute and exit process

#define TAG_WANT_DOCTOR 111
#define TAG_ACK_DOCTOR 122  // acknowledge
#define TAG_RLS_DOCTOR 133  // release

#define TAG_WANT_SALON 555
#define TAG_ACK_SALON 566
#define TAG_RLS_SALON 577

#include <map>
#include <string>
extern std::map<int, std::string> mymap;
extern int a;
#endif  // TAGS_H
