#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    bind(receiver, (struct sockaddr *)&addr, sizeof(addr));

    char buf[16];

    while (1) {
        int ret = recvfrom(receiver, buf, sizeof(buf), 0,
            NULL, NULL);
        if (ret == -1) {
            printf("Loi khi nhan du lieu");
        } else {
            buf[ret] = 0;
            puts(buf);
        }
    }
}