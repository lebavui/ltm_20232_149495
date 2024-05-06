#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

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
        
        pthread_t tid;
        pthread_create(&tid, NULL, client_proc, &client);
        pthread_detach(tid);
    }

    return 0;
}

int client_sockets[1024];
char *client_names[1024];
int num_clients = 0;

int remove_client(int client, int *client_sockets, char **client_names, int *num_clients) {
    int i = 0;
    for (; i < *num_clients; i++)
        if (client_sockets[i] == client)
            break;
    
    if (i < *num_clients) {
        if (i < *num_clients - 1) {
            client_sockets[i] = client_sockets[*num_clients - 1];
            strcpy(client_names[i], client_names[*num_clients - 1]);
        }

        free(client_names[*num_clients - 1]);
        *num_clients -= 1;
    }
}

void *client_proc(void *arg) {
    int client = *(int *)arg;
    char buf[256];

    // Nhan du lieu tu client
    while (1) {
        int ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0) {
            remove_client(client, client_sockets, client_names, &num_clients);
            break;
        }
            
        buf[ret] = 0;
        printf("Received from %d: %s\n", client, buf);

        // Kiem tra trang thai dang nhap
        int i = 0;
        for (; i < num_clients; i++)
            if (client_sockets[i] == client)
                break;
        if (i == num_clients) {
            // Chua dang nhap
            char cmd[16], id[32], tmp[32];
            int n = sscanf(buf, "%s %s %s", cmd, id, tmp);
            if (n == 2) {
                if (strcmp(cmd, "client_id:") == 0) {
                    // Kiem tra id da ton tai chua
                    int k = 0;
                    for (; k < num_clients; k++)
                        if (strcmp(client_names[k], id) == 0)
                            break;
                    if (k < num_clients) {
                        // id da ton tai
                        char *msg = "ID da ton tai. Hay nhap ID khac\n";
                        send(client, msg, strlen(msg), 0);
                    } else {
                        // id chua ton tai
                        char *msg = "Dang nhap thanh cong.\n";
                        send(client, msg, strlen(msg), 0);

                        // Chuyen client sang trang thai dang nhap
                        client_sockets[num_clients] = client;
                        client_names[num_clients] = malloc(strlen(id) + 1);
                        memcpy(client_names[num_clients], id, strlen(id) + 1);
                        // strcpy(client_names[num_clients], id);
                        num_clients++;
                    }
                } else {
                    char *msg = "Sai cu phap. Hay nhap lai.\n";
                    send(client, msg, strlen(msg), 0);
                }
            } else {
                char *msg = "Sai cu phap. Hay nhap lai.\n";
                send(client, msg, strlen(msg), 0);
            }
        } else {
            // Da dang nhap

            char receiver[32];
            sscanf(buf, "%s", receiver);

            if (strcmp(receiver, "all") == 0) {
                // Chuyen tiep tin nhan cho cac client da dang nhap khac
                for (int k = 0; k < num_clients; k++)
                    if (client_sockets[k] != client) {
                        char msg[512];
                        sprintf(msg, "%s: %s\n", client_names[i], buf + strlen(receiver) + 1);
                        send(client_sockets[k], msg, strlen(msg), 0);
                    }
            } else {
                // Chuyen tiep tin nhan cho mot client 
                for (int k = 0; k < num_clients; k++)
                    if (strcmp(client_names[k], receiver) == 0) {
                        char msg[512];
                        sprintf(msg, "%s: %s\n", client_names[i], buf + strlen(receiver) + 1);
                        send(client_sockets[k], msg, strlen(msg), 0);
                    }
            }
        }
    }

    close(client);
}