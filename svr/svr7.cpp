#include "../base/unp.h"
#include "svr_header.h"

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];

again:
	while((n = read(sockfd, buf, MAXLINE)) > 0)
		write(sockfd, buf, n);

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo:read error");
}

int main(int argc, char** argv)
{
	int listenfd, connfd;
	void sig_int(int);
	void *doit(void*);
	pthread_t tid;
	socklen_t clilen, addrlen;
	struct sockaddr * cliaddr;

	if (argc == 2)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if(argc == 3)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: svr6.app [<host>] <port#>");

	cliaddr = Malloc(addrlen);
	Signal(SIGINT, sig_int);

	for(;;){
		clilen = addrlen;
		connfd = Accept(listenfd, cliaddr, &clilen);
		Pthread_create(&tid, NULL, &doit, (void*)connfd);
	}
}

void* doit(void* arg)
{
	void str_echo((int));

	Pthread_detach(pthread_self());
	str_echo((int)arg);
	Close((int)arg);

	return (NULL);
}
