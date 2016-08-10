#pragma once
#include "futu_sdk_interface.h"
#include <string>


class HSCallBack:public IFuCallBack
{

public:
	HSCallBack();
	~HSCallBack();

	virtual unsigned long  HSAPI QueryInterface( const char * iid, IHSKnown **ppv );
	virtual unsigned long  HSAPI AddRef();
	virtual unsigned long  HSAPI Release();
	virtual void HSAPI OnNotifyConnState(IHsFutuComm* lpComm,int iType,int iStatus, const char* szNotifyTime, const char* szMessage);
	virtual void HSAPI OnRspLogin(IHsFutuComm* lpComm, IFuMessage* lpMessage);
	virtual void HSAPI OnRspLogout(IHsFutuComm* lpComm, IFuMessage* lpMessage);
	virtual void HSAPI OnRspSubResult(IHsFutuComm* lpComm,REGType sType,REGAction aType,int iResult,const char* lpParam,const char* szMessage);
	virtual void HSAPI OnReceivedBiz(IHsFutuComm* lpComm,IFuMessage* lpAnsMsg,int iRet,int iKeyID);
	virtual void HSAPI OnRecvMarketData(IHsFutuComm* lpComm,CMarketInfo* lpInfo,REGAction rAction);
	virtual void HSAPI OnRecvArgMarketData(IHsFutuComm* lpComm,CArgMarketInfo* lpInfo,REGAction rAction);
	virtual void HSAPI OnRecvOrderInfo(IHsFutuComm* lpComm,const COrderRspInfo* lpInfo);
	virtual void HSAPI OnRecvOrderRealInfo(IHsFutuComm* lpComm,const CRealRspInfo* lpInfo);
	virtual void HSAPI OnRspOnlineMsg(IHsFutuComm* lpComm,const char* szUsrID,const char* szMessage);
	virtual void HSAPI OnRecvExchStatsInfo(IHsFutuComm* lpComm,const CExchStatusInfo* lpInfo,REGAction rAction);
	
};

//HSCallBack HS;