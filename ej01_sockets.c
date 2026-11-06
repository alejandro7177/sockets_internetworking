#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

void crear_y_mostrar(const char *nombre, int domain, int type, int protocol) {
    int s = socket(domain, type, protocol);
    if (s == -1) {
        perror(nombre);
    } else {
        printf("%s descriptor: %d\n", nombre, s);
    }
}

int main(void) {
    // 1. Crear tres sockets sin cerrarlos
    int s_tcp4 = socket(AF_INET, SOCK_STREAM, 0);
    int s_udp4 = socket(AF_INET, SOCK_DGRAM, 0);
    int s_tcp6 = socket(AF_INET6, SOCK_STREAM, 0);

    // 2. Muestra los descriptores
    if (s_tcp4 != -1) printf("TCP IPv4 descriptor: %d\n", s_tcp4);
    else perror("Error TCP IPv4");

    if (s_udp4 != -1) printf("UDP IPv4 descriptor: %d\n", s_udp4);
    else perror("Error UDP IPv4");

    if (s_tcp6 != -1) printf("TCP IPv6 descriptor: %d\n", s_tcp6);
    else perror("Error TCP IPv6");

    // 3. Cierre de los tres sockets
    if (s_tcp4 != -1) close(s_tcp4);
    if (s_udp4 != -1) close(s_udp4);
    if (s_tcp6 != -1) close(s_tcp6);

    // 4. Intento con combinación inválida
    printf("\nPrueba de error (SOCK_STREAM con IPPROTO_UDP):\n");
    crear_y_mostrar("Socket invalido", AF_INET, SOCK_STREAM, IPPROTO_UDP);

    return 0;
}
