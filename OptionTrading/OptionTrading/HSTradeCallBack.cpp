#include "stdafx.h"
#include "HSTradeCallBack.h"
#include "TradeProcessor.h"

#include <string>
#include <iostream>

using namespace std;

	HSCallBack::HSCallBack()
	{
	}
	
	HSCallBack::~HSCallBack()
	{
	}

	void HSAPI HSCallBack::OnNotifyConnState(IHsFutuComm* lpComm,int iType,int iStatus, const char* szNotifyTime, const char* szMessage)
	{
		cout <<szMessage << endl;

	}

	void HSAPI HSCallBack::OnRspLogin(IHsFutuComm* lpComm, IFuMessage* lpMessage)
	{
		int num_message=lpMessage->GetCount();
		for (int i=0;i<num_message;i++)
		{
			IFuRecord* lpTmpRecord = lpMessage->GetRecord(i);
			int account = lpTmpRecord->GetInt("fund_account");
			if (account != 0) 
			{
				cout << account << "已登录" << endl;
			}

		}
	}

	void HSAPI HSCallBack::OnRspLogout(IHsFutuComm* lpComm, IFuMessage* lpMessage)
	{
		int num_message = lpMessage->GetCount();
		for (int i = 0; i<num_message; i++)
		{
			IFuRecord* lpTmpRecord = lpMessage->GetRecord(i);
			int account = lpTmpRecord->GetInt("fund_account");
			string result = lpTmpRecord->GetString("result");
			if (account != 0)
			{
				cout << account << "登出结果: " << result << endl;
			}

		}
	}

	void HSAPI HSCallBack::OnRspSubResult(IHsFutuComm* lpComm,REGType sType,REGAction aType,int iResult,const char* lpParam,const char* szMessage)
	{
			}

	void HSAPI HSCallBack::OnReceivedBiz(IHsFutuComm* lpComm,IFuMessage* lpAnsMsg,int iRet,int iKeyID)
	{
		//cout << "收到业务应答数据： " << iRet << endl;
		TradeProcessor::notifyProcess(iKeyID, MessageSharedPtr(lpAnsMsg));
	}

	void HSAPI HSCallBack::OnRecvMarketData(IHsFutuComm* lpComm,CMarketInfo* lpInfo,REGAction rAction)
	{
		Quote quote;
		quote.contract_code = lpInfo->contract_code;
		quote.ask_price = lpInfo->sale_low_price;
		quote.ask_volume = lpInfo->sale_low_amount;
		quote.bid_price = lpInfo->buy_high_price;
		quote.bid_volume = lpInfo->buy_high_amount;
		quote.uplimit = lpInfo->uplimited_price;
		quote.downlimit = lpInfo->downlimited_price;
		TradeProcessor::notifyAllProcess(quote);

	}

	void HSAPI HSCallBack::OnRecvArgMarketData(IHsFutuComm* lpComm,CArgMarketInfo* lpInfo,REGAction rAction)
	{
		
	}

	void HSAPI HSCallBack::OnRecvOrderInfo(IHsFutuComm* lpComm,const COrderRspInfo* lpInfo)
	{
		TradeProcessor::notifyProcess(lpInfo->fund_account, *lpInfo);
	}

	void HSAPI HSCallBack::OnRecvOrderRealInfo(IHsFutuComm* lpComm,const CRealRspInfo* lpInfo)
	{
		TradeProcessor::notifyProcess(lpInfo->fund_account, *lpInfo);
	}

	void HSAPI HSCallBack::OnRspOnlineMsg(IHsFutuComm* lpComm,const char* szUsrID,const char* szMessage)
	{
	}

	unsigned long  HSAPI HSCallBack::QueryInterface( const char * iid, IHSKnown **ppv )
	{
		return 0;
	}
	unsigned long  HSAPI HSCallBack::AddRef()
	{
		return 0;
	}
	unsigned long  HSAPI HSCallBack::Release()
	{
		return 0;
	}


	void HSAPI HSCallBack::OnRecvExchStatsInfo(IHsFutuComm* lpComm,const CExchStatusInfo* lpInfo,REGAction rAction)
	{
		
	}