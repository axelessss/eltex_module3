#include <stdio.h>
#include <string.h>

#define N 200

int main(int argc, char *argv[])
{
    char string[N];

    for(int i = 1; i < argc; i++)
    {
        strcat(string, argv[i]);
    }

    printf("Finally: %s", string);
    return 0;
}