#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void signalHanlder(int signo) {
    pid_t pid = wait(NULL);
    printf("Child process terminated, pid = %d\n", pid);
}

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

    signal(SIGCHLD, signalHanlder);

    while (1) {
        printf("Waiting for new client\n");
        int client = accept(listener, NULL, NULL);
        printf("New client accepted, client = %d\n", client);

        if (fork() == 0) {
            // Tien trinh con, xu ly yeu cau tu client
            // Dong socket listener
            close(listener);

            int is_login = 0;

            char buf[256];
            while (1) {
                int ret = recv(client, buf, sizeof(buf), 0);
                if (ret <= 0)
                    break;

                buf[ret] = 0;
                printf("Received: %s", buf);

                if (!is_login) {
                    // Chua dang nhap
                    char user[32], pass[32], tmp[65], line[65];
                    int n = sscanf(buf, "%s %s %s", user, pass, tmp);
                    if (n != 2) {
                        char *msg = "Sai cu phap. Hay nhap lai.\n";
                        send(client, msg, strlen(msg), 0);
                    } else {
                        // Kiem tra thong tin dang nhap
                        sprintf(tmp, "%s %s\n", user, pass);
                        FILE *f = fopen("accounts.txt", "r");
                        int found = 0;
                        while (fgets(line, sizeof(line), f) != NULL) {
                            if (strcmp(tmp, line) == 0) {
                                found = 1;
                                break;
                            }
                        }
                        if (found) {
                            char *msg = "Dang nhap thanh cong.\n";
                            send(client, msg, strlen(msg), 0);

                            // Luu thong tin dang nhap
                            is_login = 1;
                        } else {
                            char *msg = "Dang nhap that bai. Hay nhap lai.\n";
                            send(client, msg, strlen(msg), 0);    
                        }
                        fclose(f);
                    }
                } else {
                    // Da dang nhap
                    char cmd[300];

                    // Xoa ky tu xuong dong o cuoi buf
                    if (buf[strlen(buf) - 1] == '\n')
                        buf[strlen(buf) - 1] = 0;

                    // Thuc hien lenh
                    sprintf(cmd, "%s > out.txt", buf);
                    system(cmd);

                    // Tra lai ket qua cho client
                    FILE *f = fopen("out.txt", "rb");
                    while (1) {
                        int n = fread(buf, 1, sizeof(buf), f);
                        if (n <= 0)
                            break;
                        send(client, buf, n, 0);
                    }
                    fclose(f);
                }
            }

            // Ket thuc tien trinh con
            exit(0);
        }

        // Dong socket client o tien trinh cha
        close(client);
    }

    return 0;
}