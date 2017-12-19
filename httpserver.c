#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/errno.h>

struct HTTPHeaderField {
    char *name;
    char *value;
    struct HTTPHeaderField *next;
};

struct HTTPRequest {
    int protocol_minor_verison;
    char *method;
    char *path;
    struct HTTPHeaderField *header;
    char *body;
    long length;
};

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

struct HTTPRequest*
read_request(FILE* in)
{
    return NULL;
}

void
respond_to(struct HTTPRequest *req, FILE* out, const char* docroot)
{
}

void
free_request(struct HTTPRequest *req)
{
    struct HTTPHeaderField *target, *tmp;
    target = req->header;
    while (target) {
        tmp = target->next;
        free(target->name);
        free(target->value);
        free(target);
        target = tmp;
    }
    free(req->method);
    free(req->path);
    free(req->body);
    free(req);
}

void
service(FILE* in, FILE* out, const char* docroot)
{
    struct HTTPRequest *req;
    req = read_request(in);
    respond_to(req, out, docroot);
    free_request(req);
}

int main(int argc, char const* argv[])
{
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <docroot>\n", argv[0]);
        exit(1);
    }
    install_signal_handlers();
    service(stdin, stdout, argv[1]);
    return 0;
}

