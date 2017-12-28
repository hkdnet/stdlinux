#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/stat.h>

#define LINE_BUF_SIZE 1024
#define MAX_REQUEST_BODY_LENGTH 1024
#define TIME_BUF_SIZE 64
#define BLOCK_BUF_SIZE 1024
#define HTTP_MINOR_VERSION req->protocol_minor_verison
#define SERVER_NAME "httpserver"
#define SERVER_VERSION "0.1"

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

struct FileInfo {
    char *path;
    long size;
    int ok;
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

    if (strncasecmp(p, "HTTP/1.", strlen("HTTP1/")) != 0)
        log_exit("parse error on request line(3): %s", buf);
    p += strlen("HTTP/1.");
    req->protocol_minor_verison = atoi(p);
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

char*
lookup_header_field_value(struct HTTPRequest *req, char *key)
{
    struct HTTPHeaderField *h;
    h = req->header;
    while(h) {
        if (strcasecmp(h->name, key) == 0)
            return h->value;

        h = h->next;
    }
    return NULL;
}

long
content_length(struct HTTPRequest *req)
{
    char *val;
    long len;
    val = lookup_header_field_value(req, "Content-Length");
    if (!val) return 0;
    len = atol(val);
    if (len < 0) log_exit("negative Content-Length value");

    return len;
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
    req->length = content_length(req);

    if (req->length != 0) {
        if(req->length > MAX_REQUEST_BODY_LENGTH)
            log_exit("request body too long");
        req->body = xmalloc(req->length);
        if (fread(req->body, req->length, 1, in) < 1)
            log_exit("failed to reqd request body");

    } else {
        req->body = NULL;
    }

    return req;
}

// TODO: Deny '../../'
char*
build_fspath(const char* docroot, char *urlpath)
{
    char* s;
    s = xmalloc(strlen(docroot) + 1 + strlen(urlpath) + 1);

    sprintf(s, "%s/%s", docroot, urlpath);
    return s;
}

struct FileInfo*
get_file_info(const char* docroot, char *urlpath)
{
    struct FileInfo *info;
    struct stat st;

    info = xmalloc(sizeof(struct FileInfo));
    info->path = build_fspath(docroot, urlpath);
    info->ok = 0;
    if(lstat(info->path, &st) < 0) return info;
    if(!S_ISREG(st.st_mode)) return info;
    info->ok = 1;
    info->size = st.st_size;
    return info;
}

void
free_fileinfo(struct FileInfo *info)
{
    free(info->path);
    free(info);
}

void
not_implemented(struct HTTPRequest *req, FILE* out)
{
    // TODO: return http response
    fprintf(out, "Not implemented yet, sorry.\n");
}

void
not_found(struct HTTPRequest *req, FILE *out)
{
    fprintf(out, "HTTP/1.%d 404 Not Found\n", req->protocol_minor_verison);
}

void
output_common_header_fields(struct HTTPRequest *req, FILE *out, char *status)
{
    time_t t;
    struct tm *tm;
    char buf[TIME_BUF_SIZE];

    t = time(NULL);
    tm = gmtime(&t);
    if (!tm) log_exit("gemtime failed %s", strerror(errno));
    strftime(buf, TIME_BUF_SIZE, "%a, %d %b %Y %H:%M:%S GMT", tm);
    fprintf(out, "HTTP/1.%d: %s\r\n", HTTP_MINOR_VERSION, status);
    fprintf(out, "Date: %s\r\n", buf);
    fprintf(out, "Server: %s/%s\r\n", SERVER_NAME, SERVER_VERSION);
    fprintf(out, "Connection: close\r\n");

}

char*
guess_content_type(struct FileInfo *info)
{
    // TODO: impl...
    return "text/html";
}

void
do_file_response(struct HTTPRequest *req, FILE* out, const char* docroot)
{
    struct FileInfo *info;
    info = get_file_info(docroot, req->path);
    if (!info->ok) {
        free_fileinfo(info);
        not_found(req, out);
        return;
    }

    output_common_header_fields(req, out, "200 OK");
    fprintf(out, "Content-Length: %ld\r\n", info->size);
    fprintf(out, "Content-Type: %s\r\n", guess_content_type(info));
    fprintf(out, "\r\n");
    if (strcmp(req->method, "HEAD") == 0) {
        fflush(out);
        free_fileinfo(info);
        return;
    }
    int fd;
    char buf[BLOCK_BUF_SIZE];
    ssize_t n;
    fd = open(info->path, O_RDONLY);
    if (fd < 0)
        log_exit("failed to open %s: %s", info->path, strerror(errno));

    for (;;) {
        n = read(fd, buf, BLOCK_BUF_SIZE);
        if (n < 0)
            log_exit("failed to read %s: %s", info->path, strerror(errno));
        if (n == 0) break;
        if(fwrite(buf, n, 1, out) < n)
            log_exit("failed to write to socket: %s", strerror(errno));
    }
}

void
respond_to(struct HTTPRequest *req, FILE* out, const char* docroot)
{
    if (strcmp(req->method, "GET") == 0)
        do_file_response(req, out, docroot);
    else
        not_implemented(req, out);
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
