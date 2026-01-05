#ifndef FILTER_H
#define FILTER_H

int load_blocklist(const char *filename);
int is_blocked(const char *host);

#endif
