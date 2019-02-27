#include "../mydemo/mdspi.h"
#include <iostream>
#include <ctime>
#include <time.h>
#include <fstream>
#include <thread>
using namespace std;

extern CThostFtdcMdApi* pUserApi;
extern char FRONT_ADDR[];
extern TThostFtdcBrokerIDType	BROKER_ID;
extern TThostFtdcInvestorIDType INVESTOR_ID;
extern TThostFtdcPasswordType	PASSWORD;
extern char* ppInstrumentID[];
extern int iInstrumentID;
extern int iRequestID;

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,int nRequestID, bool bIsLast)
{
	cerr << "calling OnRspError" << endl;
	cerr << "服务器相应错误" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "客户端与交易后台通信连接断开" << endl;
	cerr << "Reason = " << nReason << endl;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << " OnHeartBeatWarning" << endl;
	cerr << " nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
	cerr << "calling OnFrontConnected()" << endl;
	cerr<< "连接到服务器成功" << endl;
	// 用户登录请求
	ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	cerr << "calling ReqUserLogin()" << endl;
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy_s(req.BrokerID, BROKER_ID);
	strcpy_s(req.UserID, INVESTOR_ID);
	strcpy_s(req.Password, PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);
	//这个ReqUserLogin函数是官方头文件提供的,虽然与自己重写的同名,但是并不冲突
	cerr << "发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
	//三元运算符还是很方便的
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//OnRspUserLogin用于返回登陆成功后的信息
	cerr << "calling OnRspUserLogin()" << endl;
	cerr << "服务器对登陆的请求进行响应" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///获取当前交易日
		cerr << "获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
		// 请求订阅行情
		SubscribeMarketData();
	}
}

void CMdSpi::SubscribeMarketData()
{
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	//SubscribeMarketData()订阅行情。
	//@param ppInstrumentID 合约ID  
	//@param nCount 要订阅/退订行情的合约个数
	//@remark
	//返回
	//0，代表成功。
	//1，表示网络连接失败；
	//2，表示未处理请求超过许可数；
	//3，表示每秒发送请求数超过许可数。
	cerr << "发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "订阅行情为: " << pSpecificInstrument->InstrumentID << endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "取消订阅行情为: " << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	cout 
		<<"TradingTime "<< pDepthMarketData->TradingDay <<" "<< pDepthMarketData->UpdateTime << ","
		<< "InstrumentID " << pDepthMarketData->InstrumentID << ","
		<< "LastPrice " << pDepthMarketData->LastPrice << ","
		<< "Volume " << pDepthMarketData->Volume << ","
		<< "OpenInterest " << pDepthMarketData->OpenInterest << ","
		<< "AskPrice1 " << pDepthMarketData->AskPrice1 << ","
		<< "BidPrice1 " << pDepthMarketData->BidPrice1 << ","
		<< "AskVolume1 " << pDepthMarketData->AskVolume1 << ","
		<< "BidVolume1 " << pDepthMarketData->BidVolume1 << ","
		<<endl;
		//二档及以上的行情要收费
		/*<< "  AP2  " << pDepthMarketData->AskPrice2
		<< "  AP3  " << pDepthMarketData->AskPrice3
		<< "  AP4  " << pDepthMarketData->AskPrice4
		<< "  AP5  " << pDepthMarketData->AskPrice5*/
		
	std::ofstream writer("quote.csv",ios::app);
	writer
		<< pDepthMarketData->TradingDay<<" "<< pDepthMarketData->UpdateTime << ","
		<< pDepthMarketData->InstrumentID<<","
		<< pDepthMarketData->LastPrice << ","
		<< pDepthMarketData->Volume << ","
		<< pDepthMarketData->OpenInterest<<","
		<< pDepthMarketData->AskPrice1<<","
		<< pDepthMarketData->BidPrice1 << ","
		<< pDepthMarketData->AskVolume1 << ","
		<< pDepthMarketData->BidVolume1 << ","
		<< endl;
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}
