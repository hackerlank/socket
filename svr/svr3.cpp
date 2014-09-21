#include "../base/unp.h"
#include "svr_header.h"

static int nchildren;
static pid_t *pids;

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

int main(int argc, char** argv)
{
	int listenfd, i;
	socklen_t addrlen;
	void sig_int_killchildren(int);
	pid_t child_make(int, int, int);

	if (argc == 3)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if(argc == 2)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: svr03 [<host>] <port#> <#children>");

	nchildren = atoi(argv[argc - 1]);
	pids = (pid_t *)Calloc(nchildren, sizeof(pid_t));

	for (i = 0; i < nchildren; i++)
		pids[i] = child_make(i, listenfd, addrlen);

	Signal(SIGINT, sig_int_killchildren);

	for(;;)
		pause();
}
