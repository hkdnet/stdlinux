#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#define MAX_LINE_LENGTH 1024

static void do_grep(FILE* f, regex_t* reg);

int main(int argc, char const* argv[])
{
    int i;
    int ret;
    int flags = REG_EXTENDED | REG_NOSUB | REG_NEWLINE;
    regex_t reg;
    char err[MAX_LINE_LENGTH];
    if (argc < 2) {
        fputs("No pattern\n", stderr);
        exit(1);
    }
    ret = regcomp(&reg, argv[1], flags);
    if (ret) {
        regerror(ret, &reg, err, sizeof err);
        fprintf(stderr, "%s\n", err);
        exit(1);
    }
    if (argc == 2) {
        do_grep(stdin, &reg);
    } else {
        for (i = 2; i < argc; i++) {
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
        if (match == REG_NOMATCH) continue;
        fputs(buf, stdout);
    }
};
