#include "unp.h"

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

int main()
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	// 1 socket
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	// 2 bind
	bind(listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

	// 3 listen
	listen(listenfd, LISTENQ); // LISTENQ 是环境变量，这里坐简单处理

	for( ; ; ) {
		clilen = sizeof(cliaddr);
		// 4 accept
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

		// fork: 线程
		if ((childpid = fork()) == 0) {
			close(listenfd);
			str_echo(connfd);
			exit(0);
		}

		// 5 close
		close(connfd);
	}
}
