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

    int client = accept(listener, NULL, NULL);

    int a[20];
    int n;

    while (1) {
        int ret = recv(client, a, sizeof(a), 0);
        if (ret <= 0)
            break;
        
        printf("%d bytes\n", ret);

        n = ret / sizeof(a[0]);
        for (int i = 0; i < n; i++)
            printf("%d ", a[i]);
        printf("\n");
    }

    close(client);
    close(listener);

    return 1;
}