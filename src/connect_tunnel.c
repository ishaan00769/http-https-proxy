#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>

#include "connect_tunnel.h"

#define BUFFER_SIZE 8192

static int connect_remote(const char *host, int port) {
    struct hostent *he = gethostbyname(host);
    if (!he) return -1;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

int handle_connect(int client_fd, const http_request *req) {
    int server_fd = connect_remote(req->host, req->port);
    if (server_fd < 0)
        return -1;

    const char *ok =
        "HTTP/1.1 200 Connection Established\r\n"
        "Connection: close\r\n"
        "\r\n";

    send(client_fd, ok, strlen(ok), 0);

    fd_set fds;
    char buffer[BUFFER_SIZE];

    while (1) {
        FD_ZERO(&fds);
        FD_SET(client_fd, &fds);
        FD_SET(server_fd, &fds);

        int maxfd = (client_fd > server_fd ? client_fd : server_fd) + 1;
        if (select(maxfd, &fds, NULL, NULL, NULL) <= 0)
            break;

        if (FD_ISSET(client_fd, &fds)) {
            ssize_t n = recv(client_fd, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            send(server_fd, buffer, n, 0);
        }

        if (FD_ISSET(server_fd, &fds)) {
            ssize_t n = recv(server_fd, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            send(client_fd, buffer, n, 0);
        }
    }

    close(server_fd);
    return 0;
}
