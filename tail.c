#include <stdio.h>
#include <stdlib.h>

static void do_tail(FILE* f);

#define DEFAULT_N_LINES 10
#define MAX_LINE_LENGTH 1024

int main(int argc, char * const argv[])
{
    do_tail(stdin);

    return 0;
}

static void
do_tail(FILE* f)
{
    int i;
    int cur = 0;
    long linecnt = 0;
    char buf[DEFAULT_N_LINES][MAX_LINE_LENGTH] ;

    while(fgets(buf[cur], MAX_LINE_LENGTH, f)) {
        linecnt++;
        cur = linecnt % DEFAULT_N_LINES;
    }

    cur = linecnt % DEFAULT_N_LINES;

    if (linecnt >= DEFAULT_N_LINES) {
        for(i = cur; i < DEFAULT_N_LINES; i++) {
            printf("%s", buf[i]);
        }
    }
    for(i = 0; i < cur; i++) {
        printf("%s", buf[i]);
    }

}
