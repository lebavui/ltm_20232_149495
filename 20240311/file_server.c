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

    // Nhan ten file
    int n;
    recv(client, &n, sizeof(n), 0);
    char filename[32];
    recv(client, filename, n, 0);
    filename[n] = 0;

    printf("Ten file: %s\n", filename);

    long filesize;
    recv(client, &filesize, sizeof(filesize), 0);

    printf("Kich thuoc file: %ld\n", filesize);

    FILE *f = fopen("download.pdf", "wb");
    char buf[2048];

    while (1) {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        
        fwrite(buf, 1, ret, f);
    }

    fclose(f);
    close(client);
    close(listener);

    return 1;
}