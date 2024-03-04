#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // Tao socket cho ket noi
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        perror("socket() failed");
        return 1;
    }

    // Khai bao dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    // Gan socket voi cau truc dia chi
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        return 1;
    }

    // Chuyen socket sang trang thai cho ket noi
    if (listen(listener, 5)) {
        perror("listen() failed");
        return 1;
    }

    printf("Dang cho ket noi...\n");
    
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);

    while (1)
    {
        int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
        printf("Client moi ket noi: %d - IP: %s - Port: %d\n", 
            client, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        char buf[256];
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0) {
            close(client);
            continue;
        }

        buf[ret] = 0;
        puts(buf);

        send(client, buf, strlen(buf), 0);
        close(client);
    }
    
    close(listener);

    return 0;
}