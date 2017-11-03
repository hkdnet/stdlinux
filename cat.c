#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

static void do_cat(FILE* f);
static void do_stdin_cat();
static void die(const char *s);

int main(int argc, char const* argv[])
{
    int i;
    FILE* f;
    if (argc < 2) {
        do_stdin_cat();
    } else {
        for (i = 1; i < argc; i++) {
            f = fopen(argv[i], "r");
            if (f == NULL) die(argv[i]);
            do_cat(f);
            if (fclose(f) == EOF) die(argv[i]);
        }
    }
    return 0;
}

#define BUFFER_SIZE 2048

static void
do_stdin_cat()
{
    unsigned char buf[BUFFER_SIZE];
    int n;

    char *s = "stdin";

    for(;;) {
        n = read(STDIN_FILENO, buf, sizeof buf);
        if (n < 0) die(s);
        if (n == 0) break;
        if (write(STDOUT_FILENO, buf, n) < 0) die(s);
    }
}
static void
do_cat(FILE *f)
{
    char c;
    while((c = fgetc(f)) != EOF) {
        if (putchar(c) < 0) exit(1);
    }
};

static void
die(const char *s)
{
    perror(s);
    exit(1);
}
