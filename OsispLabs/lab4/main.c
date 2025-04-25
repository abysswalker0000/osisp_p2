#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "config.h"

#ifndef NSIG
#define NSIG 32
#endif

#define CONFIG_FILE "signals.conf"
#define LOG_FILE "/tmp/signals.log" 

volatile sig_atomic_t reload_config = 0;
volatile sig_atomic_t terminate = 0;
Config config;

void handle_signal(int sig) {

    FILE *log = fopen(LOG_FILE, "a");
    if (log != NULL) {
        fprintf(log, "Получен сигнал %d\n", sig);
        fclose(log);
    } else {
        perror("fopen signals.log");
    }

    if (sig == SIGHUP) {
        reload_config = 1;
    } else if (sig == SIGTERM) {
        terminate = 1;
    }
}

void setup_signals() {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    for (int i = 1; i < NSIG; i++) {
        if (i != SIGKILL && i != SIGSTOP) {
            sigaction(i, &sa, NULL);
        }
    }
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    if (setsid() < 0) exit(EXIT_FAILURE);
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);
    umask(0);
    chdir("/"); 
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    daemonize();
    read_config(CONFIG_FILE, &config);
    setup_signals();
    while (!terminate) {
        if (reload_config) {
            read_config(CONFIG_FILE, &config);
            reload_config = 0;
        }
        pause();
    }
    return 0;
}