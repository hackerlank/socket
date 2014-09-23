#include "../base/unp.h"

int Tcp_listen(const char* host, const char* serv, socklen_t* addrlenp);

pid_t child_make(int i, int listenfd, int addrlen);

void child_main(int i, int listenfd, int addrlen);

void str_echo(int sockfd);

// 这是 accept 预先开启一组进程
static int nchildren;
static pid_t *pids;

void sig_int_killchildren(int signo);

// 这里是关于 accept 开启进程后加锁
static struct flock lock_it, unlock_it;
static int lock_fd = 1;

void file_lock_init(char* pathname);
void file_lock_wait();
void file_lock_release();
void child_main_for_file_lock(int i, int listenfd, int addrlen);
