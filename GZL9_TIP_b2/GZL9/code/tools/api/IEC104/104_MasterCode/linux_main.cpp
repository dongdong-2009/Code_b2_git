#pragma comment(lib,"../lib/104Library.lib")

#include <iostream>
#include "Iec104Api.h"
#include "Command.h"
#include "Iec104Event.h"
#include "MyEventHandler.h"
#include "Btype.h"
#include "LogThread.h"
#include "Profile.h"


using namespace esca;
int initpara(IEC104_API *api);
int sendCommand(IEC104_API *api);
void mylogRecord(int level,std::string &str);


int main()
{

	IEC104_API::init();
	LogThread::init();//log init
	IEC104_API *api;
	api = IEC104_API::getInstance();
	cout<<"104library version "<<api->getVersion()<<endl;
	if(initpara(api)==b_false)
	{
		printf("initpara error\n");
		return -1;
	}
	LogThread::instance()->Start();
	
	MyEvent_Handler myh;
	api->registerHandler(&myh);
	LogThread::instance()->openFile("log.txt",0);
	api->setLogCallBackFunc(mylogRecord);
	

	

	//api->select_Single_DO(1, 0x6001, 1, 1, 31);
	//api->select_Double_DO(1, 0x6002, 2, 1, 31);
	//api->select_RegulatingStep_DO(1, 0x6003, 2, 1, 31);
	b_ui16 nva=0xfe00;
	b_i16   sva =0xfe00;
	b_ui8 buff[]={0x41,0x08,0,0};
	float f=8.5;
	api->Start();
	for(;;)
	{
		int mode=0;
		cout<<"input workmode"<<endl;
		cin>>mode;
		if(mode<0)
			break;
		if(mode==0)
			api->setWorkMode(0);
		else
			api->setWorkMode(1);

	}
	//bsleep(5000);
	//api->setWorkMode(1);
	//sendCommand(api);
	//api->singleCommandSelect(1,0x6006,0);
	//api->singleCommandSelect(1,0x6006,1);
	//api->singleCommandSelect(1,0x6006,0);
	//api->singleCommandSelect(1,0x6006,1);	
	//api->select_SetPointNVA_DO(1, 0x6001,nva, 0, 127);
	//api->select_SetPointSVA_DO(1, 0x6001,sva, 0,0);	
	//api->select_SetPointSFA_DO(1, 0x6001, f, 0, 127);
	//api->interrogateGeneral(1, 6, 0x14);
	//api->interrogateGeneral(1, 6, 0x24);
	//api->interrogateGeneral(1, 8, 0x14);
	//api->interrogateGeneral(1, 8, 0x24);
	//api->interrogateCounter(1,6, 1, 0);
	//api->interrogateCounter(1,6, 4, 3);
	//api->interrogateCounter(1,8, 1, 0);
	//api->interrogateCounter(1,8, 4, 3);
	//api->read_command(1, 0x6001);
	//api->syncTime(1);
	//api->reset_process(1, 1);
	//api->reset_process(1, 2);
	//api->test_with_time(1, 1, 0);
	//api->test_with_time(1,1,0xfffe);
	//api->parameter_measured_value_NVA(1, 0x6001, nva,0);
	//api->parameter_measured_value_SVA(1, 0x6001, sva,0);
	//api->parameter_measured_value_SFA(1, 0x6001, f,0);
	//api->parameter_activation(1, 6, 1);
	//api->parameter_activation(1, 6, 3);
	//api->parameter_activation(1, 8, 1);
	//api->parameter_activation(1, 8, 3);
	api->Wait();
	api->stop();
	api->cleanUp();
	delete api;
	return 0;
}
/*
void main()
{
	CMutex::Init();
	LogThread::init();
	LogThread::instance()->Kill();
}*/



int initpara(IEC104_API *api)
{
	if(api==NULL)
		return -1;
	char* temp_string;		 
    	char real_config_file[DEF_FILENAME_LEN + 1];
#ifdef __OS_WIN32_
    _snprintf(real_config_file, DEF_FILENAME_LEN, "%s", DEF_CONFIG_FILE);
#else
    strncpy(real_config_file,DEF_CONFIG_FILE,DEF_FILENAME_LEN);
#endif
    	int T0=0,T1=0,T2=0,T3=0,T4=0,rtuAddr=0;
	int K=0,W=0;
	int AsduAddrLen=0,CotLen=0,InfAddrLen=0;
	int readRetries=0;
	std::string ipAddr,strLocalAddr;b_i16 port,grpID; 
	int num;
	try
	{
		Profile config_file(real_config_file);
		temp_string = config_file.get_value("LINK_PARA", "T0");
		if(!temp_string) {
			return -1;
		}
		T0=atoi(temp_string);

		temp_string = config_file.get_value("LINK_PARA", "T1");
		if(!temp_string) {
			return -1;
		}
		T1 = atoi(temp_string);

		temp_string = config_file.get_value("LINK_PARA", "T2");
		if(!temp_string) {
			return -1;
		}
		T2 = atoi(temp_string);

		temp_string = config_file.get_value("LINK_PARA", "T3");
		if(!temp_string) {
			return -1;
		}
		T3 = atoi(temp_string);

		temp_string = config_file.get_value("LINK_PARA", "T4");
		if(!temp_string) {
			return -1;
		}
		T4 = atoi(temp_string);

		temp_string = config_file.get_value("LINK_PARA", "W");
		if(!temp_string) {
			return -1;
		}
        	W  = atoi(temp_string);
        
		temp_string = config_file.get_value("LINK_PARA", "K");
		if(!temp_string) {
			return -1;
		}
		K  = atoi(temp_string);

		temp_string = config_file.get_value("APP_PARA", "AsduAddrLen");
		if(!temp_string) {
			return b_false;
		}
		AsduAddrLen=atoi(temp_string);

		temp_string = config_file.get_value("APP_PARA", "CotLen");
		if(!temp_string) {
			return b_false;
		}
		CotLen	   = atoi(temp_string);

		temp_string = config_file.get_value("APP_PARA", "InfAddrLen");
		if(!temp_string) {
			return b_false;
		}
		InfAddrLen=atoi(temp_string);

		temp_string = config_file.get_value("APP_PARA", "RtuAddr");
		if(!temp_string) {
			return b_false;
		}
		rtuAddr=atoi(temp_string);


		temp_string = config_file.get_value("SOCK_PARA", "ReadRetries");
		if(!temp_string) {
			return b_false;
		}
		readRetries=atoi(temp_string);
		
		temp_string = config_file.get_value("LINK_NUM", "linkNum");
		if(!temp_string) {
			return b_false;
		}
		num=atoi(temp_string);

		temp_string = config_file.get_value("LOCAL_ADDR", "localAddr");
		if(!temp_string)
		{
			return b_false;
		}
		strLocalAddr = temp_string ;
		
		char paraName[]="HOST1";
		for(int j=0;j<num;j++)
		{
			temp_string = config_file.get_value(paraName, "addr");
			if(!temp_string) {
			return b_false;
			}
			ipAddr = temp_string;

			temp_string = config_file.get_value(paraName, "port");
			if(!temp_string) {
			return b_false;
			}
			port = atoi(temp_string);

			temp_string = config_file.get_value(paraName, "groupID");
			if(!temp_string) {
			return b_false;
			}
			grpID= atoi(temp_string);
			paraName[4]++;
			//api->addHost(ipAddr, port, );
			api->addPScadaLink(grpID,ipAddr,port);

			
		}
		
	}
	catch (ProfileException& pe)
	{
		return b_false;
	}
	api->setT0(T0);
	api->setT1(T1);
	api->setT2(T2);
	api->setT3(T3); 
	api->setT4(T4);
	api->setRtuAddr(rtuAddr);
	api->setK(K);
	api->setW(W);
	api->setCotLen(CotLen);
	api->setAsduAddrLen(AsduAddrLen);
	api->setInfoAddrLen(InfAddrLen);
	api->setSockReadRetries(readRetries);
	api->setLocalAddr(strLocalAddr);
	return b_true;
}


int sendCommand(IEC104_API *api)
{
	int cmd,ret=0;
	while(1)
	{
		cout<<"<45>"<<"单点遥控"<<endl;
		cout<<"<100>"<<"总召唤"<<endl;
		cout<<"<101>"<<"电度量召唤"<<endl;
		cout<<"<103>"<<"时间同步"<<endl;
		cout<<"请输入要发送的报文类型\n";
		cin>>cmd;
		switch(cmd)
		{
		case 45:
			ret=api->singleCommandSelect(1,0x6006,0);
			break;
		case 100:
			ret=api->interrogateGeneral(1);
			break;
		case 101:
			ret=api->interrogateCounter(1);
			break;
		case 103:
			ret=api->syncTime(1);
			break;
		default:
			return 0;
		}
		if(ret==0)
			cout<<"send command "<<cmd<<" successfully"<<endl;
		else
			cout<<"send command"<<cmd<<" fail"<<endl;

	}
}

void mylogRecord(int level,std::string &str)
{
	LogThread::instance()->mylogRecord(level,str);
}