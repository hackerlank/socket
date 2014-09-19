#ifndef __unp_h_
#define __unp_h_

#include <iostream>
#include <sys/types.h>
#include <netinet/in.h> // base struct
#include <sys/socket.h> // base function
#include <string.h>
#include <netdb.h>
//#include <sys/ioctl.h>
//#include <sys/filio.h>
#include <unistd.h>
#include "base.h"

#ifndef HAVE_BZERO
#define bzero(ptr, n) memset(ptr, 0, n)
#endif

#define SERV_PORT 7830
#define LISTENQ 5
#define MAXLINE 4096

void err_sys(const char *fmt, ...);
void err_dump(const char* fmt, ...);
void err_quit(const char* fmt, ...);
void err_msg(const char* fmt, ...);
static void err_doit(int errnoflag, int level, const char *fmt, va_list ap);

#endif
