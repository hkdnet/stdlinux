#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024
#define MATCH_SIZE 1

static void do_slice(FILE* f, regex_t* reg);
static void print_usage(const char* name);
static int f_v;

int main(int argc, char* const argv[])
{
    int i;
    int o;
    int ret;
    int flags = REG_EXTENDED | REG_NEWLINE;
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
        do_slice(stdin, &reg);
    } else {
        for (i = optind; i < argc; i++) {
            FILE *f;
            f = fopen(argv[i], "r");
            if (!f) {
                perror(argv[i]);
                exit(1);
            }
            do_slice(f, &reg);
        }

    }
    regfree(&reg);
    return 0;
}

static void
do_slice(FILE* f, regex_t* reg)
{
    char buf[MAX_LINE_LENGTH];
    regmatch_t pmatch[MATCH_SIZE];
    int match;
    while(fgets(buf, MAX_LINE_LENGTH, f)) {
        match = regexec(reg, buf, MATCH_SIZE, pmatch, 0);
        if (!f_v && match == REG_NOMATCH) continue;
        if (f_v && match != REG_NOMATCH) continue;
        for (int i = 0; i < MATCH_SIZE; i++) {
            regmatch_t tmp = pmatch[i];
            int cur = tmp.rm_so;
            int size = tmp.rm_eo - tmp.rm_so;
            fwrite(&buf[cur], sizeof(char), size, stdout);
        }
        printf("\n");
    }
};

static void
print_usage(const char* name)
{
    fprintf(stderr, "Usage: %s [-iv] pattern [FILE ...]\n", name);
}
