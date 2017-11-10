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
    if (argc < 2) {
        fprintf(stderr, "Usage: %s n [file file...]\n", argv[0]);
        exit(1);
    }
    n = atol(argv[1]);

    if (argc == 2) {
        do_head(stdin, n);
        return 0;
    }

    int i;
    for(i = 2; i < argc; i++) {
        FILE* f;
        f = fopen(argv[i], "r");
        if (!f) {
            perror(argv[i]);
            exit(1);
        }
        do_head(f, n);
        fclose(f);
    }


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
