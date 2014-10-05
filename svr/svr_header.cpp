#include "svr_header.h"
#include <sys/mman.h>

int nchildren;
pid_t *pids;

struct flock lock_it, unlock_it;
int lock_fd = 1;
pthread_mutex_t *mptr;

Child *cptr;

// 此处的 host 也可以是 ipaddress，如果为 null 则是本机 127.0.0.1，szServName 也可以是 port
int Tcp_listen(const char* host, const char* serv, socklen_t* addrlenp)
{
	int listenfd, n;
	const int on = 1;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	// getaddrinfo 返回错误值，打印出来
	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		err_quit("tcp_listen error for %s, %s: %s", host, serv, gai_strerror(n));

	ressave = res;

	do {
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0)	// 在 socket 中，如果这个函数使用返回值来传递数据，那么返回值小于零，往往是错；如果返回值是返回错误码的，那么为零，表明正确；更好的办法是统一起来，比如多返回值，告诉我是否成功
			continue;

		Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
			break;	// bind success

		Close(listenfd);
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)
		err_sys("tcp_listen error for %s, %s", host, serv);

	Listen(listenfd, LISTENQ);

	if (addrlenp)
		*addrlenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return (listenfd);
}

void child_main(int i, int listenfd, int addrlen)
{
	int connfd;
	socklen_t clilen;
	struct sockaddr* cliaddr;

	cliaddr = (sockaddr*)Malloc(addrlen);
	printf("%d : child %ld starting\n", i, (long)getpid());
	for (;;)
	{
		clilen = addrlen;
		connfd = Accept(listenfd, cliaddr, &clilen);
		printf("%d : %ld : Accept one client \n", i, (long)getpid());
		str_echo(connfd);
		Close(connfd);
		printf("%d : %ld : Close one client \n", i, (long)getpid());
	}
}

void child_main_for_file_lock(int i, int listenfd, int addrlen)
{
	int connfd;
	socklen_t clilen;
	struct sockaddr* cliaddr;

	cliaddr = (sockaddr*)Malloc(addrlen);
	printf("%d : child %ld starting\n", i, (long)getpid());
	for (;;)
	{
		clilen = addrlen;
		file_lock_wait();
		connfd = Accept(listenfd, cliaddr, &clilen);
		file_lock_release();
		printf("%d : %ld : Accept one client \n", i, (long)getpid());
		str_echo(connfd);
		Close(connfd);
		printf("%d : %ld : Close one client \n", i, (long)getpid());
	}
}

void child_main_for_pthread_lock(int i, int listenfd, int addrlen)	// 如何共有这一段代码
{
	int connfd;
	socklen_t clilen;
	struct sockaddr* cliaddr;

	cliaddr = (sockaddr*)Malloc(addrlen);
	printf("%d : child %ld starting\n", i, (long)getpid());
	for (;;)
	{
		clilen = addrlen;
		pthread_lock_wait();
		connfd = Accept(listenfd, cliaddr, &clilen);
		pthread_lock_release();
		printf("%d : %ld : Accept one client \n", i, (long)getpid());
		str_echo(connfd);
		Close(connfd);
		printf("%d : %ld : Close one client \n", i, (long)getpid());
	}
}

pid_t child_make(int i, int listenfd, int addrlen)
{
	pid_t pid;
	void child_main(int, int, int);

	if ((pid = Fork()) > 0)
		return (pid);

	child_main(i, listenfd, addrlen);
}

pid_t child_make_for_file_lock(int i, int listenfd, int addrlen)
{
	pid_t pid;
	void child_main_for_file_lock(int, int, int);

	if((pid = Fork()) > 0)
		return (pid);

	child_main_for_file_lock(i, listenfd, addrlen);
}

pid_t child_make_for_pthread_lock(int i, int listenfd, int addrlen)
{
	pid_t pid;
	void child_main_for_pthread_lock(int, int, int);

	if((pid = Fork()) > 0)
		return (pid);

	child_main_for_pthread_lock(i, listenfd, addrlen);
}

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];
	memset(buf, 0, MAXLINE);

again:
	printf("wait client send data...\n");
	/* if ((n = Readn(sockfd, buf, MAXLINE)) > 0) */
	if ((n = Readline(sockfd, buf, MAXLINE)) > 0)
	{
		printf("send client data back ...\n");
		printf("client date num is %ld and data is %s\n", n, buf);
		Write(sockfd, buf, n);
	}

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo:read error");
}

void sig_int_killchildren(int signo)
{
	int i;
	
	//kill all children
	for (i = 0; i < nchildren; i++)
		kill(pids[i], SIGTERM);		// 发送关闭信号

	while (wait(NULL) > 0)	// wait all children close
		;

	if (errno != ECHILD)
		err_sys("wait error");

	exit(0);
} 

void sig_int_for_pipechildren(int signo)
{
	int		i;
	/* void	pr_cpu_time(void); */

	/* 4terminate all children */
	for (i = 0; i < nchildren; i++)
		kill(cptr[i].child_pid, SIGTERM);
	while (wait(NULL) > 0)		/* wait for all children */
		;
	if (errno != ECHILD)
		err_sys("wait error");

	/* pr_cpu_time(); */

	for (i = 0; i < nchildren; i++)
		printf("child %d, %ld connections\n", i, cptr[i].child_count);

	exit(0);
}

void file_lock_init(char* pathname)
{
	char lockfile[1024];

	strncpy(lockfile, pathname, sizeof(lockfile));
	lock_fd = Mkstemp(lockfile);

	unlink(lockfile);

	lock_it.l_type = F_WRLCK;
	lock_it.l_whence = SEEK_SET;
	lock_it.l_start = 0;
	lock_it.l_len = 0;

	unlock_it.l_type = F_WRLCK;
	unlock_it.l_whence = SEEK_SET;
	unlock_it.l_start = 0;
	unlock_it.l_len = 0;
}

void file_lock_wait()
{
	int rc;
	while ((rc = fcntl(lock_fd, F_SETLKW, &lock_it)) < 0) {
		if (errno == EINTR)
			continue;
		else
			err_sys("fcntl error for my_lock_wait");
	}
}

void file_lock_release()
{
	if (fcntl(lock_fd, F_SETLKW, &unlock_it) < 0)
		err_sys("fcntl error for my_lock_release");
}

void pthread_lock_init(char* szFileName)
{
	int fd;
	pthread_mutexattr_t mattr;

	fd = Open(szFileName, O_RDWR | O_CREAT, 0);
	mptr = (pthread_mutex_t *)Mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	/* memset(mptr, 0, sizeof(pthread_mutex_t)); */
	Close(fd);

	Pthread_mutexattr_init(&mattr);
	Pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
	Pthread_mutex_init(mptr, &mattr);
}

void pthread_lock_wait()
{
	Pthread_mutex_lock(mptr);
}

void pthread_lock_release()
{
	Pthread_mutex_unlock(mptr);
}

pid_t child_make_for_pipe(int i, int listenfd, int addrlen)
{
	int sockfd[2];
	pid_t pid;
	void child_main_for_pipe(int, int, int);

	Socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);

	if((pid = Fork()) > 0)
	{
		// parent
		Close(sockfd[1]);	// 父进程使用 sockfd[0]
		cptr[i].child_pid = pid;
		cptr[i].child_pipefd = sockfd[0];
		cptr[i].child_status = 0;
		return (pid);
	}

	Dup2(sockfd[1], STDERR_FILENO);	// 子进程使用 socket[1]，并且将其绑定在标准错误输出上（为什么？）
	Close(sockfd[0]);
	Close(sockfd[1]);
	Close(listenfd);
	child_main_for_pipe(i, listenfd, addrlen);
}

void child_main_for_pipe(int i, int listenfd, int addrlen)
{
	char c;
	int connfd;
	ssize_t n;
	void str_echo(int);

	printf("child %ld starting\n", (long) getpid());
	for(; ;)
	{
		if ((n == Read_fd(STDERR_FILENO, &c, 1, &connfd)) < 0)
			err_quit("read_fd returned 0");

		if (connfd < 0)
			err_quit("no descriptor from read_fd");

		str_echo(connfd);
		Close(connfd);
		Write(STDERR_FILENO, "", 1);	// tell father , child is finish the net request
	}
}
