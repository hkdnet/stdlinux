#include <stdio.h>
#include <stdlib.h>

#define _GNU_SOURCE
#include <getopt.h>

static void do_head(FILE* f, long nlines);

#define DEFAULT_N_LINES 10

static struct option longopts[] = {
    {"lines", required_argument, NULL, 'n'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0}
};

/**
 * Read from stdin, for N lines
 * Usage: head N
 */
int main(int argc, char * const argv[])
{
    long n = DEFAULT_N_LINES;
    int opt;
    while((opt = getopt_long(argc, argv, "n:", longopts, NULL)) != -1) {
        switch(opt) {
            case 'n':
                n = atol(optarg);
                break;
            case 'h':
                printf("Usage: %s [-n lines] [FILE ...]\n", argv[0]);
                exit(0);
            case '?':
                fprintf(stderr, "Usage: %s [-n lines] [FILE ...]\n", argv[0]);
                exit(1);
        }
    }

    if (argc == optind) {
        do_head(stdin, n);
        return 0;
    }

    int i;
    for(i = optind; i < argc; i++) {
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
