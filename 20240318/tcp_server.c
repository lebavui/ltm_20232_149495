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
    addr.sin_port = htons(atoi(argv[1]));

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

    // Doc xau chao tu file
    FILE *f = fopen(argv[2], "rb");
    char buf[1024];
    int len = fread(buf, 1, sizeof(buf), f);
    fclose(f);
    // Gui sang client
    send(client, buf, len, 0);

    // Nhan du lieu tu client va in ra man hinh
    f = fopen(argv[3], "wb");

    while (1) {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        
        if (ret < sizeof(buf))
            buf[ret] = 0;
        
        fwrite(buf, 1, ret, f);
        puts(buf);
    }

    fclose(f);
    close(client);
    close(listener);

    return 1;
}