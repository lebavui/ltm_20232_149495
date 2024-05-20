#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

void *client_proc(void *);

int main() {
    // Tao socket cho ket noi
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    

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
    if (listen(listener, 10)) {
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

void *client_proc(void *arg) {
    int client = *(int *)arg;
    char buf[2048];

    // Nhan du lieu tu client
    int ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0) {
        close(client);
        pthread_exit(NULL);
    }

    buf[ret] = 0;
    printf("Received from %d: %s\n", client, buf);
    
    if (strncmp(buf, "GET / ", 6) == 0) {
        // Tra ve trang web Hello World
        strcpy(buf, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello World</h1>");
        send(client, buf, strlen(buf), 0);
    } else if (strncmp(buf, "GET /image ", 11) == 0) {
        // Tra ve file anh
        strcpy(buf, "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n");
        send(client, buf, strlen(buf), 0);

        // Doc file anh va tra ve client
        FILE *f = fopen("./files/test.jpg", "rb");
        while (1) {
            int len = fread(buf, 1, sizeof(buf), f);
            if (len <= 0)
                break;
            send(client, buf, len, 0);
        }
        fclose(f);
    }  else if (strncmp(buf, "GET /audio ", 11) == 0) {
        FILE *f = fopen("./files/test.mp3", "rb");

        fseek(f, 0, SEEK_END);
        long filesize = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Tra ve file audio
        sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type: audio/mp3\r\nContent-Length: %ld\r\n\r\n", filesize);
        send(client, buf, strlen(buf), 0);

        // Doc file audio va tra ve client
        char fbuf[2048];
        while (1) {
            int len = fread(fbuf, 1, sizeof(fbuf), f);
            if (len <= 0)
                break;
            // Note: Đối với file có kích thước lớn, có thể bị lỗi Broken Pipe khi thực hiện lệnh send.
            // Để tránh lỗi này, khi gửi sử dụng tham số MSG_NOSIGNAL ở cuối.
            send(client, buf, len, MSG_NOSIGNAL);
        }
        fclose(f);
    } else if (strncmp(buf, "GET /video ", 11) == 0) {
        FILE *f = fopen("./files/test.mp4", "rb");
        
        fseek(f, 0, SEEK_END);
        long filesize = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Tra ve file video
        sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type: video/mp4\r\nContent-Length: %ld\r\n\r\n", filesize);
        send(client, buf, strlen(buf), 0);

        // Doc file video va tra ve client
        while (1) {
            int len = fread(buf, 1, sizeof(buf), f);
            if (len <= 0)
                break;
            send(client, buf, len, MSG_NOSIGNAL);
        }
        fclose(f);
    }

    close(client);
    pthread_exit(NULL);
}