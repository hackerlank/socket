#include "unp.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

int Close(int sockfd)
{
	int n;
	if ((n = close(sockfd)) < 0)
		err_sys("socket error: close");

	return(n);
}

int Fork()
{
	int n;
	if ((n = fork()) < 0)
		err_sys("socket error: fork");

	return (n);
}

ssize_t writen(int sockfd, const void* vptr, size_t n)
{
	size_t	nleft;
	ssize_t nwritten;
	const char* ptr;

	ptr = (const char*)vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nwritten = write(sockfd, ptr, nleft)) < 0) {
			if (nwritten < 0 && errno == EINTR)	// 慢系统调用，继续 write
				nwritten = 0;
			else
				return -1;
		}

		nleft -= nwritten;
		ptr += nwritten;
	}

	return n;
}

int Write(int sockfd, const void*vptr, size_t n)
{
	int nWriten;
	if ((nWriten = writen(sockfd, vptr, n)) < 0)
		err_sys("socket error: write");

	return (nWriten);
}

ssize_t readn(int sockfd, void* vptr, size_t n)
{
	size_t nleft;
	ssize_t nreadn;
	char* ptr;

	nleft = n;
	ptr = (char*)vptr;
	while (nleft > 0)
	{
		if ((nreadn = read(sockfd, ptr, nleft)) < 0) {
			if (errno == EINTR) 
				nreadn = 0;
			else
				return -1;
		} else if (nreadn == 0)
			break;

		nleft -= nreadn;
		ptr += nreadn;
	}

	return n-nleft;		// >= 0
}

int Readn(int sockfd, void* vptr, size_t n)
{
	int nReadn;
	if ((nReadn = readn(sockfd, vptr, n)) < 0)
		err_sys("socket err: readn");

	return (nReadn);
}

ssize_t readline(int sockfd, void * vptr, size_t maxlen)
{
	printf("---> %d readline: %d\n", maxlen, errno);
	ssize_t n, rc;
	char c, *ptr;
	ptr = (char*)vptr;

	for (n = 1; n < maxlen; n++)
	{
		again:
		rc = read(sockfd, &c, 1);
		printf("%d read : %c\n", rc, c);
		if (rc == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		} else if(rc == 0) {
			*ptr = 0;
			return (n - 1);
		} else {
			if (errno == EINTR)
				goto again;

			printf("---> errno : %d", errno);
			return (-1);
		}
	}

	printf("end of readline %d \n", int(n));
	*ptr = 0;
	return (n);
}

int Readline(int sockfd, void* vptr, size_t n)
{
	ssize_t nReadn;
	if ((nReadn = readline(sockfd, vptr, n)) < 0)
		err_sys("socket err: readline");

	return (nReadn);
}

int Setsockopt(int listenfd, int level, int optname, const void* optval, socklen_t optlen)
{
	int n;
	if ((n = setsockopt(listenfd, level, optname, optval, optlen)) < 0)
		err_sys("socket err: setsockopt");

	return n;
}

int Listen(int listenfd, int backlog)
{
	int n;
	if ((n = listen(listenfd, backlog)) < 0)
		err_sys("socket err: listen");

	return n;
}

int Accept(int listenfd, struct sockaddr* cliaddr, socklen_t *addrlen)
{
	int n;
	if ((n = accept(listenfd, cliaddr, addrlen)) < 0)
		err_sys("socket err: accept");

	return n;
}

void* Malloc(size_t nsize)
{
	void * ptr;
	if ((ptr = malloc(nsize)) == NULL)
		err_sys("socket err: Malloc");

	return ptr;
}

void* Calloc(size_t num, size_t nsize)
{
	void *ptr;
	if((ptr = calloc(num, nsize)) == NULL)
		err_sys("socket err: Calloc");

	return ptr;
}

Sigfunc* Signal(int signo, Sigfunc *func)
{
	Sigfunc* pfunc;
	if((pfunc = signal(signo, func)) == SIG_ERR)
		err_sys("socket err: Signal");

	return pfunc;
}

Sigfunc* signal(int signo, Sigfunc *func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef	SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	} else {
#ifdef	SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}

	if (sigaction(signo, &act, &oact) < 0)
		return(SIG_ERR);

	return (oact.sa_handler);
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);

	return;
}

void sig_int(int signo)
{
	exit(0);
}

int Mkstemp(char* filename)
{
	int fd = -1;
	if ((fd = mkstemp(filename)) == -1)
		err_sys("socket error: mkstemp");

	return fd;
}

int Open(const char*pathname, int oflag, mode_t mode)
{
	int fd;

	if((fd = open(pathname, oflag, mode)) == -1)
		err_sys("open error for %s", pathname);

	return (fd);
}

void* Mmap(void* addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	void *ptr;
	if ((ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void*)-1))
		err_sys("mmap error");

	return ptr;
}

void Pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
	int n;
	if ((n = pthread_mutexattr_init(attr)) != 0)
		err_sys("pthread_mutexattr_inti error");

	return;
}

void Pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int flag)
{
	int n;
	if ((n = pthread_mutexattr_setpshared(attr, flag)) != 0)
		err_sys("pthread_metxattr_setpshared error");

	return;
}

void Pthread_mutex_init(pthread_mutex_t *mptr, pthread_mutexattr_t *attr)
{
	int n;
	if ((n = pthread_mutex_init(mptr, attr)) != 0)
		err_sys("pthread_metxattr_init error");

	return;
}

void Pthread_mutex_lock(pthread_mutex_t *mptr)
{
	int n;
	if ((n = pthread_mutex_lock(mptr)) != 0)
		err_sys("pthread_mutex_lock error");

	return;
}

void Pthread_mutex_unlock(pthread_mutex_t *mptr)
{
	int n;
	if ((n = pthread_mutex_unlock(mptr)) != 0)
		err_sys("pthread_mutex_unlock error");

	return;
}

void Socketpair(int family, int type, int protocal, int *fd)
{
	int n;
	if ((n = socketpair(family, type, protocal, fd)) < 0)
		err_sys("socketpair err");
}

void Dup2(int fd1, int fd2)
{
	if (dup2(fd1, fd2) == -1)
		err_sys("Dup2 Oerror");
}
