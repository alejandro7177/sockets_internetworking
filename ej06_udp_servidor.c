#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <puerto>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("Error socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error bind");
        close(sock_fd);
        return 1;
    }

    printf("Servidor UDP escuchando en puerto %d...\n", port);

    char buffer[2048];
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        ssize_t bytes_recibidos = recvfrom(sock_fd, buffer, sizeof(buffer), 0,
                                           (struct sockaddr *)&client_addr, &client_len);
        if (bytes_recibidos < 0) continue;

        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, sizeof(ip_str));
        int client_port = ntohs(client_addr.sin_port);

        printf("Recibido de %s:%d | Tama%co: %zd bytes\n", ip_str, client_port, 164, bytes_recibidos);

        sendto(sock_fd, buffer, bytes_recibidos, 0,
               (struct sockaddr *)&client_addr, client_len);
    }

    close(sock_fd);
    return 0;
}
