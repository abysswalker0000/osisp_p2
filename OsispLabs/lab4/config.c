#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef NSIG
#define NSIG 32
#endif

void read_config(const char *filename, Config *config) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    config->count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL && config->count < MAX_SIGNALS) {
        int sig = atoi(line);
        if (sig > 0 && sig < NSIG) {
            config->signals[config->count++] = sig;
        }
    }
    fclose(file);
}