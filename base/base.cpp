#include "unp.h"

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

void* Malloc(int nsize)
{
	void * ptr;
	if ((ptr = malloc(nsize)) == NULL)
		err_sys("socket err: Malloc");

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
