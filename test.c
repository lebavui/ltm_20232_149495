#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

void main() {
    char buf[2048];
    int pos = 0;
    int count = 0;

    DIR *d = opendir(".");
    struct dirent *dir;
    struct stat st;

    if (d != NULL)
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                count++;

                stat(dir->d_name, &st);
                printf("%s - %ld bytes\n", dir->d_name, st.st_size);

                strcpy(buf + pos, dir->d_name);
                pos += strlen(dir->d_name) + 1;
                memcpy(buf + pos, &st.st_size, sizeof(st.st_size));
                pos += sizeof(st.st_size);
            }
        }
    puts(buf);
}