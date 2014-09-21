#include "../base/unp.h"
#include "header.h"

using namespace std;

int main(int argc, char **argv)
{
	int listenfd, connfd;
	pid_t childpid;
	void sig_chld(int), sig_int(int), web_child(int);
	socklen_t clilen, addrlen;
	struct sockaddr *cliaddr;

	if (argc == 2)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: svr2 [<host>] <port#>");

	cliaddr = (struct sockaddr*)Malloc(addrlen);

	Signal(SIGCHLD, sig_chld);
	Signal(SIGINT, sig_int);
	for(; ; ){
		clilen = addrlen;
		if((connfd = accept(listenfd, cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;
			else
				err_sys("accept err");
		}

		printf("-------> Get Client: Accept\n");
		if ((childpid = Fork()) == 0) { /*child process*/
			Close(listenfd);
			printf("-------> Get Client: echo\n");
			str_echo(connfd);
			exit(0);
		}
		Close(connfd);
	}
}
