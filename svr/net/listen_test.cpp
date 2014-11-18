#include "listen.h"
#include "../../thread/base/error.h"

void TestListen();
void TestStopListen();

int main()
{
	TestListen();

	TestStopListen();
}

void TestListen()
{
	KListen* pl = new KListen();
	int errorcode;
	errorcode = pl->Listen("127.0.0.0", 8080);	//  IP Error
	if(errorcode != 1)
	{
		K_ERROR_SYS("listen error : %d", errorcode);
	}

	errorcode = pl->Listen("127.0.0.1", 0);	// Part Error
	if (errorcode != 2)
	{
		K_ERROR_SYS("Listen error: %d", errorcode);
	}

	errorcode = pl->Listen("127.0.0.1", 8080);	// success

	errorcode = pl->Listen("127.0.0.1", 8080);	// 已经起动

	KListen* pl2 = new KListen();
	errorcode = pl->Listen("127.0.0.1", 8080);	// Part already use

	delete pl;
	delete pl2;

	pl = new KListen();
	errorcode = pl->Listen("127.0.0.1", 8080);	// 应该也是 success，但注意这里面有可能会遇上 WAIT 这个状态
}

void TestStopListen()
{
	KListen* pl = new KListen();
	pl->StopListen();	// 未起动

	pl->Listen("127.0.0.1", 8080);
	pl->StopListen();	// success

	pl->StopListen();	// 未
}
