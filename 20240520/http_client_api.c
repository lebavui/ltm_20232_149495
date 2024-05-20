#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int main() 
{
    // http://api.weatherapi.com/v1/current.json?key=48bab0abac324847925230945232306&q=Hanoi&aqi=no

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct addrinfo *info;
    if (getaddrinfo("api.weatherapi.com", "http", NULL, &info)) {
        puts("getaddrinfo() failed.");
        return 1;
    }

    int ret = connect(client, info->ai_addr, info->ai_addrlen);
    if (ret == -1) {
        perror("connect() failed");
        return 1;
    }

    char req[] = "GET /v1/current.json?key=48bab0abac324847925230945232306&q=Hanoi&aqi=no HTTP/1.1\r\nHost: api.weatherapi.com\r\n\r\n";
    send(client, req, strlen(req), 0);

    char buf[2048];
    ret = recv(client, buf, sizeof(buf), 0);
    if (ret <= 0) {
        close(client);
        return 1;
    }
    
    buf[ret] = 0;
    printf("%d bytes received\n%s\n", ret, buf);

    char *pos1 = strstr(buf, "temp_c");
    char *pos2 = strchr(pos1, ':') + 1;
    char *pos3 = strchr(pos2, ',');
    
    char temp_buf[32] = { 0 };
    memcpy(temp_buf, pos2, pos3 - pos2);
    float temp = atof(temp_buf);

    printf("Gia tri nhiet do (C) = %f\n", temp);

    close(client);
}