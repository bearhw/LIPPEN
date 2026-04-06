#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  // For uintptr_t
#include <unistd.h>



int foo(int a) {
    printf(" I am foo %p\n", (void *)&foo);     
    return a * 2;
}

int bar(int b) {
    printf(" I am bar %p\n", (void *)&bar);     
    return b + 1;
}

int main() {
    int x = 5;
    int y = foo(x);
    int z = bar(y);
    printf("this is z %i", z);
    return 0;
}