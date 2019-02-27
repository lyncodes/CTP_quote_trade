// mydemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "../api/ThostFtdcMdApi.h"
#include "../mydemo/mdspi.h"
#include <fstream>
#pragma comment(lib,"../api/thostmduserapi.lib")


CThostFtdcMdApi* pUserApi; // UserApi对象
//180.168.146.187:10010
//char FRONT_ADDR[] = "tcp://180.168.146.187:10031";		// 测试前置地址
char FRONT_ADDR[] = "tcp://180.168.146.187:10010";		// 实盘前置地址,非交易时间无数据
TThostFtdcBrokerIDType	BROKER_ID = "9999";			// 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "134183";		// 投资者代码
TThostFtdcPasswordType  PASSWORD = "lynsimnow123";			// 用户密码

char a1[] = "cu1903";
char a2[] = "al1903";
char a3[] = "zn1903";
char a4[] = "pb1903";
char a5[] = "rb1903";
char a6[] = "cu1903";
char a7[] = "au1903";
char *ppInstrumentID[] = { a1,a2,a3,a4,a5,a6,a7 };			// 行情订阅列表
int iInstrumentID = 7;						// 行情订阅数量
int iRequestID = 0;                                     // 请求编号

void write_header(){
	std::ofstream writer("quote.csv", std::ios::app);
	writer
		<< "TradingTime," 
		<< "InstrumentID," 
		<< "LastPrice," 
		<< "Volume," 
		<< "OpenInterest," 
		<< "AskPrice1," 
		<< "BidPrice1," 
		<< "AskVolume1," 
		<< "BidVolume1" 
		<< std::endl;
}

int main()
{
    std::cout << "Hello World!\n"; 
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// 创建UserApi
	write_header();
	CThostFtdcMdSpi* pUserSpi = new CMdSpi;
	pUserApi->RegisterSpi(pUserSpi);						// 注册事件类
	pUserApi->RegisterFront(FRONT_ADDR);					// connect
	pUserApi->Init();
	pUserApi->Join();
	pUserApi->Release();
}
