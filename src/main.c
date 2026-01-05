#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "http_parser.h"
#include "forwarder.h"
#include "filter.h"
#include "logger.h"
#include "connect_tunnel.h"

#define BACKLOG 64
#define BUFFER_SIZE 8192

typedef struct {
    int client_fd;
    char client_ip[INET_ADDRSTRLEN];
    int client_port;
} client_info;

// thread function 

static void *handle_client(void *arg) {
    client_info *info = (client_info *)arg;
    int client_fd = info->client_fd;

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        close(client_fd);
        free(info);
        return NULL;
    }

    buffer[bytes_read] = '\0';

    printf("---- Client Request ----\n%s\n------------------------\n", buffer);

    http_request req;
    if (parse_http_request(buffer, &req) != 0) {
        close(client_fd);
        free(info);
        return NULL;
    }

    printf("Parsed request:\n");
    printf("  Method: %s\n", req.method);
    printf("  Host:   %s\n", req.host);
    printf("  Port:   %d\n", req.port);
    printf("  Path:   %s\n", req.path);

    // filterning 

    if (is_blocked(req.host)) {
        log_block(info->client_ip,
                  info->client_port,
                  req.method,
                  req.host,
                  req.port,
                  req.path);

        const char *resp =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 9\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Forbidden";

        send(client_fd, resp, strlen(resp), 0);
        close(client_fd);
        free(info);
        return NULL;
    }
// forwarding 
    if (strcmp(req.method, "CONNECT") == 0) {
    handle_connect(client_fd, &req);
} else {
    forward_request(client_fd, buffer, &req);
}

// log allowed 
    log_access(info->client_ip,
               info->client_port,
               req.method,
               req.host,
               req.port,
               req.path,
               200);

    close(client_fd);
    free(info);
    return NULL;
}
// server

int start_server(const char *bind_addr, int port) {
    int server_fd;
    struct sockaddr_in addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(bind_addr);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }

    printf("Proxy listening on %s:%d\n", bind_addr, port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd,
                               (struct sockaddr *)&client_addr,
                               &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        client_info *info = malloc(sizeof(client_info));
        info->client_fd = client_fd;
        inet_ntop(AF_INET,
                  &client_addr.sin_addr,
                  info->client_ip,
                  sizeof(info->client_ip));
        info->client_port = ntohs(client_addr.sin_port);

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, info) != 0) {
            perror("pthread_create");
            close(client_fd);
            free(info);
        } else {
            pthread_detach(tid);
        }
    }

    close(server_fd);
    return 0;
}


int main(int argc, char *argv[]) {
    const char *bind_addr = "0.0.0.0";
    int port = 8888;

    load_blocklist("config/blocked_domains.txt");

    if (argc == 2) {
        port = atoi(argv[1]);
    }

    return start_server(bind_addr, port);
}
