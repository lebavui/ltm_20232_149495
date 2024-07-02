#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <malloc.h>
#include <openssl/ssl.h>

int main(int argc, char * argv[]) 
{
    if (argc != 2) {
        puts("Sai so luong tham so.");
        return -1;
    }

    char *pos1 = strstr(argv[1], "://");
    char protocol[16] = {0}, domain[256] = {0}, path[256] = {0};
    memcpy(protocol, argv[1], pos1 - argv[1]);

    pos1 += 3;
    char *pos2 = strchr(pos1, '/');
    memcpy(domain, pos1, pos2 - pos1);

    strcpy(path, pos2);

    printf("Protocol: %s\n", protocol);
    printf("Domain: %s\n", domain);
    printf("Path: %s\n", path);

    // Phan giai ten mien
    struct addrinfo *info;
    if (getaddrinfo(domain, protocol, NULL, &info)) {
        puts("Khong phan giai duoc ten mien.");
        return -1;
    }

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int ret = connect(client, info->ai_addr, info->ai_addrlen);
    if (ret == -1) {
        perror("Khong ket noi duoc den server");
        return -1;
    }

    if (strcmp(protocol, "http") == 0) {
        char buf[2048];

        sprintf(buf, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, domain);
        send(client, buf, strlen(buf), 0);

        // Nhan noi dung phan header
        char *header = NULL;
        int len = 0;

        while (1) {
            int ret = recv(client, buf, sizeof(buf), 0);
            if (ret <= 0) {
                close(client);
                printf("disconnected\n");
                return 1;
            }

            header = realloc(header, len + ret);
            memcpy(header + len, buf, ret);
            len += ret;

            if (memmem(header, len, "\r\n\r\n", 4) != NULL) {
                break;
            }
        }

        // In header
        puts(header);

        char *pos = memmem(header, len, "\r\n\r\n", 4);

        FILE *f = fopen("download.mp4", "wb");

        // Nhan noi dung file
        int header_length = pos - header;
        if (header_length + 4 < len)
            fwrite(pos + 4, 1, len - header_length - 4, f);
        
        while (1) {
            int ret = recv(client, buf, sizeof(buf), 0);
            if (ret <= 0)
                break;
            fwrite(buf, 1, ret, f);
        }

        free(header);
        fclose(f);
        close(client);
    } else if (strcmp(protocol, "https") == 0) {
        SSL_library_init();				// Khởi tạo thư viện OpenSSL
        const SSL_METHOD *meth = TLS_client_method();	// Khai báo phương thức mã hóa TLS
        SSL_CTX *ctx = SSL_CTX_new(meth);	// Tạo context mới
        SSL *ssl = SSL_new(ctx);		// Tạo con trỏ ssl
        if (!ssl) {
            printf("Error creating SSL.\n");
            return -1;
        }

        SSL_set_fd(ssl, client);		// Gắn con trỏ ssl với socket
        int err = SSL_connect(ssl);	// Tạo kết nối ssl
        if (err <= 0) {
            printf("Error creating SSL connection.  err=%x\n", err);
            return -1;
        }

        char buf[2048];
        sprintf(buf, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, domain);
        SSL_write(ssl, buf, strlen(buf));
        
        // Nhan noi dung phan header
        char *header = NULL;
        int len = 0;

        while (1) {
            int ret = SSL_read(ssl, buf, sizeof(buf));
            if (ret <= 0) {
                close(client);
                printf("disconnected\n");
                return 1;
            }

            header = realloc(header, len + ret);
            memcpy(header + len, buf, ret);
            len += ret;

            if (memmem(header, len, "\r\n\r\n", 4) != NULL) {
                break;
            }
        }

        // In header
        puts(header);

        char *pos = memmem(header, len, "\r\n\r\n", 4);

        FILE *f = fopen("download.mp4", "wb");

        // Nhan noi dung file
        int header_length = pos - header;
        if (header_length + 4 < len)
            fwrite(pos + 4, 1, len - header_length - 4, f);
        
        while (1) {
            int ret = SSL_read(ssl, buf, sizeof(buf));
            if (ret <= 0)
                break;
            fwrite(buf, 1, ret, f);
        }

        free(header);

        fclose(f);
        close(client);
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
    } else {
        puts("Giao thuc khong ho tro.");
        return -1;
    }
}