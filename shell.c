#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

static int empty_p(char c);
static int next_empty(char* buf, int start, int size);
static int next_char(char* buf, int start, int size);

int main(int argc, char const* argv[])
{
    char buf[MAX_LINE_LENGTH];
    memset(buf, 0x0, (sizeof(char)) * sizeof(buf));
    int status = 0;

    printf("$ "); // prompt
    while(fgets(buf, sizeof(buf), stdin)) {
        int count = 0;
        int cur = 0;
        while((cur = next_char(buf, cur, sizeof(buf))) != -1) {
            ++count;
            cur = next_empty(buf, cur + 1, sizeof(buf));
            if (cur == -1) break;
            ++cur;
        }
        if (count == 0) {
            printf("$ "); // prompt
            continue;
        }
        int pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) { // child
            char** args = malloc(sizeof(char*) * (count + 1));
            args[count] = NULL;
            int idx = 0; // idx
            while((cur = next_char(buf, cur, sizeof(buf))) != -1) {
                args[idx] = buf + cur;
                ++idx;
                cur = next_empty(buf, cur + 1, sizeof(buf));
                if (cur == -1) break;
                buf[cur] = '\0';
                ++cur;
            }

            execvp(args[0], args);
            free(args);
            memset(buf, 0x0, (sizeof(char)) * sizeof(buf));
        }
        else { // parent
            waitpid(pid, &status, 0);
            printf("$ "); // prompt
            if (WIFEXITED(status)) {
                int exitstatus = WEXITSTATUS(status);
                if (exitstatus != 0) fprintf(stderr, "process exited with %d\n", exitstatus);
                continue;
            }
            if (WIFSIGNALED(status)) {
                int sig = WTERMSIG(status);
                fprintf(stderr, "signal: %d\n", sig);
                continue;
            }
            fprintf(stderr, "abnormal exit\n");
            exit(status);
        }
    }
    return 0;
}

static int empty_p(char c)
{
    if (c == '\0') return 1;
    if (c == ' ') return 1;
    if (c == '\n') return 1;
    return 0;
}

static int
next_empty(char* buf, int start, int size)
{
    int i;
    for(i = start; i < size; i++) {
        if (empty_p(buf[i])) return i;
    }
    return -1;
}

static int
next_char(char* buf, int start, int size)
{
    int i;
    for(i = start; i < size; i++) {
        if (!empty_p(buf[i])) return i;
    }
    return -1;
}
