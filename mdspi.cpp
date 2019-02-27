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
	cerr << "��������Ӧ����" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "�ͻ����뽻�׺�̨ͨ�����ӶϿ�" << endl;
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
	cerr<< "���ӵ��������ɹ�" << endl;
	// �û���¼����
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
	//���ReqUserLogin�����ǹٷ�ͷ�ļ��ṩ��,��Ȼ���Լ���д��ͬ��,���ǲ�����ͻ
	cerr << "�����û���¼����: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
	//��Ԫ��������Ǻܷ����
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//OnRspUserLogin���ڷ��ص�½�ɹ������Ϣ
	cerr << "calling OnRspUserLogin()" << endl;
	cerr << "�������Ե�½�����������Ӧ" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///��ȡ��ǰ������
		cerr << "��ȡ��ǰ������ = " << pUserApi->GetTradingDay() << endl;
		// ����������
		SubscribeMarketData();
	}
}

void CMdSpi::SubscribeMarketData()
{
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	//SubscribeMarketData()�������顣
	//@param ppInstrumentID ��ԼID  
	//@param nCount Ҫ����/�˶�����ĺ�Լ����
	//@remark
	//����
	//0������ɹ���
	//1����ʾ��������ʧ�ܣ�
	//2����ʾδ�������󳬹��������
	//3����ʾÿ�뷢�������������������
	cerr << "�������鶩������: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout << "��������Ϊ: " << pSpecificInstrument->InstrumentID << endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "ȡ����������Ϊ: " << endl;
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
		//���������ϵ�����Ҫ�շ�
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
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}
