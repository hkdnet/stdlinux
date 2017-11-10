#include <stdio.h>
#include <stdlib.h>

/**
 * Read from stdin, for N lines
 * Usage: head N
 */
int main(int argc, char const* argv[])
{
    int n;
    char c;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s N\n", argv[0]);
        exit(1);
    }
    n = atoi(argv[1]);

    while(n--) {
        for(;;) {
            c = fgetc(stdin);
            if (c == EOF) exit(0);
            if (putchar(c) < 0) exit(1); // error
            if (c == '\n') break; // next line
        }
    }

    return 0;
}
