#include <grp.h>
#include <pwd.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define LINE_BUF_SIZE 1024
#define MAX_REQUEST_BODY_LENGTH 1024
#define TIME_BUF_SIZE 64
#define BLOCK_BUF_SIZE 1024
#define HTTP_MINOR_VERSION req->protocol_minor_verison
#define SERVER_NAME "httpserver"
#define SERVER_VERSION "0.1"
#define USAGE "Usage: %s [--port=n] [--chroot --user=u --group=g] <docroot>\n"
#define MAX_BACKLOG 5
#define DEFAULT_PORT "80"

static int debug_mode = 0;
static struct option longopts[] = {
    {"debug", no_argument, &debug_mode, 1},
    {"chroot", no_argument, NULL, 'c'},
    {"user", no_argument, NULL, 'u'},
    {"group", no_argument, NULL, 'g'},
    {"port", no_argument, NULL, 'p'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0}
};

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

    if (debug_mode) {
        vfprintf(stderr, fmt, ap);
        fputc('\n', stderr);
    }
    else {
        vsyslog(LOG_ERR, fmt, ap);
    }
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
noop_handler(int sig)
{
    ;
}

void
detach_children(void)
{
    struct sigaction act;
    act.sa_handler = noop_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART | SA_NOCLDWAIT;
    if (sigaction(SIGCHLD, &act, NULL) < 0) {
        log_exit("sigaction() failed: %s", strerror(errno));
    }
}

void
install_signal_handlers(void)
{
    trap_signal(SIGPIPE, signal_exit);
    detach_children(); // ここであってる？
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
output_common_header_fields(struct HTTPRequest *req, FILE *out, char *status)
{
    time_t t;
    struct tm *tm;
    char buf[TIME_BUF_SIZE];

    t = time(NULL);
    tm = gmtime(&t);
    if (!tm) log_exit("gemtime failed %s", strerror(errno));
    strftime(buf, TIME_BUF_SIZE, "%a, %d %b %Y %H:%M:%S GMT", tm);
    fprintf(out, "HTTP/1.%d %s\r\n", HTTP_MINOR_VERSION, status);
    fprintf(out, "Date: %s\r\n", buf);
    fprintf(out, "Server: %s/%s\r\n", SERVER_NAME, SERVER_VERSION);
    fprintf(out, "Connection: close\r\n");

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
    output_common_header_fields(req, out, "404 Not Found");
    fprintf(out, "\r\n");
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
        fprintf(out, "%s", buf);
    }
    fflush(out);
    free_fileinfo(info);
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

int
listen_socket(char* port)
{
    struct addrinfo hints, *res, *ai;
    int err;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((err = getaddrinfo(NULL, port, &hints, &res)) != 0)
        log_exit((char*)gai_strerror(err)); // cast????

    for(ai = res; ai; ai = ai->ai_next) {
        int sock;
        sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (sock < 0) continue;

        if(bind(sock, ai->ai_addr, ai->ai_addrlen) < 0) {
            close(sock);
            continue;
        }

        if (listen(sock, MAX_BACKLOG) < 0) {
            close(sock);
            continue;
        }
        freeaddrinfo(res);
        return sock;
    }

    log_exit("falied to listen socket");
    return -1; // NOT REACH
}

void
setup_environment(char* root, char* user, char* group)
{
    struct passwd *pw;
    struct group *gr;

    if (!user || !group) {
        fprintf(stderr, "use both of --user and --group \n");
    }
    gr = getgrnam(group);
    if (!gr) {
        fprintf(stderr, "no such group: %s\n", group);
    }
    if (setgid(gr->gr_gid) < 0) {
        perror("setgid(2)");
        exit(1);
    }
    if (initgroups(user, gr->gr_gid) < 0) {
        perror("initgroups(2)");
        exit(1);
    }
    pw = getpwnam(user);
    if (!pw) {
        fprintf(stderr, "no such user: %s", user);
        exit(1);
    }
    chroot(root);
    if (setuid(pw->pw_uid) < 0) {
        perror("setuid(2)");
        exit(1);
    }

}



void
server_main(int server_fd, char* docroot)
{
    for(;;) {
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof addr;
        int sock;
        int pid;
        sock = accept(server_fd, (struct sockaddr*)&addr, &addrlen);
        if (sock < 0) log_exit("accept(2) failed: %s", strerror(errno));
        pid = fork();
        if (pid < 0) exit(3);
        if (pid ==0) {
            // child
            FILE *inf = fdopen(sock, "r");
            FILE *outf = fdopen(sock, "w");
            service(inf, outf, docroot);
            exit(0);
        }
        close(sock);

    }
}

void
become_daemon()
{
    int n;
    if (chdir("/") < 0) log_exit("chdir(2) failed; %s", strerror(errno));

    freopen("/dev/null", "r", stdin);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    n = fork();

    if(n < 0) log_exit("fork(2) failed: %s", strerror(errno));
    if(n != 0) _exit(0); // parent
    if (setsid() < 0) log_exit("setsid(2) failed: %s", strerror(errno));
}

int main(int argc, char * const argv[])
{
    int server_fd;
    char *port = NULL;
    char *docroot = NULL;
    int do_chroot = 0;
    char *user = NULL;
    char *group = NULL;
    int opt;

    while((opt = getopt_long(argc, argv, "", longopts, NULL)) != -1) {
        switch (opt) {
            case 0:
                break;
            case 'c':
                do_chroot = 1;
                break;
            case 'u':
                user = optarg;
                break;
            case 'g':
                group = optarg;
                break;
            case 'p':
                port = optarg;
                break;
            case 'h':
                fprintf(stdout, USAGE, argv[0]);
                exit(0);
            case '?':
                fprintf(stderr, USAGE, argv[0]);
                exit(1);
        }
    }

    if(optind != argc - 1) {
        fprintf(stderr, USAGE, argv[0]);
        exit(1);
    }
    docroot = argv[optind];

    if (do_chroot) {
        setup_environment(docroot, user, group);
        docroot = "";
    }
    install_signal_handlers();
    server_fd = listen_socket(port);
    if (!debug_mode) {
        openlog(SERVER_NAME, LOG_PID|LOG_NDELAY, LOG_DAEMON);
        become_daemon();
    }
    server_main(server_fd, docroot);
    return 0;
}
