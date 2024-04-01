// question 1
#include <stdio.h>
int main()
{
    int n = 100; // Default value for n
    // printf("here it is program 1\n\n");
    // Compute the sequence while n is not 1
    // scanf("%d",&n);
    while (n != 1)
    {
        printf("%d ", n);

        if (n & 1)
            n = n * 3 + 1;
        else
            n >>= 1;
    }

    printf("1\n"); // Print the final '1'

    return 0;
}
