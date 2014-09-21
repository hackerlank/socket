#include "../base/unp.h"

int Tcp_listen(const char* host, const char* serv, socklen_t* addrlenp);

pid_t child_make(int i, int listenfd, int addrlen);

void child_main(int i, int listenfd, int addrlen);

void str_echo(int sockfd);
