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

// Socket 相关 warp 函数
int Setsockopt(int listenfd, int level, int optname, const void* optval, socklen_t optlen);

int Listen(int listenfd, int backlog);

int Accept(int listenfd, struct sockaddr* cliaddr, socklen_t *addrlen);
// Socket 相关 warp 函数
// 
//
//
// Linux 相关的 wrap 函数
int Fork(); // 生成子进程，父进程的返回值为子进程 ID，子进程的返回值为 0

int Open(const char*pathname, int oflag, mode_t mode);
int Close(int sockfd);

// 总的来说， Write 和 Read 是为 Socket 而写的，因为 errno == EINTR 这个错误处理
// 但放在这里应该也是合适的
int Write(int sockfd, const void*vptr, size_t n);
int Readn(int sockfd, void* vptr, size_t n);
int Readline(int sockfd, void* vptr, size_t n);	// 低效率版本

typedef void Sigfunc(int);
void sig_chld(int signo);
void sig_int(int signo);
/* void sig_int_killchildren(int signo) */

Sigfunc* signal(int signo, Sigfunc *func);

int Mkstemp(char* filename);
void* Mmap(void* addr, size_t len, int prot, int flags, int fd, off_t offset);
void Pthread_mutexattr_init(pthread_mutexattr_t *attr);
void Pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int flag);
void Pthread_mutex_init(pthread_mutex_t *mptr, pthread_mutexattr_t *attr);
void Pthread_mutex_lock(pthread_mutex_t *mptr)
void Pthread_mutex_unlock(pthread_mutex_t *mptr)
// Linux 相关的 wrap 函数
// 
//
//
// C++ 内置的 wrap 函数
void* Malloc(size_t nsize);
void* Calloc(size_t num, size_t nsize);
