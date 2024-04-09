#include <stdio.h>
#include <string.h>
int main(int argc, char *argv[])
{
    int maxlen = 0;

    printf("%s\n",argv[0]);

    for(int i = 1; i < argc; i++)
    {
        if((int)strlen(argv[i]) > maxlen)
            maxlen = (int)strlen((argv[i]));
    }
    printf("Maximum length: %d", maxlen);
    return 0;
}