#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static void do_tail(FILE* f, int nlines);

#define DEFAULT_N_LINES 10
#define MAX_LINE_LENGTH 1024

int main(int argc, char * const argv[])
{
    int i;
    int c;
    int nlines = DEFAULT_N_LINES;
    while((c = getopt(argc, argv, "n:")) != -1) {
        switch (c) {
            case 'n':
                nlines = atoi(optarg);
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-n NLIINES] [FILE ...]", argv[0]);
                exit(1);
        }
    }

    if (optind == argc) {
        do_tail(stdin, nlines);
        return 0;
    }

    for(i = optind; i < argc; i++) {
        FILE* f = fopen(argv[i], "r");
        if (!f) {
            perror("open");
            exit(1);
        }
        do_tail(f, nlines);
        fclose(f);
    }

    return 0;
}

static void
do_tail(FILE* f, int nlines)
{
    int i;
    int cur = 0;
    long linecnt = 0;
    char* buf;
    void* ptr = malloc(sizeof(char) * MAX_LINE_LENGTH * nlines);
    if (!ptr) {
        perror("malloc");
        exit(1);
    }
    buf = (char *)ptr;
    memset(buf, 0, sizeof(char) * MAX_LINE_LENGTH * nlines);

    while(fgets(buf + (MAX_LINE_LENGTH *cur), MAX_LINE_LENGTH, f)) {
        linecnt++;
        cur = linecnt % nlines;
    }

    cur = linecnt % nlines;

    if (linecnt >= nlines) {
        for(i = cur; i < nlines; i++) {
            printf("%s", buf + (i * MAX_LINE_LENGTH));
        }
    }
    for(i = 0; i < cur; i++) {
        printf("%s", buf + (i * MAX_LINE_LENGTH));
    }

    free(buf);
}
