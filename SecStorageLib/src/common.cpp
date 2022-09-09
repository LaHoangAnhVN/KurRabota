#include "../include/common.h"

#include <string.h>

const char* STORAGE_PATH  = "/home/USER_NICKNAME/storage";
const char* ACL_FILE_PATH = "/home/USER_NICKNAME/storage/acl";
const char* SOCKET_PATH = "/home/USER_NICKNAME/socket2";
int TEST_VAR;

rights_t rights_matcher(char *right) {
    rights_t r;
    if (strstr(right, "r") != NULL) {
        r = R_READ;
    }
    else if (strstr(right, "w") != NULL) {
        r = R_WRITE;
    }
    else if (strstr(right, "a") != NULL) {
        r = R_APPEND;
    }

    if (strstr(right, "+") != NULL) {
        r  = rights_t(r + 1);
    }
    return r;
}

const char *rights_matcher(rights_t rights) {
    switch (rights) {
        case R_READ:
            return "r";
        case R_READ_EXT:
            return "r+";
        case R_WRITE:
            return "w";
        case R_WRITE_EXT:
            return "w+";
        case R_APPEND:
            return "a";
        case R_APPEND_EXT:
            return "a+";
    }
    return "";
}
