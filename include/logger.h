#ifndef LOGGER_H
#define LOGGER_H

void log_access(const char *client_ip,
                int client_port,
                const char *method,
                const char *host,
                int port,
                const char *path,
                int status);

void log_block(const char *client_ip,
               int client_port,
               const char *method,
               const char *host,
               int port,
               const char *path);

#endif
