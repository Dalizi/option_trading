// OptionTrading.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TradeProcessor.h"
#include <iostream>
#include <Windows.h>
#include <fstream>

using namespace std;
int main()
{
	TradeProcessor::setupCommunication();
	fstream f("Account.txt");
	string line;
	while (getline(f, line)) {
		if(!line.empty())
			TradeProcessor::createProcessor(stoi(line), "999999");
	}
	char c;
	//auto msg = NewFuMessage(MSG_TYPE_NEW_SINGLE_ORDER, 0);
	//auto rec = msg->AddRecord();

	//rec->SetInt("fund_account", 18901685);
	//rec->SetString("futu_exch_type", "F2");
	//rec->SetString("contract_code", "m1701-C-3000");
	//rec->SetChar("entrust_bs", '1');
	//rec->SetDouble("futu_entrust_price", 250);
	//rec->SetChar("futures_direction", '1');
	//rec->SetChar("futu_entrust_prop", '0');
	//rec->SetChar("entrust_kind", '0');
	//rec->SetChar("hedge_type", '0');
	//rec->SetDouble("entrust_amount", 2);
	////TradeProcessor::notifyProcess(18901685, msg);
	//while (cin >> c) {
	//	cout << msg->AddRef() << endl;
	//	msg->Release();
	//}
	while (cin >> c) TradeProcessor::testAll();

	//cout << "Price is: " << Pricing::delta_call(1, 1, 0.02, 0.25, 0.1) << endl;
	//cout << "Root is: " <<Pricing::implied_vol_call(0.5, 0.03, 1,1,0.02, 0.25) << endl;
	//system("Pause");
	return 0;
}

