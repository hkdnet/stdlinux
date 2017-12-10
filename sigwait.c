#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

typedef struct sigaction sigaction_t;

void handler(int sig)
{
    printf("Catch signal...\nBye :)\n");
}

int main(int argc, char const* argv[])
{
    sigaction_t wait_sigint, old;
    wait_sigint.sa_handler = handler;
    sigemptyset(&wait_sigint.sa_mask);
    wait_sigint.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &wait_sigint, &old) < -1) {
        perror("sigaction");
    }

    pause();
    return 0;
}
