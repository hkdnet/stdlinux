#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

static void do_cat(FILE* f);
static void do_cat_with_invisibles(FILE* f);
static void do_cat_without_invisibles(FILE* f);
static void die(const char *s);
static int show_invisibles = 0;

int main(int argc, char * const argv[])
{
    int i;
    FILE* f;
    int opt;
    while((opt = getopt(argc, argv, "e")) != -1) {
        switch(opt) {
            case 'e':
                show_invisibles = 1;
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-e] [FILE ...]\n", argv[0]);
                exit(1);
        }
    }
    if (optind == argc) {
        do_cat(stdin);
    } else {
        for (i = optind; i < argc; i++) {
            f = fopen(argv[i], "r");
            if (f == NULL) die(argv[i]);
            do_cat(f);
            if (fclose(f) == EOF) die(argv[i]);
        }
    }
    return 0;
}

static void
do_cat(FILE *f)
{
    if (show_invisibles) do_cat_with_invisibles(f);
    else do_cat_without_invisibles(f);
}

static void
do_cat_without_invisibles(FILE *f)
{
    char c;
    while((c = fgetc(f)) != EOF) {
        if (putchar(c) < 0) exit(1);
    }
};

static void
do_cat_with_invisibles(FILE *f)
{
    char c;
    while((c = fgetc(f)) != EOF) {
        if (c == '\t') {
            if (putchar('\\') < 0) exit(1);
            if (putchar('t') < 0) exit(1);
        }
        else if (c == '\n') {
            if (putchar('$') < 0) exit(1);
            if (putchar('\n') < 0) exit(1);
        }
        else {
            if (putchar(c) < 0) exit(1);
        }
    }
};

static void
die(const char *s)
{
    perror(s);
    exit(1);
}
