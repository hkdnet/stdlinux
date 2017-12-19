#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/errno.h>

void
log_exit(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
    exit(1);
}

void*
xmalloc(size_t sz)
{
    void *p;
    p = malloc(sz);
    if(!p) log_exit("failed to allocate memory");
    return p;
}

void
signal_exit(int sig)
{
    log_exit("exit by signal %d", sig);
}

void
trap_signal(int sig, void (*handler)(int))
{
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    if(sigaction(sig, & act, NULL) < 0)
        log_exit("sigaction() failed: %s", strerror(errno));
}

void
install_signal_handlers(void)
{
    trap_signal(SIGPIPE, signal_exit);
}

