#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "http_parser.h"

static void trim_crlf(char *s) {
    char *p = s;
    while (*p) {
        if (*p == '\r' || *p == '\n') {
            *p = '\0';
            return;
        }
        p++;
    }
}

int parse_http_request(const char *raw, http_request *req) {
    memset(req, 0, sizeof(*req));
    req->port = 80;

    char buffer[4096];
    strncpy(buffer, raw, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char *line = strtok(buffer, "\n");
    if (!line) return -1;

    trim_crlf(line);

    char uri[2048];
    if (sscanf(line, "%7s %2047s", req->method, uri) != 2)
        return -1;

    /* Absolute URI: http://host[:port]/path */
    
    if (strncmp(uri, "http://", 7) == 0) {
        char *host_start = uri + 7;
        char *path_start = strchr(host_start, '/');

        if (path_start) {
            strncpy(req->path, path_start, MAX_PATH - 1);
            *path_start = '\0';
        } else {
            strcpy(req->path, "/");
        }

        char *port_pos = strchr(host_start, ':');
        if (port_pos) {
            *port_pos = '\0';
            req->port = atoi(port_pos + 1);
        }

        strncpy(req->host, host_start, MAX_HOST - 1);
    }

    // Relative URI — get Host header 

    else {
        strncpy(req->path, uri, MAX_PATH - 1);

        char *h = strstr(raw, "\nHost:");
        if (!h) h = strstr(raw, "\nhost:");
        if (!h) return -1;

        h += 6;
        while (*h == ' ') h++;

        char hostbuf[256];
        sscanf(h, "%255s", hostbuf);

        char *port_pos = strchr(hostbuf, ':');
        if (port_pos) {
            *port_pos = '\0';
            req->port = atoi(port_pos + 1);
        }

        strncpy(req->host, hostbuf, MAX_HOST - 1);
    }

    return 0;
}
