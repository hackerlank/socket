#include "../base/unp.h"
/* #include <sys/flock.h> */
#include <fcntl.h>

int Tcp_listen(const char* host, const char* serv, socklen_t* addrlenp);

pid_t child_make(int i, int listenfd, int addrlen);

void child_main(int i, int listenfd, int addrlen);

void str_echo(int sockfd);

// 这是 accept 预先开启一组进程
static int nchildren;
static pid_t *pids;

void sig_int_killchildren(int signo);

// 这里是关于 accept 开启进程后文件加锁
static struct flock lock_it, unlock_it;
static int lock_fd = 1;

void file_lock_init(char* pathname);
void file_lock_wait();
void file_lock_release();
void child_main_for_file_lock(int i, int listenfd, int addrlen);
pid_t child_make_for_file_lock(int i, int listenfd, int addrlen);

// 这里是关于 accept 开启线程后的线程加锁
static pthread_mutex_t *mptr;

void pthread_lock_init(char* pathname);	// 记得线程是放置在一个地方的
void pthread_lock_wait();
void pthread_lock_release();
void child_main_for_pthread_lock(int i, int listenfd, int addrlen);	// 如何共有这一段代码
pid_t child_make_for_pthread_lock(int i, int listenfd, int addrlen);
