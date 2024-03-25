#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
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

    char buf[2048];
    int pos;
    char path[256];
    char filename[32];
    long filesize;

    while (1) {
        int client = accept(listener, NULL, NULL);
        int ret = recv(client, buf, sizeof(buf), 0);

        pos = 0;
        strcpy(path, buf);
        pos += strlen(path) + 1;

        puts(path);

        while (pos < ret) {
            strcpy(filename, buf + pos);
            pos += strlen(filename) + 1;
            memcpy(&filesize, buf + pos, sizeof(filesize));
            pos += sizeof(filesize);

            printf("%s - %ld bytes\n", filename, filesize);
        }
        
        close(client);
    }
    
    close(listener);

    return 1;
}