// question 2
#include <stdio.h>

int main() {
    int n;
    // Read integers from standard input until 1 is reached
    while(scanf("%d",&n)!=EOF){

        if(n==1){
            printf("1\n\n");
            break;
        }
        while (n != 1) {
        printf("%d ", n);

        if (n & 1)
            n = n * 3 + 1;
        else
            n >>= 1;
    }

    printf("1\n\n"); // Print the final '1'
    printf("Next number = ");
    }

    return 0;
}
