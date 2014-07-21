#include "../unp.h"

using namespace std;

int main()
{
	cout << "hello word" << endl;
}

// 此处的 host name 也可以是 ipaddress，szServName 也可以是 port
int CheckDataFromServer(const char* szHostName, const char* szServName)
{
	int		sockfd, n;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	// 此处 getaddrinfo 将会依据 host name, server name 以及 hints 的信息，返回一组 addrinfo 的值——都以 hints 为模板
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
	} while ((res = res -> ai_next) != NULL)
}
