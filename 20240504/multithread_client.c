#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void *thread_proc(void *);

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

    pthread_t tid;
    pthread_create(&tid, NULL, thread_proc, &client);

    char buf[256];

    while (1) {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)   
            break;
        buf[ret] = 0;
        puts(buf);
    }

    close(client);

    return 0;
}

void *thread_proc(void *arg) {
    int client = *(int *)arg;
    char buf[256];
    while (1) {
        fgets(buf, sizeof(buf), stdin);
        send(client, buf, strlen(buf), 0);
        if (strncmp(buf, "exit", 4) == 0)
            break;
    }
    close(client);
}