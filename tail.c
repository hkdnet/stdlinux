#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static void do_tail(FILE* f, int nlines);
typedef struct line {
    char* line;
    size_t size;
} line_t;

#define DEFAULT_N_LINES 10
#define INIT_LINE_LENGTH 1024

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
    line_t* buf = malloc(sizeof(line_t) * nlines);
    for (i = 0; i < nlines; i++) {
        void* ptr = malloc(sizeof(char) * INIT_LINE_LENGTH);
        if (!ptr) {
            perror("malloc");
            exit(1);
        }
        line_t tmp = { (char *)ptr, INIT_LINE_LENGTH };
        buf[i] = tmp;
    }

    while(fgets(buf[cur].line, buf[cur].size, f)) {
        linecnt++;
        cur = linecnt % nlines;
    }

    cur = linecnt % nlines;

    if (linecnt >= nlines) {
        for(i = cur; i < nlines; i++) {
            printf("%s", buf[i].line);
        }
    }
    for(i = 0; i < cur; i++) {
        printf("%s", buf[i].line);
    }


    for (i = 0; i < nlines; i++) {
        free(buf[i].line);
    }
    free(buf);
}
