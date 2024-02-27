#include <stdio.h>
#include <arpa/inet.h>

void main() {
    short int N = 8000;
    printf("%d", htons(N));
}