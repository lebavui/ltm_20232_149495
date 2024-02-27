#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

void main() {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    printf("IP: %s\n", inet_ntoa(addr.sin_addr));
}