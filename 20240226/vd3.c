#include <stdio.h>
#include <string.h>

void main() {
    char resp[] = "227 Entering Passive Mode (213,229,112,130,216,4)";
    char *pos = strchr(resp, '(');
    char *p1 = strtok(pos, "(),");
    char *p2 = strtok(NULL, "(),");
    char *p3 = strtok(NULL, "(),");
    char *p4 = strtok(NULL, "(),");
    printf("IP: %s.%s.%s.%s\n", p1, p2, p3, p4);
    char *i1 = strtok(NULL, "(),");
    char *i2 = strtok(NULL, "(),");
    int port = atoi(i1) * 256 + atoi(i2);
    printf("Port: %d\n", port);
}