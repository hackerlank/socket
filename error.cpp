#include "unp.h"

#include <syslog.h>

static void err_doit(int, int, const char*, va_list);

void err_sys(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);

	exit(1);
}

static void err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
	int errno_save, n;
	char buf[MAXLINE+1];

	errno_save = errno;
#ifdef HAVE_VSNPRINTF
	vsnprintf(buf, MAXLINE, fmt, ap);	// save call
#else
	vsprintf(buf, fmt, ap);
#endif
	n = strlen(buf);
	if (errnoflag)
		snprintf(buf+n, MAXLINE - n, ": %s", strerror(errno_save));
	strcat(buf, "\n");

	syslog(level, buf);

	return;
}
