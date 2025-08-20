
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "lib/jieba.h"

#ifndef MAX_CHAR_LEN
#define MAX_CHAR_LEN 1024
#endif

#ifndef LISTEN_MAX
#define LISTEN_MAX 10
#endif

#ifndef MAX_EVENTS
#define MAX_EVENTS 10
#endif

extern Jieba jieba_load_handle (int argc, char **argv);
extern char *jieba_cut (Jieba handle, char *s);
// extern void  jieba_free_handle (Jieba handle);
Jieba handle;
int   socket_fd;

void clean_up (int signum) {
    fprintf (stderr, "\n[signum=%d] clean up... ", signum);
    FreeJieba (handle);
    close (socket_fd);
    fprintf (stderr, "done.\n");
    exit (EXIT_SUCCESS);
}

int main (int argc, char **argv) {
    if (argc < 7) {
        fprintf (stderr, "Usage:\n\
  %s <DICT> <HMM> <USER_DICT> <IDF> <STOP_WORD> PORT\n",
                 argv[0]);
        exit (EXIT_FAILURE);
    }

    int                connect_fd;
    struct sockaddr_in servaddr;
    char               buff[MAX_CHAR_LEN];
    int                n, recv_len;
    int                ready;
    struct pollfd     *pfds;
    char              *words;
    // nfds_t             nfds;
    // char               *s;

    // 初始化Socket
    if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf (stderr, "create socket error: %s(errno: %d)\n",
                 strerror (errno), errno);
        exit (0);
    }
    // 初始化
    memset (&servaddr, 0, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    // IP地址设置成INADDR_ANY，让系统自动获取本机的IP地址。
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servaddr.sin_port        = htons (atoi (argv[6]));

    // 将本地地址绑定到所创建的套接字上
    if (bind (socket_fd, (struct sockaddr *) &servaddr, sizeof (servaddr)) ==
        -1) {
        fprintf (stderr, "bind socket error: %s (errno: %d)\n",
                 strerror (errno), errno);
        exit (0);
    } // 开始监听是否有客户端连接
    if (listen (socket_fd, LISTEN_MAX) == -1) {
        fprintf (stderr, "listen socket error: %s(errno: %d)\n",
                 strerror (errno), errno);
        exit (0);
    } else {
        fprintf (stderr, "[INFO] Listening on port %s\n", argv[6]);
    }

    fcntl (socket_fd, F_SETFL, O_NONBLOCK);

    // ================================================================
    fprintf (stderr, "[INFO] Loading jiaba handle... ");
    handle = jieba_load_handle (argc, argv);
    fprintf (stderr, "done.\n");
    signal (SIGINT, clean_up);
    signal (SIGTERM, clean_up);
    // ================================================================

    fprintf (stderr, "[INFO] Waiting for client...\n");

    struct epoll_event ev, events[MAX_EVENTS];
    int                conn_sock, nfds, epfd;

    epfd = epoll_create1 (0);
    if (epfd == -1) {
        perror ("epoll_create1");
        exit (EXIT_FAILURE);
    }

    ev.events  = EPOLLIN;
    ev.data.fd = socket_fd;
    if (epoll_ctl (epfd, EPOLL_CTL_ADD, socket_fd, &ev) == -1) {
        perror ("epoll_ctl: socket_fd");
        exit (EXIT_FAILURE);
    }

    int j = 0;
    for (;;) {
        nfds = epoll_wait (epfd, events, MAX_EVENTS, -1);
        fprintf (stderr, "[%d]\n", j);
        if (nfds == -1) {
            perror ("epoll_wait");
            exit (EXIT_FAILURE);
        }

        int i = 0;
        for (i = 0; i < nfds; i++) {
            int actFd = events[i].data.fd;
            if (actFd == socket_fd) {
                conn_sock = accept (socket_fd, (struct sockaddr *) NULL, NULL);
                if (conn_sock == -1) {
                    perror ("accept");
                    exit (EXIT_FAILURE);
                }
                fcntl (conn_sock, F_SETFL, O_NONBLOCK);

                // setnonblocking (conn_sock);
                ev.events  = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;

                if (epoll_ctl (epfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    perror ("epoll_ctl: conn_sock");
                    exit (EXIT_FAILURE);
                }
            } else if (events[i].events & EPOLLIN) {
                for (;;) {
                    bzero (buff, sizeof (buff));
                    n = read (events[i].data.fd, buff, sizeof (buff));
                    if (n <= 0 /* || errno == EAGAIN */) {
                        break;
                    } else {
                        buff[strcspn (buff, "\n")] = '\0';
                        fprintf (stderr, "    recv (client #%d): '%s'\n", conn_sock,
                                 buff);

                        words = jieba_cut (handle, buff);
                        fprintf (stderr, "    seg: %s\n", words);

                        write (events[i].data.fd, words, strlen (words));
                    }
                }
            }

            /* check if the connection is closing */
            if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
                fprintf (stderr, "[INFO] connection closed\n");
                epoll_ctl (epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                close (events[i].data.fd);
                continue;
            }
        }
        j++;
    }
    free (words);
    close (socket_fd);
    FreeJieba (handle);
    return EXIT_SUCCESS;
}
