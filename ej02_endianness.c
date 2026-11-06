#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

void mostrar_bytes(const void *ptr, size_t size) {
    const unsigned char *p = (const unsigned char *)ptr;
    for (size_t i = 0; i < size; i++) {
        printf("0x%02X ", p[i]);
    }
    printf("\n");
}

int main(void) {
    // 1. Determinar endianness
    uint16_t test = 0x0102;
    unsigned char *first_byte = (unsigned char *)&test;
    printf("1. Endianness:\n");
    if (*first_byte == 0x02) {
        printf("   La máquina es Little-Endian (primer byte: 0x02)\n");
    } else {
        printf("   La máquina es Big-Endian (primer byte: 0x01)\n");
    }

    // 2. Número 8080 (0x1F90) antes y después de htons()
    uint16_t port = 8080;
    uint16_t port_net = htons(port);
    printf("\n2. Puerto 8080 (0x1F90):\n");
    printf("   Original (Host): ");
    mostrar_bytes(&port, sizeof(port));
    printf("   Con htons() (Red): ");
    mostrar_bytes(&port_net, sizeof(port_net));

    // 3. Dirección IP 0xC0A8010A antes y después de htonl()
    uint32_t ip = 0xC0A8010A;
    uint32_t ip_net = htonl(ip);
    printf("\n3. IP 0xC0A8010A (192.168.1.10):\n");
    printf("   Original (Host): ");
    mostrar_bytes(&ip, sizeof(ip));
    printf("   Con htonl() (Red): ");
    mostrar_bytes(&ip_net, sizeof(ip_net));

    // 4. Asignación a sin_port en struct sockaddr_in
    struct sockaddr_in addr1, addr2;
    
    // Sin convertir
    addr1.sin_port = 8080;
    printf("\n4. Sin htons() -> ntohs(addr1.sin_port): %u (0x%04X)\n", 
           ntohs(addr1.sin_port), ntohs(addr1.sin_port));

    // Con htons()
    addr2.sin_port = htons(8080);
    printf("   Con htons() -> ntohs(addr2.sin_port): %u (0x%04X)\n", 
           ntohs(addr2.sin_port), ntohs(addr2.sin_port));

    return 0;
}
