#ifndef ACCESSDAEMON_DAEMON_API_H
#define ACCESSDAEMON_DAEMON_API_H
#include "common.h"

int sec_init();
int sec_close();
int sec_open(const char* name, rights_t mode);
int sec_openat(int uid, const char* name, rights_t mode);
int sec_unlink(const char* name);
int sec_unlinkat(int uid, const char* name);
int sec_grant(int uid, const char* file, rights_t rights);
int sec_rewoke(int uid, const char* file);
rights_t sec_check(int uid, const char* file, rights_t rights);

#endif //ACCESSDAEMON_DAEMON_API_H
