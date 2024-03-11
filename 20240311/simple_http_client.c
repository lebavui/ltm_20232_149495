#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int main() 
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct addrinfo *res;
    int ret = getaddrinfo("httpbin.org", "http", NULL, &res);
    if (ret == -1 || res == NULL) {
        printf("Khong phan giai duoc ten mien.\n");
        return 1;
    }

    ret = connect(client, res->ai_addr, res->ai_addrlen);
    if (ret == -1) {
        printf("Khong ket noi duoc den server.\n");
        return 1;
    }

    char buf[1024] = "GET /get HTTP/1.1\r\nHost: httpbin.org\r\nConnection: close\r\n\r\n";
    send(client, buf, strlen(buf), 0);

    ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0) {
        close(client);
        return 1;
    }

    if (ret < sizeof(buf))
        buf[ret] = 0;

    puts(buf);

    close(client);

    return 0;
}