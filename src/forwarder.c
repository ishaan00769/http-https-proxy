#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "forwarder.h"

#define BUFFER_SIZE 8192

int forward_request(int client_fd, const char *raw_request, const http_request *req) {
    int server_fd;
    struct sockaddr_in server_addr;
    struct hostent *he;

    he = gethostbyname(req->host);
    if (!he) {
        perror("gethostbyname");
        return -1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(req->port);
    memcpy(&server_addr.sin_addr, he->h_addr_list[0], he->h_length);

    if (connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(server_fd);
        return -1;
    }

    //Send request to remote server 

    send(server_fd, raw_request, strlen(raw_request), 0);

    // response back to client 
    
    char buffer[BUFFER_SIZE];
    ssize_t n;

    while ((n = recv(server_fd, buffer, sizeof(buffer), 0)) > 0) {
        send(client_fd, buffer, n, 0);
    }

    close(server_fd);
    return 0;
}
