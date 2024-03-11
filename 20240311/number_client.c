#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

struct diemthi {
    int diem_qt;
    int diem_ck;
};

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

    struct diemthi dt;
    
    while (1) {
        printf("Nhap diem qt: ");
        scanf("%d", &dt.diem_qt);
        printf("Nhap diem ck: ");
        scanf("%d", &dt.diem_ck);

        send(client, &dt, sizeof(dt), 0);

        if (dt.diem_qt == 0)
            break;        
    }

    close(client);

    return 0;
}