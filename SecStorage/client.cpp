
#include <iostream>

#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#include "src/socket_utils.h"
#include "SecStorageLib/daemon_api.h"

int main() {
    sec_init();
    int fd =   sec_open("secret.txt", R_READ);
    char buf[256];
    read(fd, buf, 255);
    printf("%s", buf);
    sec_close();
    return 0;
}


//    const char* socketPath = "/home/socket2";
//    int socketFd = socket(AF_UNIX, SOCK_STREAM, 0);
//    sockaddr_un socketAddr, peerAddr;
//
//    memset(&socketAddr, 0, sizeof(socketAddr));
//
//    socketAddr.sun_family = AF_UNIX;
//    strcpy(socketAddr.sun_path, socketPath);
//
//    connect(socketFd, (sockaddr*) &socketAddr, sizeof(socketAddr));
//
//    Request request {
//        .request_type = Request::REQ_OPEN,
//        .target_uid = 1000,
//        .name = "secret.txt",
//        .rights = R_APPEND_EXT
//    };
//
//
//    auto _msg = compose_msg_with_creds(&request);
//
//    if (sendmsg(socketFd, &_msg, 0) < 0) {
//        perror("sendmsg: ");
//        exit(9);
//    }
//
//    const char* buf ="hello";
//
//    write(fd, buf, 5);
//
//    printf("%s", buf);
//
//    close(fd);
//    close(socketFd);
