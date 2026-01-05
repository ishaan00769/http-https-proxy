#ifndef CONNECT_TUNNEL_H
#define CONNECT_TUNNEL_H

#include "http_parser.h"

int handle_connect(int client_fd, const http_request *req);

#endif
