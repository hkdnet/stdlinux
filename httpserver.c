#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/errno.h>

#define LINE_BUF_SIZE 1024

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

void
read_http_request_first_line(struct HTTPRequest *req, FILE* in)
{
    char buf[LINE_BUF_SIZE];
    if(!fgets(buf, LINE_BUF_SIZE, in))
        log_exit("Failed to read a line");
    if(!strcmp(buf, "\r\n"))
        log_exit("Not a http request? The first line is...: %s", buf);

    char *p, *path;
    p = strchr(buf, ' ');
    if (!p) log_exit("No space??");
    *p++ = '\0';
    req->method = xmalloc(p - buf);
    strcpy(req->method, buf);

    path = p;
    p = strchr(path, ' ');
    if (!p) log_exit("No space between path and version??");
    *p++ = '\0';
    req->path = xmalloc(p - path);
    strcpy(req->path, path);
}

struct HTTPHeaderField*
read_header_field(FILE* in)
{
    char buf[LINE_BUF_SIZE];
    if(!fgets(buf, LINE_BUF_SIZE, in))
        log_exit("Failed to read a line for header");
    if(!strcmp(buf, "\r\n")) return NULL;
    if(!strcmp(buf, "\n")) return NULL;
    struct HTTPHeaderField *h;
    char *p;
    p = strchr(buf, ':');
    if (!p)
        log_exit("Parse error on request header field: %s", buf);

    *p++ = '\0';
    h = xmalloc(sizeof(struct HTTPHeaderField));
    h->name = xmalloc(p - buf);
    strcpy(h->name, buf);

    p += strspn(p, " \t");
    h->value = xmalloc(strlen(p) + 1);
    strcpy(h->value, p);

    return h;
}

struct HTTPRequest*
read_request(FILE* in)
{
    struct HTTPRequest *req;
    struct HTTPHeaderField *h;
    req = xmalloc(sizeof(struct HTTPRequest));
    read_http_request_first_line(req, in);
    req->header = NULL;
    while((h = read_header_field(in))) {
        h->next = req->header;
        req->header = h;
    }

    return req;
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
    // free(req->body);
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

