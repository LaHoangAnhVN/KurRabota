#include <sys/socket.h>

#include "common.h"

msghdr receive_msg_with_rights(Response* response, int incomingFd);
msghdr compose_msg_with_creds(Request* request);