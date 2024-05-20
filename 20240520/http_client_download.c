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

int main() 
{
    // Download file http://storage.googleapis.com/it4060_samples/ecard.mp4

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Phan giai ten mien
    struct addrinfo *info;
    if (getaddrinfo("storage.googleapis.com", "http", NULL, &info)) {
        puts("getaddrinfo() failed.");
        return 1;
    }

    int ret = connect(client, info->ai_addr, info->ai_addrlen);
    if (ret == -1) {
        perror("connect() failed");
        return 1;
    }

    char req[] = "GET /it4060_samples/ecard.mp4 HTTP/1.1\r\nHost: storage.googleapis.com\r\nConnection: close\r\n\r\n";
    send(client, req, strlen(req), 0);

    char buf[2048];
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

    printf("Done.");
}