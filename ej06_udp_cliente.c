#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <host> <puerto> [conectado]\n", argv[0]);
        return 1;
    }

    int modo_conectado = (argc >= 4 && strcmp(argv[3], "conectado") == 0);

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(argv[1], argv[2], &hints, &res) != 0) {
        perror("Error resolviedo host/puerto");
        return 1;
    }

    int sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock_fd == -1) {
        perror("Error socket");
        freeaddrinfo(res);
        return 1;
    }

    // Configurar timeout de 2 segundos
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    if (modo_conectado) {
        if (connect(sock_fd, res->ai_addr, res->ai_addrlen) == -1) {
            perror("Error connect");
            close(sock_fd);
            freeaddrinfo(res);
            return 1;
        }
    }

    int tamanos[] = {1, 18, 1000};
    for (int i = 0; i < 3; i++) {
        int tam = tamanos[i];
        char *msg_send = malloc(tam);
        char *msg_recv = malloc(tam + 100);
        memset(msg_send, 'A', tam);

        printf("\n--- Enviando datagrama de %d bytes (%s) ---\n", tam, modo_conectado ? "modo conectado" : "modo NO conectado");

        int exito = 0;
        for (int intento = 1; intento <= 3; intento++) {
            printf("Intento %d: enviando...", intento);
            fflush(stdout);

            if (modo_conectado) {
                if (send(sock_fd, msg_send, tam, 0) < 0) {
                    perror(" Error en send()");
                }
            } else {
                if (sendto(sock_fd, msg_send, tam, 0, res->ai_addr, res->ai_addrlen) < 0) {
                    perror(" Error en sendto()");
                }
            }

            ssize_t n = 0;
            if (modo_conectado) {
                n = recv(sock_fd, msg_recv, tam + 100, 0);
            } else {
                n = recvfrom(sock_fd, msg_recv, tam + 100, 0, NULL, NULL);
            }

            if (n >= 0) {
                printf(" Recibido ok! Bytes devueltos: %zd\n", n);
                if (n == tam) {
                    printf("Verificación: El tamaño coincide perfectamente (%d bytes).\n", tam);
                } else {
                    printf("Verificación: TAMAÑO NO COINCIDE.\n");
                }
                exito = 1;
                break;
            } else {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    printf(" TIMEOUT (sin respuesta tras 2s)\n");
                } else if (errno == ECONNREFUSED) {
                    printf(" ERROR: Connection refused (ICMP Unreachable)\n");
                    break;
                } else {
                    perror(" Error recv");
                    break;
                }
            }
        }

        if (!exito) {
            printf("Abandonando mensaje de %d bytes tras fallar intentos.\n", tam);
        }

        free(msg_send);
        free(msg_recv);
    }

    close(sock_fd);
    freeaddrinfo(res);
    return 0;
}
