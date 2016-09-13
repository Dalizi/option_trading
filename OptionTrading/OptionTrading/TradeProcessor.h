#pragma once

#include "HSTradeRequest.h"
#include "BlockingQueue.h"
#include "BlockingMap.h"

#include <string>
#include <memory>
#include <ppl.h>
#include <concurrent_unordered_map.h>
#include <thread>
#include <iostream>
#include <utility>


struct Quote {
	std::string contract_code;
	double ask_price, bid_price, ask_volume, bid_volume, uplimit, downlimit;
};

class MessageSharedPtr  {
public:
	MessageSharedPtr() = default;
	MessageSharedPtr(IFuMessage* msg):msg_(msg){
		msg->AddRef();
	}
	MessageSharedPtr(const MessageSharedPtr& p):msg_(p.msg_) {
		p.msg_->AddRef();
	}
	MessageSharedPtr & operator=(const MessageSharedPtr & rhs)
	{
		if (this == &rhs)
			return *this;
		if (rhs.msg_) rhs.msg_->AddRef();
		if (msg_) msg_->Release();
		msg_ = rhs.msg_;
		return *this;
	}
	~MessageSharedPtr() {
		msg_->Release();
	}

	IFuMessage* get() const{
		return msg_;
	}
private:
	IFuMessage* msg_=nullptr;
};
class TradeProcessor
{
public:
	virtual ~TradeProcessor();
	static void setupCommunication();
	static std::shared_ptr<TradeProcessor> createProcessor(int account, const std::string &password);
	static std::shared_ptr<TradeProcessor> getProcessor(int account);
	static void notifyAllProcess(const Quote&);
	static void notifyAllProcess(const MessageSharedPtr&);
	static void notifyProcess(int account, const MessageSharedPtr&);
	static void notifyProcess(int account, const CRealRspInfo&);
	static void notifyProcess(int account, const COrderRspInfo&);
	static void testAll();
	void pushQuote(const Quote& quote);
	void pushMsg(const MessageSharedPtr& msg);
	void pushOrder(const COrderRspInfo&);
	void pushTransaction(const CRealRspInfo&);

	void setQuote(const std::string key, const Quote& quote);
	Quote getQuote(const std::string key);

	Quote popQuote();
	MessageSharedPtr popMsg();
	CRealRspInfo popTransaction();
	COrderRspInfo popOrder();
	int asyncSendRequest(IFuMessage* lpReqMsg, int iKeyID = -1);
	
	void start();
	void transportQuote();
	void transportMsg();
	void transportTransaction();
	void transportOrder();

	virtual void processQuote(const Quote&);
	virtual void processMsg(const MessageSharedPtr&);
	virtual void processTransaction(const CRealRspInfo&);
	virtual void processOrder(const COrderRspInfo&);
	void test();
private:
	TradeProcessor();
	TradeProcessor(int account, const std::string &password);
private:
	int account_;
	std::string password_;
	BlockingQueue<Quote> quotes_queue_;
	BlockingQueue<MessageSharedPtr> msg_queue_;
	BlockingQueue<CRealRspInfo> transaction_queue_;
	BlockingQueue<COrderRspInfo> order_queue_;
	static std::shared_ptr<HSRequest> comm_;
	static concurrency::concurrent_unordered_map<int, std::shared_ptr<TradeProcessor>> processors_;
	std::thread msg_thread_, quote_thread_, order_thread_, transaction_thread_;
	BlockingMap<std::string, Quote> quotes_map_;
};

