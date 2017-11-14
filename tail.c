#include <stdio.h>
#include <stdlib.h>

static void do_tail(FILE* f);

#define DEFAULT_N_LINES 10

typedef struct {
    char* line;
    long len;
} line_t;

int main(int argc, char * const argv[])
{
    do_tail(stdin);

    return 0;
}

static void
do_tail(FILE* f)
{
    int i;
    int linecnt = 0;
    line_t buf[DEFAULT_N_LINES];

    char* line;
    size_t linecap;
    ssize_t linelen;
    while((linelen = getline(&line, &linecap, f)) >= 0) {
        line_t tmp = { line, linelen };
        buf[linecnt % DEFAULT_N_LINES] = tmp;
        linecnt++;
    }

    if (!feof(f)) exit(1);

    if (linecnt >= DEFAULT_N_LINES) {
        for(i = linecnt; i < DEFAULT_N_LINES; i++) {
            fwrite(buf[i].line, sizeof(char), buf[i].len, stdout);
        }
    }
    for(i = 0; i < linecnt; i++) {
        fwrite(buf[i].line, sizeof(char), buf[i].len, stdout);
    }

}
