#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void signalHanlder(int signo) {
    int pid = wait(NULL);
    printf("Child process terminated, pid = %d\n", pid);
}

int main() {
    signal(SIGCHLD, signalHanlder);
    
    if (fork() == 0) {
        printf("Child process started, pid = %d\n", getpid());
        sleep(10);
        printf("Child process done, pid = %d\n", getpid());
        exit(0);
    }

    getchar();
    printf("Main process done.\n");
    return 0;
}