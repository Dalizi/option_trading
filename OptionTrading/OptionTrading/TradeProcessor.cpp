#include "stdafx.h"
#include "TradeProcessor.h"

#include <sstream>
#include <iostream>
#include <condition_variable>
#include <Windows.h>

using namespace std;


shared_ptr<HSRequest> TradeProcessor::comm_(new HSRequest());
concurrency::concurrent_unordered_map<int, std::shared_ptr<TradeProcessor>> TradeProcessor::processors_;

void TradeProcessor::setupCommunication() {
	comm_->ConnectMarket();
}

shared_ptr<TradeProcessor> TradeProcessor::createProcessor(int account, const std::string &password) {
	shared_ptr<TradeProcessor> pProcessor;
	auto conn_state = comm_->GetConnState(SERVICE_TYPE_TRADE);
	if (conn_state == Uninitialized || conn_state == Disconnected || conn_state == Rejected) {
		cout <<"账户： "<<account <<"启动失败: "<< "连接未初始化，未连接或已被拒绝..." << endl;
		return pProcessor;
	}
	pProcessor.reset(new TradeProcessor(account, password));
	processors_[account] = pProcessor;
	pProcessor->start();
	stringstream ss;
	ss << account;
	comm_->SubscribeRequest(RspReport, Subscription, ss.str().c_str());
	return pProcessor;
}

shared_ptr<TradeProcessor> TradeProcessor::getProcessor(int account) {
	auto pProcessor = processors_[account];
	cout << "账号: " << account << "已断开" << endl;
	return pProcessor;
}

void TradeProcessor::notifyAllProcess(const Quote&quote) {
	for (auto& p : processors_) {
		auto pProcessor = p.second;
		if (pProcessor) {
			pProcessor->pushQuote(quote);
		}
	}
}
void TradeProcessor::notifyAllProcess(const MessageSharedPtr& msg) {
	for (auto& p : processors_) {
		auto pProcessor = p.second;
		if (pProcessor) {
			pProcessor->pushMsg(msg);
		}
	}
}

void TradeProcessor::notifyProcess(int account, const MessageSharedPtr& msg) {
	processors_[account]->pushMsg(msg);
}


void TradeProcessor::notifyProcess(int account, const CRealRspInfo& trans) {
	processors_[account]->pushTransaction(trans);
}
void TradeProcessor::notifyProcess(int account, const COrderRspInfo& order) {
	processors_[account]->pushOrder(order);
}
void TradeProcessor::testAll() {
	for (auto t : processors_) {
		t.second->test();
	}
}
TradeProcessor::TradeProcessor(int account, const string &password):account_(account), password_(password)
{
	stringstream ss;
	ss << account_;
	if (comm_->UserLogin(ss.str(), password_))
		cout << account << "已登录" << endl;
}

void TradeProcessor::start() {
	//quote_thread_ = thread(&TradeProcessor::transportQuote, this);
	msg_thread_ = thread(&TradeProcessor::transportMsg, this);
	order_thread_ = thread(&TradeProcessor::transportOrder, this);
	transaction_thread_ = thread(&TradeProcessor::transportTransaction, this);
}

void TradeProcessor::pushQuote(const Quote& quote) {
	quotes_queue_.Put(quote);
}
void TradeProcessor::pushMsg(const MessageSharedPtr& msg) {
	msg_queue_.Put(msg);
}

void TradeProcessor::pushOrder(const COrderRspInfo& o) {
	order_queue_.Put(o);
}
void TradeProcessor::pushTransaction(const CRealRspInfo& t) {
	transaction_queue_.Put(t);
}
Quote TradeProcessor::popQuote() {
	return quotes_queue_.Take();
}
MessageSharedPtr TradeProcessor::popMsg() {
	return msg_queue_.Take();
}
CRealRspInfo TradeProcessor::popTransaction() {
	return transaction_queue_.Take();
}
COrderRspInfo TradeProcessor::popOrder() {
	return order_queue_.Take();
}

int TradeProcessor::asyncSendRequest(IFuMessage* lpReqMsg, int iKeyID) {
	auto n = lpReqMsg->GetCount();
	for (int i = 0; i < n; ++i) {
		auto rec = lpReqMsg->GetRecord(i);
		rec->SetInt("fund_account", account_);
	}
	return comm_->AsyncSend(lpReqMsg, iKeyID);
}

void TradeProcessor::transportQuote() {
	while (true) {
		auto quote = popQuote();
		processQuote(quote);
	}
}

void TradeProcessor::transportMsg() {
	while (true) {
		auto msg = popMsg();
		processMsg(msg);
	}
}

void TradeProcessor::transportOrder() {
	while (true) {
		auto order = popOrder();
		processOrder(order);
	}
}

void TradeProcessor::transportTransaction() {
	while (true) {
		auto transaction = popTransaction();
		processTransaction(transaction);
	}
}

void TradeProcessor::setQuote(const std::string key, const Quote& quote) {
	quotes_map_.insert(key, quote);
}
Quote TradeProcessor::getQuote(const std::string key) {
	return quotes_map_.at(key);
}


void TradeProcessor::processQuote(const Quote& quote) {
	setQuote(quote.contract_code, quote);
}
void TradeProcessor::processMsg(const MessageSharedPtr& msg) {
	/*auto r_msg = msg.get();
	cout << "Message Type: " << r_msg->GetMsgType() << endl;
	if (r_msg->GetMsgType() == 203){
		for (int i = 0; i < r_msg->GetCount(); ++i) {
			auto rec = r_msg->GetRecord(i);
			printf("[期货账号]:[%d],[委托号]:[%d], [委托状态]:[%c]\n",
				rec->GetInt("fund_account"),
				rec->GetInt("entrust_no"),
				rec->GetChar("entrust_status"));
		}
	}*/
}

void TradeProcessor::processOrder(const COrderRspInfo& lpInfo) {
	printf("委托回报——[期货账号]:[%d],[合约代码]:[%s],[买卖]:[%s],[开平]:[%s],[价格]:[%.3f], [数量]:[%d], [成交数量]:[%d], [撤单数量]:[%d],[委托号]:[%d], [委托批号]:[%d], [主场编号]:[%s], [委托状态]:[%c], [当前时间]:[%d]\n",
		lpInfo.fund_account,
		lpInfo.contract_code,
		lpInfo.entrust_bs,
		lpInfo.entrust_direction,
		lpInfo.entrust_price,
		lpInfo.entrust_amount,
		lpInfo.business_total_amount,
		lpInfo.cacel_amount,
		lpInfo.entrust_no,
		lpInfo.batch_no,
		lpInfo.confirm_no,
		lpInfo.entrust_status,
		lpInfo.curr_time);

	/*comm_->SubscribeRequest(SingleCode, Snapshot, "m1701-C-2900");
	auto quote = popQuote();
	if (quote.contract_code != "m1701-C-2900") pushQuote(quote);
	auto msg = NewFuMessage(MSG_TYPE_NEW_SINGLE_ORDER, 0);
	auto rec = msg->AddRecord();

	rec->SetInt("fund_account", account_);
	rec->SetString("futu_exch_type", "F2");
	rec->SetString("contract_code", quote.contract_code.c_str());
	rec->SetChar("entrust_bs", '1');
	rec->SetDouble("futu_entrust_price", quote.ask_price);
	rec->SetChar("futures_direction", '1');
	rec->SetChar("futu_entrust_prop", '0');
	rec->SetChar("entrust_kind", '0');
	rec->SetChar("hedge_type", '0');
	rec->SetDouble("entrust_amount", 1);
	asyncSendRequest(msg);*/
}

void TradeProcessor::processTransaction(const CRealRspInfo& lpInfo) {
	comm_->SubscribeRequest(SingleCode, Snapshot, "m1701-C-2900");
	auto quote = popQuote();
	if (quote.contract_code != "m1701-C-3000") pushQuote(quote);
	printf("成交回报——[期货账号]:[%d],[合约代码]:[%s],[成交价格]:[%.3f], [成交数量]:[%.1f], [总成交]:[%d], [委托号]:[%d], [委托批号]:[%d], [主场编号]:[%s], [委托状态]:[%c], [成交编号]:[%s], [成交时间]:[%d]\n",
		lpInfo.fund_account,
		lpInfo.contract_code,
		lpInfo.business_price,
		lpInfo.business_amount,
		lpInfo.business_total_amount,
		lpInfo.entrust_no,
		lpInfo.batch_no,
		lpInfo.confirm_no,
		lpInfo.entrust_status,
		lpInfo.business_no,
		lpInfo.business_time);
	auto msg = NewFuMessage(MSG_TYPE_NEW_SINGLE_ORDER, 0);
	auto rec = msg->AddRecord();
	rec->SetInt("fund_account", account_);
	rec->SetString("futu_exch_type", "F2");
	rec->SetString("contract_code", quote.contract_code.c_str());
	rec->SetChar("entrust_bs", string(lpInfo.entrust_bs)=="1"?'2':'1');
	rec->SetDouble("futu_entrust_price", string(lpInfo.entrust_bs) == "2"?(lpInfo.business_price-0.5):(lpInfo.business_price+0.5));
	rec->SetChar("futures_direction", string(lpInfo.entrust_direction)=="2"|| string(lpInfo.entrust_direction) == "4"?'1':'2');
	rec->SetChar("futu_entrust_prop", '0');
	rec->SetChar("entrust_kind", '0');
	rec->SetChar("hedge_type", '0');
	rec->SetDouble("entrust_amount", lpInfo.business_amount);
	cout <<"买卖方向："<< rec->GetChar("entrust_bs") << "开平方向：" << rec->GetChar("futures_direction") <<endl;
	asyncSendRequest(msg);
}

TradeProcessor::~TradeProcessor()
{
	msg_thread_.join();
	quote_thread_.join();
}

void TradeProcessor::test() {
	comm_->SubscribeRequest(SingleCode, Snapshot, "m1701-C-2900");
	auto quote = popQuote();
	if (quote.contract_code != "m1701-C-2900") pushQuote(quote);
	auto msg = NewFuMessage(MSG_TYPE_NEW_SINGLE_ORDER, 0);
	auto rec = msg->AddRecord();

	rec->SetInt("fund_account", account_);
	rec->SetString("futu_exch_type", "F2");
	rec->SetString("contract_code", "m1701-C-2900");
	rec->SetChar("entrust_bs", '2');
	rec->SetDouble("futu_entrust_price", 435);
	rec->SetChar("futures_direction", '1');
	rec->SetChar("futu_entrust_prop", '0');
	rec->SetChar("entrust_kind", '0');
	rec->SetChar("hedge_type", '0');
	rec->SetDouble("entrust_amount", 10);
	asyncSendRequest(msg);
	
	msg = NewFuMessage(MSG_TYPE_GET_ENTRUST_ORDERS, 0);
	rec = msg->AddRecord();
	rec->SetInt("fund_account", account_);
	asyncSendRequest(msg);
}