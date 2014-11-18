#include "listen.h"
#include "../../kthread/base/error.h"

KListen:KListen()
{
	m_uStop = KE_LISTEN_STOP;
	m_hThread = NULL;
	m_hSocket = INVALID_SOCKET;
	m_nLastOne = 0;
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
// set socket handle opt fail 7
// bind socket fail 8
// 参数
// szIP 传入的 IP 地址，不允许为空，支持添"0.0.0.0"时，自动分配
// nPort 传入参数
int KListen:Listen(const char* szIP, int nPort)
{
	int nRet = 1;
	int errcode = 0;
	int nOptRet = 0;
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

	// set socket handle opt
	nRet = 7;
	nOptRet = _SetSocketOpt();
	KF_PROCESS_ERROR(nOptRet == 0);

	// set ip
	if (m_dwLocalIP != 0) {
		sAddr.sin_addr.s_addr = m_dwLocalIP;
	} else {
		sAddr.sin_addr.s_addr = ADDR_ANY;
	}

	// set port
	m_nPort = nPort;
	sAddr.sin_port = htons(nPort);

	// set bind
	nRet = 8;
	KF_PROCESS_ERROR(bind(m_hSocket, (sockaddr*)sAddr, sizeof(sAddr)) != SOCKET_ERROR);

	// set listen
	nRet = 9;
	KF_PROCESS_ERROR(listen(m_hSocket, MAX_PEND_NUMBER) != SOCKET_ERROR);

	// set accept list
	m_nLastOne = 0;

	m_hThread = new KThread();
	errcode = m_hThread->Create(WorkThread, this);
	KF_PROCESS_ERROR(errcode == 0);

	nRet = 0;
ExitFailed:
	return nRet;
}

// success 0
// socket handle error 1
// set handle nonblock fail 2
// set handle reuse state fail 3
// set tcp nodelay fail 4
int KListen:_SetSocketOpt() {
	int nRet = 1;
	unsigned int uParam = 1;
	int nBlockFlag = 0;
	int nReuse = 1;
	int nDelayFlag = 1;

	// check socket handle
	KF_PROCESS_ERROR(m_hSocket != INVALID_SOCKET);

	// set handle nonblock
	nRet = 2;
#ifdef WIN32
	KF_PROCESS_ERROR(ioctlsocket(m_hSocket, FIONBIO, (u_long *)&uParam) != SOCKET_ERROR);
#endif

#ifdef LINUX
	nBlockFlag = fcntl(m_hSocket, F_GETFL, 0);
	KF_PROCESS_ERROR(nFlag != -1);
	nBlockFlag |= O_NONBLOCK;
	fcntl(m_hSocket, F_SETFL, nBlockFlag);
#endif

	// set socket handle reuse : this is useful to handle TIME_WAIT
	nRet = 3;
	KF_PROCESS_ERROR(setsockopt(m_hSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&nReuse, sizeof(nReuse)) != SOCKET_ERROR);

	// set tcp nodelay
	nRet = 4;
	KF_PROCESS_ERROR(setsockopt(m_hSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&nDelayFlag, sizeof(nDelayFlag)) != SOCKET_ERROR);

	nRet = 0;
ExitFailed:
	return nRet;
}

// success 0
// alreadystop 1
int KListen:StopListen()
{
	int nRet = 1;
	// check stop state
	KF_PROCESS_ERROR(m_uStop == KE_LISTEN_LIVE);

	// clear socket handle
	if (m_hSocket != INVALID_SOCKET)	// 容错，应该必须不是 INVALID_SOCKET
	{
		close(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
	
	// clear thread handle
	if (m_hThread)
	{
		m_hThread->Terminate();
		delete m_hThread;
		m_hThread = NULL;
	}

	// clear accept socket handle
	while (m_nLastOne >= 0)
	{
		close(m_hAcceptSocket[m_nLastOne]);
		m_nLastOne--;
	}
	
	// 设置 m_uStop 为 stop
	m_uStop = KE_LISTEN_STOP;
	
	nRet = 0;
ExitFailed:
	return nRet;
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

	// check for socket
	KF_PROCESS_ERROR(pWork->m_hSocket != INVALID_SOCKET);

	for (;;)
	{
		FD_ZERO(&sProcessSet);
		FD_SET(pWork->m_hSocket, &sProcessSet);

		FD_ZERO(&sFailSet);
		FD_SET(pWork->m_hSocket, &sFailSet);

		nSelectRet = select(pWork->m_hSocket + 1, &sProcessSet, NULL, &sFailSet, &sTimeVal);	// ？是否是立即返回
		if (nSelectRet == SOCKET_ERROR || FD_ISSET(pWork->m_hSocket, &sFailSet))
		{
			pWork->ListenFail();
		}
		else if (FD_ISSET(pWork->m_hSocket, &sProcessSet))
		{
			pWork->ListenSuccess();
		}
	}

ExitFailed:
	return;
}

// accept
// 返回值
// success 0
// socket handle error 1
// max accept socket 2
int KListen:ListenSuccess()
{
	unsigned int hAcceptSocket;
	sockaddr_in sAddr;
	int nRet = 1;

#ifdef WIN32
	int nErrorCode;
	int nAddrLen;
	nAddrLen = sizeof(sAddr);
#endif

#ifdef LINUX
	socklen_t nAddrLen;
	nAddrLen = (socklen_t)sizeof(sAddr);
#endif

	KF_PROCESS_ERROR(m_hSocket != INVALID_SOCKET);

	nRet = 2;
	KF_PROCESS_ERROR(m_nLastOne < 1000);

	hAcceptSocket = accept(m_hSocket, (sockaddr*)&sAddr, &nAddrLen);
	if (hAcceptSocket == INVALID_SOCKET) 
	{
#ifdef WIN32
		nErrorCode = WSAGetLastError();
		if (nErrorCode != WSAEWOULDBLOCK)
		{
			ListenFail();
		}
#endif
#ifdef LINUX
		if (errno != EINPROGRESS && errno != EAGAIN)
		{
			ListenFail();
		}
#endif
	}
	else
	{
		m_hAcceptSocket[m_nLastOne] = hAcceptSocket;
		m_nLastOne ++;
	}

	nRet = 0;
ExitFailed:
	return nRet;
}

int KListen::ListenFail()
{
	StopListen();
	return 0;
}
