#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char** line;
    long len;
} line_t;

static void do_tail(FILE* f);
static void print_line_t(FILE* f, line_t l);

#define DEFAULT_N_LINES 10

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
    int cur;
    line_t buf[DEFAULT_N_LINES];

    char* line;
    size_t linecap;
    ssize_t linelen;
    while((linelen = getline(&line, &linecap, f)) >= 0) {
        line_t tmp = { &line, linelen };
        buf[linecnt % DEFAULT_N_LINES] = tmp;
        linecnt++;
    }

    if (!feof(f)) exit(1);

    cur = linecnt % DEFAULT_N_LINES;

    if (linecnt >= DEFAULT_N_LINES) {
        for(i = cur; i < DEFAULT_N_LINES; i++) {
            print_line_t(stdout, buf[i]);
        }
    }
    for(i = 0; i < cur; i++) {
        print_line_t(stdout, buf[i]);
    }

}

static void
print_line_t(FILE* f, line_t l)
{
    fwrite(l.line, sizeof(char), l.len, f);
}
