#include <stdio.h>
#include <stdlib.h>
/**
 * SYNOPSIS
 *      #include <stdio.h>
 *
 *      size_t
 *      fread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
 *
 *      size_t
 *      fwrite(const void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);
 *
 *
 * RETURN VALUES
 *      The functions fread() and fwrite() advance the file position indicator for the stream
 *      by the number of bytes read or written.  They return the number of objects read or
 *      written.  If an error occurs, or the end-of-file is reached, the return value is a
 *      short object count (or zero).
 *
 *      The function fread() does not distinguish between end-of-file and error; callers must
 *      use feof(3) and ferror(3) to determine which occurred.  The function fwrite() returns
 *      a value less than nitems only if a write error has occurred.
 */

static void do_cat(FILE* f);

int main(int argc, char const* argv[])
{
    do_cat(stdin);
    return 0;
}

#define BUFFER_SIZE 2048

static void
do_cat(FILE* f)
{
    char buf[BUFFER_SIZE];
    int n;
    for(;;) {
        n = fread(buf, sizeof(char), sizeof buf / sizeof(char), f);
        if (ferror(f)) {
            perror("cat_f");
            exit(1);
        }
        fwrite(buf, sizeof(char), n, stdout);
        if (feof(f)) {
            return;
        }
    }
}
