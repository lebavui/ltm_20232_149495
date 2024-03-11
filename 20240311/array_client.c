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

    int a[20];
    int n;

    while (1) {
        printf("Nhap so phan tu: ");
        scanf("%d", &n);
        for (int i = 0; i < n; i++) {
            printf("a[%d] = ", i);
            scanf("%d", &a[i]);   
        }

        if (n == 0)
            break;

        send(client, a, sizeof(a[0]) * n, 0);
    }

    close(client);

    return 0;
}