#ifndef _SVR_HEADER_H__
#define _SVR_HEADER_H__

#include "../base/unp.h"
/* #include <sys/flock.h> */
#include <fcntl.h>

int Tcp_listen(const char* host, const char* serv, socklen_t* addrlenp);

pid_t child_make(int i, int listenfd, int addrlen);

void child_main(int i, int listenfd, int addrlen);

void str_echo(int sockfd);

// 这是 accept 预先开启一组进程
void sig_int_killchildren(int signo);
void sig_int_for_pipechildren(int signo);

// 这里是关于 accept 开启进程后文件加锁
void file_lock_init(char* pathname);
void file_lock_wait();
void file_lock_release();
void child_main_for_file_lock(int i, int listenfd, int addrlen);
pid_t child_make_for_file_lock(int i, int listenfd, int addrlen);

// 这里是关于 accept 开启进程后的线程加锁
void pthread_lock_init(char* pathname);	// 记得线程是放置在一个地方的，所以这个给予了一个路径
void pthread_lock_wait();
void pthread_lock_release();
void child_main_for_pthread_lock(int i, int listenfd, int addrlen);	// 如何共有这一段代码
pid_t child_make_for_pthread_lock(int i, int listenfd, int addrlen);

// 预先派生自进程，通过管道传递
struct Child{
	pid_t	child_pid;
	int		child_pipefd;
	int		child_status;
	long 	child_count;
};

pid_t child_make_for_pipe(int i, int listenfd, int addrlen);
void child_main_for_pipe(int i, int listenfd, int addrlen);

#endif
