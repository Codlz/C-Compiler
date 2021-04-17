#include "stdio.h"

int main() {
    int a = 1 + 1;
    int b = 2 + 2;
    double c = 1.05;
    char s[] = "jisuanji";

    if (a > b) {
        b = a;
    }
    else if (a < b) {
        a = b;
    }
    
    if (c == 0) {
        printf("%f", c);
    }

    return 0;
}