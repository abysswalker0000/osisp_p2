#include <netinet/tcp.h>
#include "functions.h"

bool check_ttl(const std::string& ttl) {
    try {
        size_t idx;
        int val = std::stoi(ttl, &idx);
        return idx == ttl.size() && val >= 1 && val <= 255;
    } catch (const std::invalid_argument&) {
        return false;
    }
}

void display_help(const char *prog) {
    std::cerr << "Usage: sudo " << prog << " [-f first_ttl] [-m max_ttl] <target_host>\n"
              << "Options:\n"
              << "  -f, --first-ttl=VALUE  Begin from this TTL (default: 1)\n"
              << "  -m, --max-ttl=VALUE    Maximum hops to probe (default: 30)\n"
              << "  -h, --help             Show this help message\n";
}

int open_socket(int proto) {
    int sock = socket(AF_INET, SOCK_RAW, proto);
    if (sock < 0) {
        exit(EXIT_FAILURE);
    }
    return sock;
}

bool process_args(int argc, char *argv[], int& start_ttl, int& max_hops, std::string& target_host) {
    struct option opts[] = {
        {"first-ttl", required_argument, nullptr, 'f'},
        {"max-ttl", required_argument, nullptr, 'm'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };

    int c;
    while ((c = getopt_long(argc, argv, "f:m:h", opts, nullptr)) != -1) {
        switch (c) {
            case 'f':
                if (!check_ttl(optarg)) {
                    std::cerr << "First TTL must be between 1 and 255.\n";
                    return false;
                }
                start_ttl = std::stoi(optarg);
                break;
            case 'm':
                if (!check_ttl(optarg)) {
                    std::cerr << "Max TTL must be between 1 and 255.\n";
                    return false;
                }
                max_hops = std::stoi(optarg);
                break;
            case 'h':
                display_help(argv[0]);
                return false;
            case '?':
                std::cerr << "Invalid option: '" << static_cast<char>(optopt) << "'.\n";
                display_help(argv[0]);
                return false;
            default:
                abort();
        }
    }

    if (optind >= argc) {
        std::cerr << "Target host required.\n";
        display_help(argv[0]);
        return false;
    }

    target_host = argv[optind];
    if (max_hops < start_ttl) {
        std::cerr << "Max TTL must be at least first TTL.\n";
        return false;
    }

    return true;
}

bool lookup_host(const std::string& host, struct in_addr& ip) {
    struct hostent *entry = gethostbyname(host.c_str());
    if (!entry) {
        std::cerr << (h_errno == TRY_AGAIN ? "Temporary DNS failure. Try again.\n" : "Failed to resolve host.\n");
        return false;
    }
    std::memcpy(&ip, entry->h_addr_list[0], sizeof(struct in_addr));
    return true;
}

unsigned short compute_checksum(void *data, int len) {
    unsigned short *buf = reinterpret_cast<unsigned short *>(data);
    unsigned int sum = 0;

    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }

    if (len == 1) {
        sum += *(reinterpret_cast<unsigned char *>(buf));
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return ~sum;
}

long long get_microseconds() {
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) == -1) {
        perror("gettimeofday");
        return -1;
    }
    return static_cast<long long>(tv.tv_sec) * 1000000LL + tv.tv_usec;
}

void perform_traceroute(int sock, const struct sockaddr_in& dest, int start_ttl, int max_hops) {
    fd_set readfds;
    struct timeval wait;
    struct icmphdr icmp{};
    char buffer[PACKET_SIZE];
    bool done = false;

    for (int ttl = start_ttl; ttl <= max_hops && !done; ++ttl) {
        std::cout << ttl << " ";
        for (int attempt = 0; attempt < 3; ++attempt) {
            if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
                perror("setsockopt");
                return;
            }

            long long sent = get_microseconds();
            if (sent == -1) {
                return;
            }

            icmp.type = ICMP_ECHO;
            icmp.code = 0;
            icmp.un.echo.id = getpid();
            icmp.un.echo.sequence = ttl;
            icmp.checksum = compute_checksum(&icmp, sizeof(icmp));

            std::memcpy(buffer, &icmp, sizeof(icmp));

            if (sendto(sock, buffer, sizeof(icmp), 0, reinterpret_cast<const struct sockaddr *>(&dest), sizeof(dest)) <= 0) {
                perror("sendto");
                return;
            }

            FD_ZERO(&readfds);
            FD_SET(sock, &readfds);
            wait.tv_sec = TIMEOUT_SEC;
            wait.tv_usec = 0;

            if (select(sock + 1, &readfds, nullptr, nullptr, &wait) > 0) {
                char response[PACKET_SIZE];
                struct sockaddr_in src{};
                socklen_t len = sizeof(src);
                ssize_t received = recvfrom(sock, response, sizeof(response), 0, reinterpret_cast<struct sockaddr *>(&src), &len);
                if (received < 0) {
                    perror("recvfrom");
                    return;
                }

                std::cout << inet_ntoa(src.sin_addr) << " ";
                struct host