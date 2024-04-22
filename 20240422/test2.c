#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    if (fork() == 0) {
        printf("Child process 1 started, pid = %d\n", getpid());
        sleep(20);
        printf("Child process done\n");
        exit(0);
    }

    if (fork() == 0) {
        printf("Child process 2 started, pid = %d\n", getpid());
        sleep(10);
        printf("Child process done\n");
        exit(0);
    }

    printf("Waiting for child process\n");
    
    while (wait(NULL) != -1);

    printf("Main process done.\n");
    return 0;
}