#include "daemon_api.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdio>

#include "common.h"
#include "utils.h"

#define SUCCESS_CODE 0
#define ERROR_CODE -1

static int socketFd = -1;

int sec_init(){
    socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    sockaddr_un socketAddr;

    memset(&socketAddr, 0, sizeof(socketAddr));

    socketAddr.sun_family = AF_UNIX;
    strcpy(socketAddr.sun_path, SOCKET_PATH);

    connect(socketFd, (sockaddr*) &socketAddr, sizeof(socketAddr));

    return SUCCESS_CODE;
}

int sec_close(){
    if (socketFd == -1)
        return ERROR_CODE;

    close(socketFd);
    socketFd = -1;
    return SUCCESS_CODE;
}

int sec_open(const char* name, rights_t mode){
    return sec_openat(int(getuid()), name, mode);
}

int sec_openat(int uid, const char* name, rights_t mode){
    Request request {
            .request_type = Request::REQ_OPEN,
            .target_uid = uid,
            .rights = mode
    };
    strcpy(request.name, name);

    auto req_msg = compose_msg_with_creds(&request);

    if (sendmsg(socketFd, &req_msg, 0) < 0) {
        perror("sendmsg: ");
    }

    Response resp{};
    msghdr resp_msg = receive_msg_with_rights(&resp, socketFd);

    if (!resp.success)
        return ERROR_CODE;

    struct cmsghdr  *cmsg;

    cmsg = CMSG_FIRSTHDR(&resp_msg);

    return *(int*) CMSG_DATA(cmsg);
}

int sec_unlink(const char* name){
    return sec_unlinkat(int(getuid()), name);
}

int sec_unlinkat(int uid, const char* name){
    Request request {
            .request_type = Request::REQ_UNLINK,
            .target_uid = uid
    };
    strcpy(request.name, name);

    auto req_msg = compose_msg_with_creds(&request);

    if (sendmsg(socketFd, &req_msg, 0) < 0) {
        perror("sendmsg: ");
    }

    Response resp{};
    msghdr resp_msg = receive_msg_with_rights(&resp, socketFd);

    if (!resp.success)
        return ERROR_CODE;

    struct cmsghdr  *cmsg;

    cmsg = CMSG_FIRSTHDR(&resp_msg);

    return SUCCESS_CODE;
}

int sec_grant(int uid, const char* file, rights_t rights){
    Request request {
            .request_type = Request::REQ_SET_RIGHTS,
            .target_uid = uid,
            .rights = rights
    };
    strcpy(request.name, file);

    auto req_msg = compose_msg_with_creds(&request);

    if (sendmsg(socketFd, &req_msg, 0) < 0) {
        perror("sendmsg: ");
    }

    Response resp{};
    msghdr resp_msg = receive_msg_with_rights(&resp, socketFd);

    if (!resp.success)
        return ERROR_CODE;

    struct cmsghdr  *cmsg;

    cmsg = CMSG_FIRSTHDR(&resp_msg);

    return SUCCESS_CODE;
}

int sec_rewoke(int uid, const char* file){
    return sec_grant(uid, file, R_NONE);
}

rights_t sec_check(int uid, const char* file){
    Request request {
            .request_type = Request::REQ_GET_RIGHTS,
            .target_uid = uid
    };
    strcpy(request.name, file);

    auto req_msg = compose_msg_with_creds(&request);

    if (sendmsg(socketFd, &req_msg, 0) < 0) {
        perror("sendmsg: ");
    }

    Response resp{};
    msghdr resp_msg = receive_msg_with_rights(&resp, socketFd);

    if (!resp.success)
        return R_NONE;

    struct cmsghdr  *cmsg;

    cmsg = CMSG_FIRSTHDR(&resp_msg);

    return resp.rights;
}
