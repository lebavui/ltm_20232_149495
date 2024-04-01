#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

#define MAX_FDS 2048

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
    addr.sin_port = htons(8000);

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

    struct pollfd fds[MAX_FDS];
    int nfds = 0;

    // Gan socket listener vao tap tham do
    fds[0].fd = listener;
    fds[0].events = POLLIN;
    nfds++;

    char buf[256];

    while (1) {
        int ret = poll(fds, nfds, 0);
        if (ret == -1) {
            break;
        }
        if (ret == 0) {
            printf("Timed out!!!\n");
            continue;
        }

        for (int i = 0; i < nfds; i++) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == listener) {
                    // Co ket noi moi
                    int client = accept(listener, NULL, NULL);
                    if (client >= MAX_FDS) {
                        // Vuot qua so luong toi da
                        close(client);
                    } else {
                        // Them vao mang tham do su kien
                        fds[nfds].fd = client;
                        fds[nfds].events = POLLIN;
                        nfds++;

                        printf("New client connected: %d\n", client);
                    }
                } else {
                    // Co du lieu tu client truyen den
                    int client = fds[i].fd;
                    ret = recv(client, buf, sizeof(buf), 0);
                    if (ret <= 0) {
                        // Ket noi bi dong => Xoa khoi mang
                    } else {
                        // Xu ly du lieu nhan duoc
                        buf[ret] = 0;
                        printf("Received from %d: %s\n", client, buf);
                    }
                }
            }
        }
    }

    return 0;
}