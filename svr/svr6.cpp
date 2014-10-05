#include "svr_header.h"
#include "../base/unp.h"

extern int		nchildren;
extern Child *cptr;

int main(int argc, char **argv)
{
	int			listenfd, i, navail, maxfd, nsel, connfd, rc;
	ssize_t		n;
	fd_set		rset, masterset;
	socklen_t	addrlen, clilen;
	struct sockaddr	*cliaddr;

	if (argc == 3)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: serv06 [ <host> ] <port#> <#children>");

	FD_ZERO(&masterset);
	FD_SET(listenfd, &masterset);
	maxfd = listenfd;
	cliaddr = (struct sockaddr *)Malloc(addrlen);

	nchildren = atoi(argv[argc-1]);
	navail = nchildren;
	cptr = (struct Child*)Calloc(nchildren, sizeof(Child));
	/* cptr = (struct Child*)Malloc(nchildren * sizeof(Child)); */

	/* 4prefork all the children */
	for (i = 0; i < nchildren; i++) {
		child_make_for_pipe(i, listenfd, addrlen);	/* parent returns */
		FD_SET(cptr[i].child_pipefd, &masterset);
		maxfd = std::max(maxfd, cptr[i].child_pipefd);
	}

	Signal(SIGINT, sig_int_for_pipechildren);

	for ( ; ; ) {
		rset = masterset;
		if (navail <= 0)
			FD_CLR(listenfd, &rset);	/* turn off if no available children */
		nsel = Select(maxfd + 1, &rset, NULL, NULL, NULL);

		/* 4check for new connections */
		if (FD_ISSET(listenfd, &rset)) {
			clilen = addrlen;
			connfd = Accept(listenfd, cliaddr, &clilen);

			for (i = 0; i < nchildren; i++)
				if (cptr[i].child_status == 0)
					break;				/* available */

			if (i == nchildren)
				err_quit("no available children");	// 没有多余的子链接的时候，关闭此次请求

			cptr[i].child_status = 1;	/* mark child as busy */
			cptr[i].child_count++;
			navail--;

			n = Write_fd(cptr[i].child_pipefd, (void *)"", 1, connfd);
			Close(connfd);
			if (--nsel == 0)
				continue;	/* all done with select() results */
		}

		/* 4find any newly-available children */
		for (i = 0; i < nchildren; i++) {
			if (FD_ISSET(cptr[i].child_pipefd, &rset)) {
				if ( (n = Read(cptr[i].child_pipefd, &rc, 1)) == 0)
					err_quit("child %d terminated unexpectedly", i);
				cptr[i].child_status = 0;
				navail++;
				if (--nsel == 0)
					break;	/* all done with select() results */
			}
		}
	}
}
