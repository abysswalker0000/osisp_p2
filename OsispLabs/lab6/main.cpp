#include "functions.h"

int main(int argc, char *argv[]) {
    int start_ttl = 1;
    int max_hops = MAX_HOPS;
    std::string target_host;

    struct sockaddr_in dest{};

    if (argc == 1 || (argc == 2 && std::string(argv[1]) == "--help")) {
        display_help(argv[0]);
        return 0;
    }

    if (!process_args(argc, argv, start_ttl, max_hops, target_host)) {
        return 1;
    }

    struct in_addr dest_ip{};
    if (!lookup_host(target_host, dest_ip)) {
        return 1;
    }

    int sock = open_socket(IPPROTO_ICMP);

    dest.sin_family = AF_INET;
    dest.sin_addr = dest_ip;

    std::cout << "Tracing path to " << inet_ntoa(dest.sin_addr) << std::endl;

    perform_traceroute(sock, dest, start_ttl, max_hops);

    close(sock);
    return 0;
}   