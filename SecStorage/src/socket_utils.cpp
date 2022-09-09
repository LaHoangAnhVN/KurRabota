#include "socket_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

//#include "../SecStorageLib/include/common.h"

msghdr compose_msg_with_fd(Response* response, int fd) {
    // Permission data is filled by the kernel or program
//#define AUTO_FILL_DATA

    struct msghdr msgh;
    struct iovec iov;
#ifndef AUTO_FILL_DATA
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(int))];
        /* Space large enough to hold a ucred structure */
    } control_un;
    struct cmsghdr *cmhp;
    struct ucred *ucp;
#endif

    /* On Linux, we must transmit at least 1 byte of real data in
       order to send ancillary data */
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = response;
    iov.iov_len = sizeof(Response);
    msgh.msg_name = NULL;
    msgh.msg_namelen = 0;
#ifdef AUTO_FILL_DATA
    msgh.msg_control = NULL;
    msgh.msg_controllen = 0;
#else
    msgh.msg_control = control_un.control;
    msgh.msg_controllen = sizeof(control_un.control);
    cmhp = CMSG_FIRSTHDR(&msgh);
    cmhp->cmsg_len = CMSG_LEN(sizeof(int));
    cmhp->cmsg_level = SOL_SOCKET;
    cmhp->cmsg_type = SCM_RIGHTS;
    *( int *) CMSG_DATA(cmhp) = fd;
#endif
    return msgh;
}



msghdr receive_msg_with_creds(int incomingFd) {
    struct msghdr msgh;
    struct iovec iov;
    int data;
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(struct ucred))];
        /* Space large enough to hold a ucred structure */
    } control_un;

    control_un.cmh.cmsg_len = CMSG_LEN(sizeof(struct ucred));
    control_un.cmh.cmsg_level = SOL_SOCKET;
    control_un.cmh.cmsg_type = SCM_CREDENTIALS;
    msgh.msg_control = control_un.control;
    msgh.msg_controllen = sizeof(control_un.control);
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = calloc(1, sizeof(Request));
    iov.iov_len = sizeof(Request);
    msgh.msg_name = NULL;
    msgh.msg_namelen = 0;


    if (recvmsg(incomingFd, &msgh, 0) <= 0)
    {
        printf("ERROR recvmsg\n");
        perror("recvmsg");
    }
    return msgh;
}

int listen_socket(const char *socketPath, int socketQueueCapacity) {
    int socketFd = socket(AF_UNIX, SOCK_STREAM, 0);

    sockaddr_un socketAddr;
    memset(&socketAddr, 0, sizeof(socketAddr));

    socketAddr.sun_family = AF_UNIX;
    strcpy(socketAddr.sun_path, socketPath);

    if(bind(socketFd, (struct sockaddr *)&socketAddr, sizeof(socketAddr)) < 0)
    {
        perror("bind:");
        throw "Failed to bind socket";
    }

    listen(socketFd, socketQueueCapacity);

    return socketFd;
}

int accept_connection(int socketFd, sockaddr *peerAddr) {
    socklen_t peerAddrLen = sizeof(*peerAddr);
    int incomingFd = accept(socketFd, (sockaddr*) peerAddr, &peerAddrLen);
    if (incomingFd == -1) {
        perror("accept:");
        throw "Failed to accept connection";
    }

    int SO_PASSCRED_value = 1;
    if (setsockopt(incomingFd, SOL_SOCKET, SO_PASSCRED, &SO_PASSCRED_value, sizeof(SO_PASSCRED_value)) == -1)
        throw("setsockopt");

    return incomingFd;
}
