#include <stdio.h>
#include <pthread.h>
#include <malloc.h>
#include <unistd.h>

int sum = 0;

void *thread_proc(void *);

int main() {
    pthread_t tid;
    pthread_create(&tid, NULL, thread_proc, NULL);
    
    printf("New thread created.\n");

    pthread_join(tid, NULL);

    printf("sum = %d\n", sum);
}
 
void *thread_proc(void *args) {
    sum = 0;
    printf("Start computing\n");
    for (int i = 1; i <= 10; i++) {
        sleep(1);
        sum += i * i;
    }
    printf("Done.\n");
}