#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int sum = 0;
    int num;

    for(int i = 1; i < argc; i++)
    {
        num = atoi(argv[i]);
        sum+=num;
    }

    printf("Sum: %d", sum);
    return 0;
}