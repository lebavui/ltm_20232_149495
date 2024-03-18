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

    char buf[256];

    int mssv;
    char hoten[64];
    unsigned char ngay, thang;
    unsigned short nam;
    float diemtb;

    while (1) {
        int client = accept(listener, NULL, NULL);
        int ret = recv(client, buf, sizeof(buf), 0);

        int pos = 0;
        memcpy(&mssv, buf, sizeof(mssv));
        pos += sizeof(mssv);
        ngay = buf[pos++];
        thang = buf[pos++];
        memcpy(&nam, buf + pos, sizeof(nam));
        pos += sizeof(nam);
        memcpy(&diemtb, buf + pos, sizeof(diemtb));
        pos += sizeof(diemtb);
        
        int len = ret - pos;
        memcpy(hoten, buf + pos, len);
        hoten[len] = 0;

        printf("%d %s %d-%d-%d %.2f\n", mssv, hoten, nam, thang, ngay, diemtb);

        close(client);
    }
    
    close(listener);

    return 1;
}