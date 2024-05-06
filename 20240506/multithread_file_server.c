#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <dirent.h>

void *client_proc(void *);

int clients[64];

int main() {
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
    addr.sin_port = htons(8000);

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

    while (1) {
        printf("Waiting for new client\n");
        int client = accept(listener, NULL, NULL);
        printf("New client accepted, client = %d\n", client);
        
        char buf[2048];
        int pos = 0;
        int count = 0;

        DIR *d = opendir(".");
        struct dirent *dir;

        buf[0] = '\0';

        if (d != NULL)
            while ((dir = readdir(d)) != NULL) {
                if (dir->d_type == DT_REG) {
                    count++;
                    strcat(buf, dir->d_name);
                    strcat(buf, "\r\n");
                }
            }

        if (count == 0) {
            // Thu muc khong co file nao
            char *msg = "ERROR No files to download\r\n";
            send(client, msg, strlen(msg), 0);
            close(client);
        } else {
            // Thu muc co file
            char tmp[32];
            sprintf(tmp, "OK %d files\r\n", count);
            send(client, tmp, strlen(tmp), 0);
            strcat(buf, "\r\n\r\n");
            send(client, buf, strlen(buf), 0);

            pthread_t tid;
            pthread_create(&tid, NULL, client_proc, &client);
            pthread_detach(tid);
        }
    }

    return 0;
}

void *client_proc(void *arg) {
    int client = *(int *)arg;
    char buf[2048];

    // Nhan du lieu tu client
    while (1) {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;

        buf[ret] = 0;
        printf("Received from %d: %s\n", client, buf);

        // Xoa ky tu xuong dong o cuoi neu co
        if (buf[ret - 1] == '\n')
            buf[ret - 1] = 0;

        FILE *f = fopen(buf, "rb");
        if (f == NULL) {
            char *msg = "ERROR Cannot open file. Enter other file name.\r\n";
            send(client, msg, strlen(msg), 0);
        } else {
            fseek(f, 0, SEEK_END);
            long filesize = ftell(f);
            fseek(f, 0, SEEK_SET);

            char tmp[32];
            sprintf(tmp, "OK %ld bytes\r\n", filesize);
            send(client, tmp, strlen(tmp), 0);

            while (!feof(f)) {
                int n = fread(buf, 1, sizeof(buf), f);
                if (n <= 0)
                    break;
                send(client, buf, n, 0);
            }

            fclose(f);
            // Dong ket noi, ket thuc phien truyen nhan du lieu
            break;
        }
    }

    close(client);
}