#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() 
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1) {
        perror("connect() failed");
        return 1;
    }

    char filename[] = "test.pdf";
    FILE *f = fopen(filename, "rb");
    char buf[2048];

    // Truyen ten file
    int n = strlen(filename);
    send(client, &n, sizeof(n), 0);
    send(client, filename, strlen(filename), 0);

    // Truyen kich thuoc file
    fseek(f, 0, SEEK_END);
    long filesize = ftell(f);
    send(client, &filesize, sizeof(filesize), 0);

    // Truyen noi dung file
    fseek(f, 0, SEEK_SET);
    while (1) {
        int ret = fread(buf, 1, sizeof(buf), f);
        if (ret <= 0)
            break;    
        send(client, buf, ret, 0);
    }

    fclose(f);
    close(client);

    return 0;
}