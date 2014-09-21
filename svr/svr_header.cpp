#include "svr_header.h"

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

pid_t child_make(int i, int listenfd, int addrlen)
{
	pid_t pid;
	void child_main(int, int, int);

	if ((pid = Fork()) > 0)
		return (pid);

	child_main(i, listenfd, addrlen);
}

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];
	memset(buf, 0, MAXLINE);

again:
	printf("wait client send data...\n");
	/* while((n = Readn(sockfd, buf, MAXLINE)) > 0) */
	if ((n = Readn(sockfd, buf, MAXLINE)) > 0)
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
