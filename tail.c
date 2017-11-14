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
    int idx = 0;
    line_t buf[DEFAULT_N_LINES];

    char* line;
    size_t linecap;
    ssize_t linelen;
    while((linelen = getline(&line, &linecap, f)) >= 0) {
        line_t tmp = { line, linelen };
        buf[idx % DEFAULT_N_LINES] = tmp;
        idx++;
    }

    if (!feof(f)) exit(1);

    if (idx > DEFAULT_N_LINES) {
        for(i = idx; i < DEFAULT_N_LINES; i++) {
            fwrite(buf[i].line, sizeof(char), buf[i].len, stdout);
        }
    }
    for(i = 0; i < idx; i++) {
        fwrite(buf[i].line, sizeof(char), buf[i].len, stdout);
    }

}
