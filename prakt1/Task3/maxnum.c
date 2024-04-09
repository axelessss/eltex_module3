#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int maxnum = 0;
    int num;

    for(int i = 1; i < argc; i++)
    {
        num = atoi(argv[i]);
        if(num > maxnum)
            maxnum = num;
    }
    printf("Maximum number: %d", maxnum);
    return 0;
}