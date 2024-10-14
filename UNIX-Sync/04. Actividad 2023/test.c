#include <stdio.h>

void increment() {
    static int count = 0;  // Static variable with initial value 0
    count++;
    printf("Count: %d\n", count);
}

int main() {
    increment();  // Output: Count: 1
    increment();  // Output: Count: 2
    increment();  // Output: Count: 3
    return 0;
}