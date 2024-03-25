#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) 
{
    char buf[2048];
    int pos = 0;

    char path[256];
    getcwd(path, sizeof(path));
    strcpy(buf, path);
    pos += strlen(path) + 1;
    
    DIR *d = opendir(path);
    struct dirent *dir;
    struct stat st;

    if (d != NULL)
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                stat(dir->d_name, &st);
                printf("%s - %ld bytes\n", dir->d_name, st.st_size);

                strcpy(buf + pos, dir->d_name);
                pos += strlen(dir->d_name) + 1;
                memcpy(buf + pos, &st.st_size, sizeof(st.st_size));
                pos += sizeof(st.st_size);
            }
        }

    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1) {
        perror("connect() failed");
        return 1;
    }

    // Gui sang server
    send(client, buf, pos, 0);

    close(client);

    return 0;
}