/* 
包裹函数：这样写是因为，线程函数遇到错误时并不设置标准 Unix 的 errno 变量，而是把 errno 的值作为函数返回值返回给调用者
那么通常代码就会如下
int n;
if ( (n = pthread_mutex_lock (&ndone_mutex)) != 0)
	errno = n, err_sys("pthread_mutex_lock error");

但，如果我们不需要捕获住特定的某个错误，并且都是以终止进程的方式来处理错误的话，可以将其如下包裹

void Pthread_mutex_lock(pthread_mutex_t *mptr)
{
	int n;
	if( (n = pthread_mutex_lock(mptr)) == 0)
		return;

	errno = n;
	err_sys("pthread_mutex_lock error");
}

这样，调用者只需一句话 Pthread_mutex_lock(&ndone_mutex); 即可处理好错误——对该函数来说，是赋值给 errno，对 socket 系列来说则不用，但 socket 可省略掉终止进程这一步

另外，这种形式也可以用宏来实现，每一个配上一个宏，可以考虑怎么做。同时，我也要记住 coco2d 中，我们常用的两种宏
*/
int Close(int sockfd);

int Write(int sockfd, const void*vptr, size_t n);

int Fork();

int Readn(int sockfd, void* vptr, size_t n);

int Setsockopt(int listenfd, int level, int optname, const void* optval, socklen_t optlen);

int Listen(int listenfd, int backlog);
	
void* Malloc(int nsize);

typedef void Sigfunc(int);
Sigfunc* signal(int signo, Sigfunc *func);
Sigfunc* Signal(int signo, Sigfunc *func);
