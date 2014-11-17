#ifndef __LISTEN_H__
#define __LISTEN_H__

#include "../../thread/kthread.h"

class KListen
{
	public:
		enum KE_LISTEN_STATE
		{
			KE_LISTEN_STOP = 0,
			KE_LISTEN_LIVE = 1,
		};
		enum MAX_PEND_NUMBER = 128;

		KListen();
		~KListen();

		int Listen(const char* szIP, int nPort);
		int StopListen();

		void WorkThread(void* pThis);

		int ListenFail();
		int ListenSuccess();

	private:
		KThread *m_hThread;
		unsigned int m_uStop;

	private:
		unsigned long m_dwLocalIP;
		unsigned int m_nPort;
		unsigned int m_hSocket;
}

#endif
