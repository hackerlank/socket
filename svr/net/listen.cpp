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
// port used 4 
// socket handle used 5
// create socket handle fail 6
// set socket reuse value fail 7
// bind socket fail 8
// 参数
// szIP 传入的 IP 地址，不允许为空，支持添"0.0.0.0"时，自动分配
// nPort 传入参数
int KListen:Listen(const char* szIP, int nPort)
{
	int nRet = 1;
	int errcode = 0;
	int nReuse = 1;
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
	m_uStop = KE_LISTEN_LIVE;

	// check socket handle
	nRet = 5;
	KF_PROCESS_ERROR(m_hSocket == INVALID_SOCKET);

	// create socket handle
	nRet = 6;
	m_hSocket = socket(AF_INET, SOCKET_STREAM, IPPROTO_TCP);
	KF_PROCESS_ERROR(m_hSocket != INVALID_SOCKET);

	// set socket handle reuse value
	nRet = 7;
	KF_PROCESS_ERROR(setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&nReuse, sizeof(nReuse)) != SOCKET_ERROR);

	// set ip
	if (m_dwLocalIP != 0) {
		sAddr.sin_addr.s_addr = m_dwLocalIP;
	} else {
		sAddr.sin_addr.s_addr = ADDR_ANY;
	}

	// set port
	m_nPort = nPort;
	sAddr.sin_port = htons(nPort);

	// bind
	nRet = 8;
	KF_PROCESS_ERROR(bind(m_hSocket, (sockaddr*)sAddr, sizeof(sAddr)) != SOCKET_ERROR);

	// listen
	nRet = 9;
	KF_PROCESS_ERROR(listen(m_hSocket, MAX_PEND_NUMBER) != SOCKET_ERROR);

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
	
	// 设置 m_uStop 为 stop
}

void KListen:WorkThread(void *pThis)
{
	fd_set sProcessSet, sFailSet;
	timeval sTimeVal = {0};
	int nSelectRet;

	// check for this point
	KF_PROCESS_ERROR(pThis != NULL);
	KListen* pWork = (KListen*)pThis;

	// check for state
	KF_PROCESS_ERROR(pWork->m_uStop != KE_LISTEN_STOP);

	FD_ZERO(&sProcessSet);
	FD_SET(pWork->m_hSocket, &sProcessSet);

	FD_ZERO(&sFailSet);
	FD_SET(pWork->m_hSocket, &sFailSet);
	
	nSelectRet = select(pWork->m_hSocket + 1, &sProcessSet, NULL, &sFailSet, &sTimeVal);	// ？是否是立即返回
	if (nSelectRet == SOCKET_ERROR || FD_ISSET(pWork->m_hSocket, &sFailSet))
	{
		pWork->ListenFail();
	}
	else
	{
		pWork->ListenSuccess();
	}

ExitFailed:
	return;
}
