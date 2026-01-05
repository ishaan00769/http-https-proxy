#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_BLOCKED 1024
#define MAX_HOSTLEN 256

static char blocked[MAX_BLOCKED][MAX_HOSTLEN];
static int blocked_count = 0;

static void normalize(char *s) {
    for (; *s; s++) {
        if (*s == '\n' || *s == '\r') {
            *s = '\0';
            break;
        }
        *s = tolower(*s);
    }
}

int load_blocklist(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen blocklist");
        return -1;
    }

    char line[MAX_HOSTLEN];
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#' || line[0] == '\n')
            continue;

        normalize(line);
        strncpy(blocked[blocked_count++], line, MAX_HOSTLEN - 1);

        if (blocked_count >= MAX_BLOCKED)
            break;
    }

    fclose(f);
    return 0;
}

int is_blocked(const char *host) {
    char temp[MAX_HOSTLEN];
    strncpy(temp, host, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    normalize(temp);

    for (int i = 0; i < blocked_count; i++) {
        if (strcmp(temp, blocked[i]) == 0)
            return 1;
    }
    return 0;
}
