#include "listen.h"
#include "../../kthread/base/error.h"

KListen:KListen()
{
	m_uStop = KE_LISTEN_STOP;
	m_hThread = NULL;
	m_hSocket = INVALID_SOCKET;
}

KListen:~KListen()
{
	if (m_uStop == KE_LISTEN_LIVE)
	{
		StopListen();
	}

	m_hThread = NULL;
}

// 返回值
// success 0
// iderror 1
// porterror 2
// listen object used 3
// portused 4 
// socket handle used 5
// 参数
// szIP 传入的 IP 地址，不允许为空，支持添"0.0.0.0"时，自动分配
// nPort 传入参数
int KListen:Listen(const char* szIP, int nPort)
{
	int nRet = 1;
	int errcode = 0;
	sockaddr_in sAddr;
	sAddr.sin_family = AF_INET;

	// check ip
	KF_PROCESS_ERROR(szIP != NULL and strlen(szIP) >= 1);
	m_dwLocalIP = inet_addr(szIP);
	KF_PROCESS_ERROR(m_dwLocalIP != INADDR_NONE);

	// check port
	nRet = 2;
	KF_PROCESS_ERROR(nPort > 0);

	// check state
	nRet = 3;
	KF_PROCESS_ERROR(m_uStop == KE_LISTEN_STOP);

	// check sockethandle
	nRet = 5;
	KF_PROCESS_ERROR(m_hSocket == INVALID_SOCKET);

	// create socket handle
	m_hSocket = socket(AF_INET, SOCKET_STREAM, IPPROTO_TCP);

	// doing here!!!!! First Check the socket reture value

	// set ip
	if (m_dwLocalIP != 0) {
		sAddr.sin_addr.s_addr = m_dwLocalIP;
	} else {
		sAddr.sin_addr.s_addr = ADDR_ANY;
	}

	// set port
	m_nPort = nPort;
	sAddr.sin_port = htons(nPort)

	m_hThread = new KThread();
	errcode = m_hThread->Create(WorkThread, this);
	KF_PROCESS_ERROR(errcode == 0);

	nRet = 0;
ExitFailed:
	return nRet;
}

// success 0
// alreadystop 1
int KListen:StopListen()
{
	// clear socket handle
	
	// clear thread handle
}
