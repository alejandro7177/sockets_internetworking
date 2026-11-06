#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <host> <servicio>\n", argv[0]);
        return 1;
    }

    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (status != 0) {
        printf("Error getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        char ipstr[INET6_ADDRSTRLEN];
        int port = 0;
        char hostbuf[NI_MAXHOST];

        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
            port = ntohs(ipv4->sin_port);
            printf("Familia: IPv4 | IP: %s | Puerto: %d\n", ipstr, port);
        } else if (p->ai_family == AF_INET6) {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipstr, sizeof(ipstr));
            port = ntohs(ipv6->sin6_port);
            printf("Familia: IPv6 | IP: %s | Puerto: %d\n", ipstr, port);
        }

        int res_name = getnameinfo(p->ai_addr, p->ai_addrlen, hostbuf, sizeof(hostbuf), NULL, 0, 0);
        if (res_name == 0) {
            printf("  Nombre inverso: %s\n", hostbuf);
        } else {
            printf("  Nombre inverso: No disponible (%s)\n", gai_strerror(res_name));
        }
    }

    freeaddrinfo(res);
    return 0;
}
