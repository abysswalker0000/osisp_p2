#ifndef CONFIG_H
#define CONFIG_H

#include <signal.h>

#define MAX_SIGNALS 32

typedef struct {
    int signals[MAX_SIGNALS];
    int count;
} Config;

void read_config(const char *filename, Config *config);

#endif