#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <malloc.h>

int main() {
    SSL_library_init();				// Khởi tạo thư viện OpenSSL
    const SSL_METHOD *meth = TLS_client_method();	// Khai báo phương thức mã hóa TLS
    SSL_CTX *ctx = SSL_CTX_new(meth);	// Tạo context mới
    SSL *ssl = SSL_new(ctx);		// Tạo con trỏ ssl
    if (!ssl) {
        printf("Error creating SSL.\n");
        return -1;
    }

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct  addrinfo *info;
    
    // https://lebavui.github.io/videos/ecard.mp4

    if (getaddrinfo("lebavui.github.io", "https", NULL, &info)) {
        puts("getaddrinfo() failed");
        return -1;
    }

    if (connect(client, info->ai_addr, info->ai_addrlen)) {
        perror("connect() failed");
        return -1;
    }

    SSL_set_fd(ssl, client);		// Gắn con trỏ ssl với socket
    int err = SSL_connect(ssl);	// Tạo kết nối ssl
    if (err <= 0) {
        printf("Error creating SSL connection.  err=%x\n", err);
        return -1;
    }

    char req[] = "GET /videos/ecard.mp4 HTTP/1.1\r\nHost: lebavui.github.io\r\nConnection: close\r\n\r\n";
    SSL_write(ssl, req, strlen(req));

    char buf[2048];
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
}