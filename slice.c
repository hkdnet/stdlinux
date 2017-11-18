#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024

static void do_grep(FILE* f, regex_t* reg);
static void print_usage(const char* name);
static int f_v;

int main(int argc, char* const argv[])
{
    int i;
    int o;
    int ret;
    int flags = REG_EXTENDED | REG_NOSUB | REG_NEWLINE;
    regex_t reg;
    char err[MAX_LINE_LENGTH];

    while((o =getopt(argc, argv, "iv")) != -1) {
        switch(o) {
            case 'i':
                flags = flags | REG_ICASE;
                break;
            case 'v':
                f_v = 1;
                break;
            case '?':
                print_usage(argv[0]);
                exit(1);
        }
    }

    if (optind >= argc) {
        print_usage(argv[0]);
        exit(1);
    }

    ret = regcomp(&reg, argv[optind], flags);
    if (ret) {
        regerror(ret, &reg, err, sizeof err);
        fprintf(stderr, "%s\n", err);
        exit(1);
    }
    optind++;
    if (optind == argc) {
        do_grep(stdin, &reg);
    } else {
        for (i = optind; i < argc; i++) {
            FILE *f;
            f = fopen(argv[i], "r");
            if (!f) {
                perror(argv[i]);
                exit(1);
            }
            do_grep(f, &reg);
        }

    }
    regfree(&reg);
    return 0;
}

static void
do_grep(FILE* f, regex_t* reg)
{
    char buf[MAX_LINE_LENGTH];
    int match;

    while(fgets(buf, MAX_LINE_LENGTH, f)) {
        match = regexec(reg, buf, 0, NULL, 0);
        if (!f_v && match == REG_NOMATCH) continue;
        if (f_v && match != REG_NOMATCH) continue;
        fputs(buf, stdout);
    }
};

static void
print_usage(const char* name)
{
    fprintf(stderr, "Usage: %s [-iv] pattern [FILE ...]\n", name);
}
