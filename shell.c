#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024

int main(int argc, char const* argv[])
{
    char buf[MAX_LINE_LENGTH];
    int status = 0;

    printf("$ "); // prompt
    while(fgets(buf, sizeof(buf), stdin)) {
        int pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) { // child
            execlp("cat", "cat", NULL);
        }
        else { // parent
            printf("$ "); // prompt
            waitpid(pid, &status, 0);
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
