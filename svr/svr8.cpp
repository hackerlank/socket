#include "../base/unp.h"
#include "svr_header.h"

extern Thread	* tptr;
extern int listenfd, nthreads;
extern socklen_t addrlen;
extern pthread_mutex_t m_lock;

int main(int argc, char** argv)
{
	int i;
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

	nthreads = atoi(argv[argc - 1]);
	tptr = (Thread*)Calloc(nthreads, sizeof(Thread));

	for (i = 0; i < nthreads; i++)
		thread_make(i);

	Signal(SIGINT, sig_int);
	for(;;)
		pause();
}
