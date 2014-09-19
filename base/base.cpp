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
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nwritten = write(sockfd, ptr, nleft)) < 0) {
			if (nwritten < 0 && errno == EINTR)	// 慢系统调用，继续 write
				nwritten = 0;
			else
				return -1;
			end	
		}

		nleft -= nwritten;
		ptr += nwritten;
	}

	return n;
}

int Write(int sockfd, const void*vptr, szie_t n)
{
	
}

