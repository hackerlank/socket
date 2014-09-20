#include "../base/unp.h"

using namespace std;

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

		if ((childpid = Fork()) == 0) { /*child process*/
			Close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		Close(connfd);
	}
}
