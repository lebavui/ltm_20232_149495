#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Before FORK, pid = %d\n", getpid());
    for (int i = 0; i < 5; i++) {
        if (fork() == 0) {
            printf("Child process %d\n", i);
            // Cong viec ben trong tien trinh con
            // ...
            //
            exit(0);
        }
    }

    printf("Done.\n");

    return 0;
} 