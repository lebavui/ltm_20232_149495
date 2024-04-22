#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>

int main() 
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8000);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1) {
        perror("connect() failed");
        return 1;
    }

    if (fork() == 0) {
        // Doc chuoi ky tu tu ban phim, gui sang server
        char buf[256];
        while (1) {
            fgets(buf, sizeof(buf), stdin);
            send(client, buf, strlen(buf), 0);
            if (strncmp(buf, "exit", 4) == 0)
                break;
        }
    } else {
        // Nhan du lieu tu server, in ra man hinh
        char buf[256];
        while (1) {
            int ret = recv(client, buf, sizeof(buf), 0);
            if (ret <= 0)
                break;
            buf[ret] = 0;
            puts(buf);
        }
    }

    killpg(0, SIGKILL);

    return 0;
}