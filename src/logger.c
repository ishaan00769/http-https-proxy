#include <stdio.h>
#include <time.h>
#include "logger.h"

static void timestamp(char *buf, size_t len) {
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    strftime(buf, len, "%Y-%m-%d %H:%M:%S", tm);
}

void log_access(const char *client_ip,
                int client_port,
                const char *method,
                const char *host,
                int port,
                const char *path,
                int status) {
    FILE *f = fopen("logs/access.log", "a");
    if (!f) return;

    char ts[64];
    timestamp(ts, sizeof(ts));

    fprintf(f,
        "[%s] %s:%d \"%s %s\" %s:%d %d ALLOWED\n",
        ts,
        client_ip,
        client_port,
        method,
        path,
        host,
        port,
        status
    );

    fclose(f);
}

void log_block(const char *client_ip,
               int client_port,
               const char *method,
               const char *host,
               int port,
               const char *path) {
    FILE *f = fopen("logs/block.log", "a");
    if (!f) return;

    char ts[64];
    timestamp(ts, sizeof(ts));

    fprintf(f,
        "[%s] %s:%d \"%s %s\" %s:%d BLOCKED\n",
        ts,
        client_ip,
        client_port,
        method,
        path,
        host,
        port
    );

    fclose(f);
}
