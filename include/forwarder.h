#ifndef FORWARDER_H
#define FORWARDER_H

#include "http_parser.h"

int forward_request(int client_fd, const char *raw_request, const http_request *req);

#endif
