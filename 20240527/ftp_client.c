#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

int client;

unsigned short send_pasv();
int send_list();
int download_file(char *remote_file);
int upload_file(char *local_file);
int rename_file(char *cur_file, char *new_file);
int delete_file(char *filename);
int print_working_dir();
int change_working_dir(char *dirname);
int make_dir(char *dirname);
int remove_dir(char *dirname);

int main() 
{
    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("10.13.32.202");
    addr.sin_port = htons(21);

    int ret = connect(client, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
        perror("connect() failed");
        return 1;
    }

    char buf[2048];

    // Nhan xau chao
    ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0) {
        close(client);
        return 1;
    }

    buf[ret] = 0;
    puts(buf);

    // Dang nhap
    char user[32], pass[32];
    printf("Nhap username: ");
    fgets(user, sizeof(user), stdin);
    printf("Nhap password: ");
    fgets(pass, sizeof(pass), stdin);

    // Xoa ky tu xuong trong
    user[strlen(user) - 1] = 0;
    pass[strlen(pass) - 1] = 0;

    // Gui lenh USER
    sprintf(buf, "USER %s\r\n", user);
    send(client, buf, strlen(buf), 0);

    ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0) {
        close(client);
        return 1;
    }

    buf[ret] = 0;
    puts(buf);

    // Gui lenh PASS
    sprintf(buf, "PASS %s\r\n", pass);
    send(client, buf, strlen(buf), 0);

    ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0) {
        close(client);
        return 1;
    }

    buf[ret] = 0;
    puts(buf);

    // Kiem tra dang nhap thanh cong
    if (strncmp(buf, "230 ", 4) == 0) {
        puts("Dang nhap thanh cong.");
    } else {
        puts("Dang nhap that bai.");
        close(client);
        return 1;
    }

    // Lay ve danh sach cua thu muc hien tai
    send_list();

    // Download file
    printf("Nhap ten file de download: ");
    char filename[256];
    fgets(filename, sizeof(filename), stdin);
    filename[strlen(filename) - 1] = 0;
    download_file(filename);

    close(client);
}

unsigned short send_pasv(int client) {
    char buf[2048];

    send(client, "PASV\r\n", 6, 0);

    int ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0) {
        close(client);
        return 1;
    }

    buf[ret] = 0;
    // puts(buf);

    char *pos = strchr(buf, '(');
    int i1 = atoi(strtok(pos, "(),"));
    int i2 = atoi(strtok(NULL, "(),"));
    int i3 = atoi(strtok(NULL, "(),"));
    int i4 = atoi(strtok(NULL, "(),"));
    int p1 = atoi(strtok(NULL, "(),"));
    int p2 = atoi(strtok(NULL, "(),"));

    return p1 * 256 + p2;
}

int send_list() {
    // Gui lenh PASV
    unsigned short port = send_pasv(client);
    printf("Port: %d\n", port);

    // Mo ket noi du lieu 
    int client_data = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr_data;
    addr_data.sin_family = AF_INET;
    addr_data.sin_addr.s_addr = inet_addr("10.13.32.202");
    addr_data.sin_port = htons(port);

    int ret = connect(client_data, (struct sockaddr *) &addr_data, sizeof(addr_data));
    if (ret == -1) {
        perror("connect() failed");
        return 1;
    }

    // Gui lenh LIST
    send(client, "LIST\r\n", 6, 0);

    char buf[2048];

    ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0) {
        close(client);
        return 1;
    }

    buf[ret] = 0;
    puts(buf);

    // Nhan du lieu tren kenh du lieu
    // In ra man hinh
    while (1) {
        ret = recv(client_data, buf, sizeof(buf) - 1, 0);
        if (ret <= 0) {
            close(client_data);
            break;
        }

        buf[ret] = 0;
        printf("%s", buf);
    }

    printf("\n");

    ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0) {
        close(client);
        return 1;
    }

    buf[ret] = 0;
    puts(buf);
}

int download_file(char *remote_file) {
    unsigned short port = send_pasv(client);
    printf("Port: %d\n", port);

    // Mo ket noi du lieu 
    int client_data = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr_data;
    addr_data.sin_family = AF_INET;
    addr_data.sin_addr.s_addr = inet_addr("10.13.32.202");
    addr_data.sin_port = htons(port);

    int ret = connect(client_data, (struct sockaddr *) &addr_data, sizeof(addr_data));
    if (ret == -1) {
        perror("connect() failed");
        return 1;
    }

    char buf[2048];

    // Gui lenh retr
    sprintf(buf, "retr %s\r\n", remote_file);
    send(client, buf, strlen(buf), 0);

    ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0) {
        close(client);
        return 1;
    }

    buf[ret] = 0;
    puts(buf);

    // Nhan noi dung file
    FILE *f = fopen(remote_file, "wb");
    while (1) {
        ret = recv(client_data, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        fwrite(buf, 1, ret, f);
    }
    close(client_data);
    fclose(f);

    ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0) {
        close(client);
        return 1;
    }

    buf[ret] = 0;
    puts(buf);
}