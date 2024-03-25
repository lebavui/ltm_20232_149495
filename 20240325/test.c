#include "headers.h"
#include <sys/select.h>

int main() {
    fd_set fdread;

    int socket1 = 65;
    int socket2 = 67;

    FD_ZERO(&fdread);
    FD_SET(socket1, &fdread);
    FD_SET(socket2, &fdread);

    return 0;
}
