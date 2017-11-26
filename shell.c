#include <stdio.h>

#define MAX_LINE_LENGTH 1024

int main(int argc, char const* argv[])
{
    char buf[MAX_LINE_LENGTH];

    printf("$ "); // prompt
    while(fgets(buf, sizeof(buf), stdin)) {
        printf("%s", buf);
        printf("$ "); // prompt
    }
    return 0;
}
