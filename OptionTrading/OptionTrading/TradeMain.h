#pragma once
#include "HSTradeRequest.h"

class TradeMain
{
public:
	static TradeMain& getInstance() {
		static TradeMain instance;
		return instance;
	}
	TradeMain(const TradeMain&) = delete;
	void operator=(const TradeMain&) = delete;
	~TradeMain();

private:
	TradeMain();

public:
	
private:

};

