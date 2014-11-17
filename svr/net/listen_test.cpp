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
	pl->Listen("127.0.0.0", 8080);	//  IP Error

	pl->Listen("127.0.0.1", 0);	// Part Error

	pl->Listen("127.0.0.1", 8080);	// success

	pl->Listen("127.0.0.1", 8080);	// 已经起动

	KListen* pl2 = new KListen();
	pl->Listen("127.0.0.1", 8080);	// Part already use

	delete pl;
	delete pl2;

	pl = new KListen();
	pl->Listen("127.0.0.1", 8080);	// 应该也是 success，但注意这里面有可能会遇上 WAIT 这个状态
}

void TestStopListen()
{
	KListen* pl = new KListen();
	pl->StopListen();	// 未起动

	pl->Listen("127.0.0.1", 8080);
	pl->StopListen();	// success

	pl->StopListen();	// 未
}
