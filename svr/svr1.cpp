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

int main()
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	// 1 socket
	// 参数一：socket family:
	// 1 IPv4 -> AF_INET
	// 2 IPv6 -> AF_INET6
	// 3 路由 -> AF_ROUTE，看上去似乎是路由器之类的基础接口? 必须了解清楚，todo
	// ...还有 Unix 域、密钥 等协议——为什么它们会是一个层级的，后文称它们为地址族
	// 参数二：socket type:
	// 1 SOCK_STREAM TCP 字节流套接口
	// 2 SOCK_DGRAM UDP 套接口
	// 3 SOCK_SEQPACKET SCTP
	// 4 SOCK_RAW 原始套接口
	// 通常来说，参数一和参数二的组合能够得到一个缺省值（也许就是参数二），在参数二中已写好
	// 但还是可以在在参数三中指定是那种协议；但为什么要这么做，指定字节流套接口，能服进行 UDP 协议？
	// 参数三：protocol:
	// 1 IPPROTO_TCP
	// 2 IPPROTO_UDP
	// 3 IPPROTO_SCTP
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
		printf("-------> accept one client \n");

		// fork: 线程
		if ((childpid = fork()) == 0) {
			printf("-------> echo one client\n");

			close(listenfd);
			str_echo(connfd);
			/* close(connfd); */
			exit(0);
		}

		// 5 close
		close(connfd);
	}
}
