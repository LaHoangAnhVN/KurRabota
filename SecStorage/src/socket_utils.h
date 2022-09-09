#ifndef ACCESSDAEMON_SOCKET_UTILS_H
#define ACCESSDAEMON_SOCKET_UTILS_H

#include <sys/socket.h>

#include "SecStorageLib/common.h"

msghdr compose_msg_with_fd(Response* response, int fd);



msghdr receive_msg_with_creds(int incomingFd);

int listen_socket(const char * socketPath, int socketQueueCapacity);

int accept_connection(int socketFd, sockaddr* peerAddr);

ucred get_creds_from_ancillary_data(msghdr* msg);

#endif //ACCESSDAEMON_SOCKET_UTILS_H
