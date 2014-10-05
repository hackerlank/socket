#include "../base/unp.h"
#include "svr_header.h"

extern Thread	* tptr;
extern pthread_mutex_t m_lock;
extern pthread_cond_t clifd_cond;
extern int clifd[MAXCLI], iget, iput;

int main(int argc, char** argv)
{
	int i;
	socklen_t addrlen, clilen;
	struct sockaddr * cliaddr;
	int listenfd;
	
	if (argc == 3)
	{
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	} 
	else if (argc == 4)
	{
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	}
	else
	{
		err_quit("usage: svr8.app[<host>] <post#> <#thread>");
	}

	cliaddr = (sockaddr*)Malloc(addrlen);

	int nthreads = atoi(argv[argc - 1]);
	tptr = (Thread*)Calloc(nthreads, sizeof(Thread));
	iget = iput = 0;

	for (i = 0; i < nthreads; i++)
		thread_make_for_cond_wait(i);

	Signal(SIGINT, sig_int);
	for(;;)
	{
		clilen = addrlen;
		int connfd;	
		connfd = Accept(listenfd, cliaddr, &clilen);
		printf("____accept one connet\n");
		Pthread_mutex_lock(&m_lock);
		printf("____lock one connet %d, %d\n", &clifd_cond, &m_lock);
		clifd[iput] = connfd;
		if(++iput == MAXCLI)
			iput = 0;
		if(iput == iget)
			err_quit("iput == iget = %d", iput);

		Pthread_cond_signal(&clifd_cond);
		Pthread_mutex_lock(&m_lock);
	}
}
