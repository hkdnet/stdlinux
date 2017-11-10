#include <stdio.h>
#include <stdlib.h>


static void do_head(FILE* f, long nlines);

/**
 * Read from stdin, for N lines
 * Usage: head N
 */
int main(int argc, char const* argv[])
{
    long n;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s N\n", argv[0]);
        exit(1);
    }
    n = atol(argv[1]);

    do_head(stdin, n);

    return 0;
}

static void
do_head(FILE* f, long nlines)
{
    char c;
    while(nlines--) {
        for(;;) {
            c = fgetc(f);
            if (c == EOF) exit(0);
            if (putchar(c) < 0) exit(1); // error
            if (c == '\n') break; // next line
        }
    }
}
