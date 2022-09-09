#ifndef ACCESSDAEMON_COMMON_H
#define ACCESSDAEMON_COMMON_H

extern const char* STORAGE_PATH;
extern const char* ACL_FILE_PATH;
extern const char* SOCKET_PATH;
extern int TEST_VAR;

enum rights_t { // https://docs.microsoft.com/ru-ru/dotnet/api/system.security.permissions.fileiopermissionaccess?view=dotnet-plat-ext-6.0&viewFallbackFrom=netcore-3.1#system-security-permissions-fileiopermissionaccess-write
    R_READ = 0, // "r"
    R_READ_EXT = 1, // "r+"
    R_WRITE = 2, // "w"  also defined delete right
    R_WRITE_EXT = 3, // "w+" also defined delete right
    R_APPEND = 4, // "a"
    R_APPEND_EXT = 5, // "a+"
    R_NONE = 6 // for unset rights
};

const int RIGHTS_COUNT = 6;

struct Request{
    enum Type{
        REQ_OPEN,
        REQ_UNLINK,
        REQ_GET_RIGHTS,
        REQ_SET_RIGHTS
    };

    Type request_type;
    int target_uid;
    char name[256];
    char modes[3];
    rights_t rights;
};

struct Response{
    bool success;
    char msg[256];
    rights_t rights;
};

rights_t rights_matcher(char* right);

const char* rights_matcher(rights_t rights);

#endif //ACCESSDAEMON_COMMON_H
