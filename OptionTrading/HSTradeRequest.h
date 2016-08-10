#pragma once
#include "HSTradeCallBack.h"

#include <string>
#include <mutex>

class HSRequest
{
public:
	HSRequest();
	~HSRequest();

	virtual int ConnectMarket();
	virtual int GetConnState(int type);
	virtual int UserLogin(const std::string &, const std::string &);
	virtual int UserLogout(const std::string &);
	virtual int SyncSend(const IFuMessage* lpReqMsg, IFuMessage* lpAnsMsg, int iTimeout = 3000);
	virtual int AsyncSend(const IFuMessage* lpReqMsg, int iKeyID = -1);
	virtual int SubscribeRequest(REGType rType, REGAction rAction, const char* szParam);
	virtual std::string GetErrorMsg(int);
private:
	IHsFutuComm* m_pHScomm;
	HSCallBack*  m_pHSCallBack;
	std::mutex send_mutex_;
};