#include <stdio.h>
#include <unistd.h>

int main() {
    int count = 10;
    printf("Process ID: %d\n", getpid());
    while (count > 0) {
        printf("Counting: %d\n", count--);
        sleep(1);
    }
    return 0;
}
