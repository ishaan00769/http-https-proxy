#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#define MAX_METHOD 8
#define MAX_HOST   256
#define MAX_PATH   2048

typedef struct {
    char method[MAX_METHOD];
    char host[MAX_HOST];
    int  port;
    char path[MAX_PATH];
} http_request;

int parse_http_request(const char *raw, http_request *req);

#endif
