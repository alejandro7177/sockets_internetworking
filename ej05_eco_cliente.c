#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

    int sock_fd = -1;
    for (p = res; p != NULL; p = p->ai_next) {
        char ipstr[INET6_ADDRSTRLEN];
        void *addr = (p->ai_family == AF_INET) ?
            (void*)&(((struct sockaddr_in *)p->ai_addr)->sin_addr) :
            (void*)&(((struct sockaddr_in6 *)p->ai_addr)->sin6_addr);
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));

        printf("Intentando conectar a %s...\n", ipstr);

        sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock_fd == -1) continue;

        if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == 0) {
            printf("Conexión exitosa con %s\n", ipstr);
            break;
        }

        perror("Falló conexión");
        close(sock_fd);
        sock_fd = -1;
    }

    if (sock_fd == -1) {
        printf("No se pudo conectar a ninguna dirección.\n");
        freeaddrinfo(res);
        return 1;
    }
    freeaddrinfo(res);

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        send(sock_fd, buffer, strlen(buffer), 0);
        ssize_t bytes_received = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';
        printf("Respuesta: %s", buffer);
    }

    close(sock_fd);
    return 0;
}
