#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main(int argc, char **argv) {
    int x = add(3, 4);
    int y = factorial(5);
    printf("add: %d, factorial: %d\n", x, y);
    return 0;
}
