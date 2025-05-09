#ifndef TRACEROUTE_FUNCTIONS_H
#define TRACEROUTE_FUNCTIONS_H

#include <arpa/inet.h>
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define PACKET_SIZE 4096
#define MAX_HOPS 30
#define TIMEOUT_SEC 1

bool check_ttl(const std::string& ttl);
int open_socket(int proto);
void display_help(const char *prog);
void perform_traceroute(int sock, const struct sockaddr_in& dest, int start_ttl, int max_hops);
bool process_args(int argc, char *argv[], int& start_ttl, int& max_hops, std::string& target_host);
bool lookup_host(const std::string& host, struct in_addr& ip);
unsigned short compute_checksum(void *data, int len);
long long get_microseconds();
