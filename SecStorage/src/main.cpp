#include <iostream>

#include <unistd.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "acl_utils.h"
#include "socket_utils.h"
#include "file_utils.h"

#include "SecStorageLib/common.h"

int main() {

    unlink(SOCKET_PATH);

    int socketFd = listen_socket(SOCKET_PATH, 5);

    sockaddr_un peerAddr;

    while (true) {
        int incomingFd = accept_connection (socketFd, (sockaddr*) &peerAddr);
        auto msgh = receive_msg_with_creds(incomingFd);

        // Got creds
        ucred ucredp;
        struct cmsghdr *cmhp = CMSG_FIRSTHDR(&msgh);
        memcpy(&ucredp, CMSG_DATA(cmhp), sizeof(ucredp));

        Request* request = (Request *)msgh.msg_iov->iov_base;
        printf("Received credentials pid=%d, uid=%d, gid=%d ,message %s\n",
               ucredp.pid, ucredp.uid, ucredp.gid, request->name);

        u_int user_UID = ucredp.uid;
        char working_dir[256] = {0};
        char file_path[256] = {0};

        rights_t urights;

        sprintf(working_dir, "%s/%u", STORAGE_PATH, request->target_uid);

        msghdr msg{};
        int file_descriptor;
        Response response;
        // Main flow
        switch(request->request_type) {
            case Request::REQ_OPEN:
                if (!dir_exist(working_dir)) {
                    mkdir(working_dir, 0700);
                }

                if (!check_permissions(request->target_uid, ucredp.uid, request->name, request->rights)) {
                    response.success = false;
                    sprintf(response.msg , "File '%s' cannot be opened with '%s' rights. User doesn't have enough rights", request->name, rights_matcher(request->rights));
                    break;
                }

                sprintf(file_path, "%s/%s", working_dir, request->name);

                file_descriptor = open_file(file_path, rights_matcher(request->rights));

                response.success = true;
                sprintf(response.msg , "Successfully opened file '%s' with '%s' rights", request->name, rights_matcher(request->rights));
                break;
            case Request::REQ_UNLINK:
                if (!dir_exist(working_dir)) {
                    response.success = false;
                    sprintf(response.msg , "File '%s' doesn't exist", request->name);
                    break;
                }

                if (!check_permissions(request->target_uid, ucredp.uid, request->name, R_WRITE)
                && !check_permissions(request->target_uid, ucredp.uid, request->name, R_WRITE_EXT)) {
                    response.success = false;
                    sprintf(response.msg , "User doesn't have 'w' or 'w+' rights for unlink '%s' file", request->name);
                    break;
                }


                sprintf(file_path, "%s/%s", working_dir, request->name);
                unlink(file_path);

                response.success = true;
                sprintf(response.msg , "Successfully unlink file '%s'", request->name);
                break;
            case Request::REQ_GET_RIGHTS:
                // TODO check file is exist or not

                for (int i=0 ; i < RIGHTS_COUNT; ++i) {
                    if(check_permissions(request->target_uid, ucredp.uid, request->name, rights_t(i))) {
                        urights = rights_t(i);
                        break;
                    }
                }

                break;
            case Request::REQ_SET_RIGHTS:

                // TODO: check file is exist

                set_permissions(request->target_uid, ucredp.uid, request->name, request->rights);
                break;
        }

        msg = compose_msg_with_fd(&response, file_descriptor);
        if (sendmsg(incomingFd, &msg, 0) < 0) {
            perror("sendmsg: ");
            exit(9);
        }
        close(file_descriptor);
        close(incomingFd);
    }

    close(socketFd);

    //free(msgh.msg_iov->iov_base);

    return 0;
}
