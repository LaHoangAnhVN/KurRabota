#ifndef ACCESSDAEMON_ACL_UTILS_H
#define ACCESSDAEMON_ACL_UTILS_H

#include "socket_utils.h"

bool delete_line_from_acl(char* acl_file, int line_num);
char* generate_acl_line(int uid, char* file, rights_t rights);
void add_line_to_acl(char* acl_file, char* line);
bool set_permissions(int target_uid, int current_uid, char* path, rights_t right);
bool check_permissions(int target_uid, int current_uid, char* path, rights_t right);

#endif //ACCESSDAEMON_ACL_UTILS_H
