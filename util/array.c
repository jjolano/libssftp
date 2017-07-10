#include "array.h"
#include "server/server.h"

array(struct pollfd, pollfd_);
array(ftpclient_event_connect, connect_);
array(ftpclient_event_disconnect, disconnect_);
