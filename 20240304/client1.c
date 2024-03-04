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

    char msg[] = "Hello server\n";
    send(client, msg, strlen(msg), 0);

    char buf[256];
    while (1) {
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        buf[ret] = 0;
        printf("%d bytes received: %s\n", ret, buf);
    }

    close(client);
}