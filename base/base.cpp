int Close(int sockfd)
{
	int n;
	if ((n = close(sockfd)) < 0)
		err_sys("socket error");

	return(n);
}

ssize_t writen(int sockfd, const void* vptr, size_t n)
{
	size_t	nleft;
	ssize_t nwritten;
	const char
}

int Write(int sockfd, const void*vptr, szie_t n)
{
	
}

