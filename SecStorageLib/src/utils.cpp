#include <cstdio>
#include <cstdlib>
#include "utils.h"

msghdr receive_msg_with_rights(Response* response, int incomingFd) {
    struct msghdr msgh;
    struct iovec iov;
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(int))];
        /* Space large enough to hold a ucred structure */
    } control_un;

    control_un.cmh.cmsg_len = CMSG_LEN(sizeof(int));
    control_un.cmh.cmsg_level = SOL_SOCKET;
    control_un.cmh.cmsg_type = SCM_CREDENTIALS;
    msgh.msg_control = control_un.control;
    msgh.msg_controllen = sizeof(control_un.control);
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = response;
    iov.iov_len = sizeof(Response);
    msgh.msg_name = NULL;
    msgh.msg_namelen = 0;


    if (recvmsg(incomingFd, &msgh, 0) <= 0)
    {

        perror("recvmsg");
        printf("ERROR recvmsg\n");
    }
    return msgh;
}

msghdr compose_msg_with_creds(Request* request) {
    // Permission data is filled by the kernel or program
#define AUTO_FILL_DATA

    struct msghdr msgh;
    struct iovec iov;
    int data = 0xbeef;
#ifndef AUTO_FILL_DATA
    union {
        struct cmsghdr cmh;
        char   control[CMSG_SPACE(sizeof(struct ucred))];
                        /* Space large enough to hold a ucred structure */
    } control_un;
    struct cmsghdr *cmhp;
    struct ucred *ucp;
#endif

    /* On Linux, we must transmit at least 1 byte of real data in
       order to send ancillary data */
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = request;
    iov.iov_len = sizeof(Request);
    msgh.msg_name = NULL;
    msgh.msg_namelen = 0;
#ifdef AUTO_FILL_DATA
    msgh.msg_control = NULL;
    msgh.msg_controllen = 0;
#else
    msgh.msg_control = control_un.control;
    msgh.msg_controllen = sizeof(control_un.control);
    cmhp = CMSG_FIRSTHDR(&msgh);
    cmhp->cmsg_len = CMSG_LEN(sizeof(struct ucred));
    cmhp->cmsg_level = SOL_SOCKET;
    cmhp->cmsg_type = SCM_CREDENTIALS;
    ucp = (struct ucred *) CMSG_DATA(cmhp);
    ucp->pid = getpid();
    ucp->uid = getuid();
    ucp->gid = getgid();
#endif
    return msgh;
}
