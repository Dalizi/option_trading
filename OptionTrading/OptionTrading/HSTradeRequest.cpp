#include "stdafx.h"
#include "HSTradeRequest.h"

#include <iostream>

using namespace std;

HSRequest::HSRequest()
{
	m_pHScomm = NewFuCommObj(0);
	m_pHSCallBack = new HSCallBack();
}
HSRequest::~HSRequest()
{
	m_pHSCallBack->Release();
}

int HSRequest::ConnectMarket()
{
	const char * csHSLic= "31332263F984F05D4510538804520AB1";
	const char * m_szMarketIP = "111.205.107.203:2800";
	m_pHScomm->SetConfig("futu", "server",m_szMarketIP);
	m_pHScomm->SetConfig("futu","biz_license_str",(char *)csHSLic);
	m_pHScomm->SetConfig("futu","entrust_type","7");//委托方式

	const char * csFileName;
	csFileName= "license1.dat";
	m_pHScomm->SetConfig("futu","comm_license_file",csFileName);
	int ireturn = 0;
	if(ireturn = m_pHScomm->Init(m_pHSCallBack,10000))
		{
			cout <<"初始化失败:return="<<ireturn<<" ErrorMessage:"<<m_pHScomm->GetErrorMsg(ireturn) <<endl;
			m_pHScomm->Release();
			return -1;
		}

	//启动(行情)
	if(ireturn = m_pHScomm->Start(SERVICE_TYPE_QUOTE))
	{
		cout <<"行情启动失败:return="<<ireturn<<" ErrorMessage:"<<m_pHScomm->GetErrorMsg(ireturn) <<endl;
		m_pHScomm->Release();
		return -1;
	}
	//启动(交易) 
	if(0 != (ireturn = m_pHScomm->Start(SERVICE_TYPE_TRADE)))
	{
		cout <<"交易启动失败:return="<<ireturn<<" ErrorMessage:"<<m_pHScomm->GetErrorMsg(ireturn) <<endl;
		m_pHScomm->Release();
		return -1;
	}
	
	return 0;
}

int HSRequest::GetConnState(int type) {
	return m_pHScomm->GetStatus(type);
}


int HSRequest::UserLogin(const string &login_name, const string &password)
{
	int iret = m_pHScomm->DoLogin(login_name.c_str(), password.c_str(), 1);
	if (iret)
	{
		cout << login_name << "登录失败" <<endl;
	}
	return iret;
}

int HSRequest::UserLogout(const string &login_name) 
{
	return m_pHScomm->DoLogout(login_name.c_str());
	
}

int HSRequest::SyncSend(const IFuMessage* lpReqMsg, IFuMessage* lpAnsMsg, int iTimeout) {
	lock_guard<mutex> lck(send_mutex_);
	return m_pHScomm->SyncSendRecv(lpReqMsg, lpAnsMsg, iTimeout);
}

int HSRequest::AsyncSend(const IFuMessage* lpReqMsg, int iKeyID) {
	lock_guard<mutex> lck(send_mutex_);
	return m_pHScomm->AsyncSend(lpReqMsg, iKeyID);
}

string HSRequest::GetErrorMsg(int err_no) {
	return m_pHScomm->GetErrorMsg(err_no);
}

int HSRequest::SubscribeRequest(REGType rType, REGAction rAction, const char* szParam) {
	return m_pHScomm->SubscribeRequest(rType, rAction, szParam);
}

