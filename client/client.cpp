#include "../base/unp.h"

using namespace std;

#define MAXN 16384

// 此处的 host name 也可以是 ipaddress，szServName 也可以是 port
int Tcp_connect(const char* szHostName, const char* szServName)
{
	int		sockfd, n;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	// 此处 getaddrinfo 将会依据 host name, server name 以及 hints 的信息，返回一组 addrinfo 的值——都以 hints 为模板
	// struct addrinfo {
	//	int	ai_flags; 常用的两个值 AI_PASSIVE 和 AI_CANONNAME，前者告知函数被动打开套接口，后者则是告知函数返回主机的规范名
	//	int ai_family;	后面三个参数都在 socket 中介绍过(参见 svr1.cpp )
	//	int ai_socktype;
	//	int ai_protocol;
	//	socklen_t ai_addrlen; 后面 ai_addr 指针指向的 socketaddr 对象的大小，为 16
	//	char * ai_canonname; host 的 canonical name，通常写在 DNS 的记录中
	//	struct sockaddr * ai_addr; 指向 sockaddr 的指针，用在 connect 等地方
	//	struct addrinfo * ai_next; 指向下一个 addrinfo 指针 
	// }
	// 对于客户端程序来说，常见的用法是指定 host name 和 service，用来获得指定服务的地址信息（然后就可以调用 connect )
	// 对于服务端程序来说，常见用法则是指定 service，同时指定 AI_PASSIVE 标志，那么将会从返回值获得 INADDR_ANY 或 IN6ANNR_ANY_INT 标注的 IP 地址(然后就可以调用 socket bind 等)
	if ( (n = getaddrinfo(szHostName, szServName, &hints, &res)) != 0)
		err_quit("tcp_connect error for %s, %s : %s", szHostName, szServName, gai_strerror(n));

	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		
		// sockfd ：socket() 成功时返回一个非负整数
		if (sockfd < 0)
			continue;

		// connect() 成功时返回零
		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;

		// connect() 失败的话，则关闭这个套接口
		Close(sockfd);
	} while ((res = res -> ai_next) != NULL);	// ai_next 即是指向下一个 addrinfo 对象，getaddrinfo 函数可能会返回多个 addrinfo 对象，所以做出链表

	// res 为空，说明遍历到了链表最后，都没有成功完成 connect
	if (res == NULL)
		err_sys("tcp_connect err for %s, %s", szHostName, szServName);

	// 到这里，说明connect 已经链接成功
	// 但 getaddrinfo 返回的结构中的指针（即ai_canonname, ai_addr )指向的内容都是动态分配的，使用 freeaddrinfo 则是将它们释放的接口
	freeaddrinfo(ressave);

	return sockfd;
}

int main(int argc, char **argv)
{
	printf("hello world");
	int i, j, fd, nchildren, nloops, nbytes;
	pid_t pid;
	ssize_t n;
	char request[MAXLINE], reply[MAXN];

	if (argc != 6)
		// children 是指发起几个子进程
		// loop 是指每个子进程发起多少次连接
		// bytes 则是指连接将从服务端接受到的字符大小
		err_quit("usage: client <hostname or IPaddr><port><#children><#loop/child><#bytes/request>");

	nchildren = atoi(argv[3]);
	nloops = atoi(argv[4]);
	nbytes = atoi(argv[5]);
	memset(request, 0, MAXLINE);
	snprintf(request, sizeof(request), "%d\n", nbytes);

	Signal(SIGCHLD, sig_chld);
	Signal(SIGINT, sig_int);
	for (i = 0; i < nchildren; i++)
	{
		printf("fork one\n");
		// 发起子进程
		if ((pid = Fork()) == 0) {
			for (j = 0; j < nloops; j++) {
				// 发起链接，当有套接字链接成功，则返回成功的套接字
				printf("-------------> One TCP Connect\n");
				fd = Tcp_connect(argv[1], argv[2]);
				int nwrite = Write(fd, request, strlen(request));
				printf("-------------> Send TCP Data %d : %s \n", nwrite, request);
				if((n = Readn(fd, reply, nbytes)) != nwrite)	// 假设服务器是个回射服务器
					err_quit("err_quit : server returned %d bytes, nedd %d nbytes", n, nwrite);

				Close(fd);
			}
			printf("child %d done\n", i);
			exit(0);	// 子进程僵死
		}
	}

	while(wait(NULL) > 0)
		;

	if (errno != ECHILD)
		err_sys("wait error");

	exit(1);
}

