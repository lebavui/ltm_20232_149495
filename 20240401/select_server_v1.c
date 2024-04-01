#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

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

    fd_set fdread;

    int clients[FD_SETSIZE];
    int numClients = 0;

    int maxdp;
    char buf[256];

    while (1) {
        // Khoi tao tap fdread
        FD_ZERO(&fdread);

        // Gan cac socket vao tap fdread
        FD_SET(listener, &fdread);
        maxdp = listener;
        for (int i = 0; i < numClients; i++) {
            FD_SET(clients[i], &fdread);
            if (clients[i] > maxdp)
                maxdp = clients[i];
        }

        // Goi ham select cho su kien
        int ret = select(maxdp + 1, &fdread, NULL, NULL, NULL);
        if (ret == -1) {
            perror("socket() failed");
            break;
        }

        // Kiem tra su kien cua socket nao
        if (FD_ISSET(listener, &fdread)) {
            // Co ket noi moi
            int client = accept(listener, NULL, NULL);
            if (client >= FD_SETSIZE) {
                close(client);
            } else {
                // Luu socket vao mang
                clients[numClients] = client;
                numClients++;
                printf("New client connected: %d\n", client);
            }
        }

        for (int i = 0; i < numClients; i++)
            if (FD_ISSET(clients[i], &fdread)) {
                ret = recv(clients[i], buf, sizeof(buf), 0);
                if (ret <= 0) {
                    // Ket noi bi ngat
                    printf("Socket closed %d\n", clients[i]);
                    close(clients[i]);

                    // Xoa socket tai vi tri i
                    if (i < numClients - 1) {
                        clients[i] = clients[numClients - 1];
                    }
                    numClients--;
                    i--;

                    continue;
                } else {
                    // Xu ly du lieu nhan duoc
                    buf[ret] = 0;
                    printf("Received from %d: %s\n", clients[i], buf);
                }
            }
    }

    return 0;
}