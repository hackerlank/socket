#include "head.h"
#include <sys/types.h>
#include <unistd.h>

int x;
int* px;

void printx()
{
	x = 2;
	px = new int(2);
}

void child_make()
{
	x = 2;
	pid_t pid;
	if ((pid = fork()) > 0 )
	{
		printx();	
	}
}
