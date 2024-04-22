#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    if (fork() == 0) {
        printf("Child process 1 started, pid = %d\n", getpid());
        while (1) {
            sleep(1);
            printf("Child process 1 running\n");
        }
        exit(0);
    }

    if (fork() == 0) {
        printf("Child process 2 started, pid = %d\n", getpid());
        while (1) {
            sleep(1);
            printf("Child process 2 running\n");
        }
        exit(0);
    }

    sleep(10);
    killpg(0, SIGKILL);
    printf("Main process done.\n");
    return 0;
}