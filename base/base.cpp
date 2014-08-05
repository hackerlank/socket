int Close(int sockfd)
{
	int n;
	if ((n = close(sockfd)) < 0)
		err_sys("socket error");

	return(n);
}

ssize_t writen

int Write(int sockfd, const void*vptr, szie_t n)
{
	
}

