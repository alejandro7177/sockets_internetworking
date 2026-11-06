#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Función para enviar todos los bytes recibidos
ssize_t send_all(int socket, const char *buffer, size_t length) {
    size_t total_sent = 0;
    while (total_sent < length) {
        ssize_t sent = send(socket, buffer + total_sent, length - total_sent, 0);
        if (sent <= 0) return sent;
        total_sent += sent;
    }
    return total_sent;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <puerto> [noreuse]\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int noreuse = (argc >= 3 && strcmp(argv[2], "noreuse") == 0);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Error socket");
        return 1;
    }

    if (!noreuse) {
        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) == -1) {
        perror("Error listen");
        close(server_fd);
        return 1;
    }

    printf("Servidor eco escuchando en puerto %d...\n", port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) continue;

        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, sizeof(ip_str));
        int client_port = ntohs(client_addr.sin_port);
        printf("Cliente conectado: %s:%d\n", ip_str, client_port);

        char buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
            if (send_all(client_fd, buffer, bytes_read) <= 0) break;
        }

        printf("Cliente desconectado: %s:%d\n", ip_str, client_port);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
