#include "../base/unp.h"
#include "svr_header.h"

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
		err_quit("usage: svr7.app [<host>] <port#>");

	cliaddr = (sockaddr *)Malloc(addrlen);
	Signal(SIGINT, sig_int);

	for(;;){
		clilen = addrlen;
		connfd = Accept(listenfd, cliaddr, &clilen);
		Pthread_create(&tid, NULL, &doit, (void*)&connfd);
	}
}

void* doit(void* arg)
{
	Pthread_detach(pthread_self());
	int sockfd = *((int *)arg);
	str_echo(sockfd);
	Close(sockfd);

	return (NULL);
}
