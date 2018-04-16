#ifndef __IEC104__API__H
#define __IEC104__API__H

//#include<pthread.h>
#include"Adaptee.h"
#include"Command.h"
#include "Iec104Event.h"
#include "Threads.h"
#include "Iec104.h"
#include "Iec104Def.h"
/* 
*class IEC104_API 
*packet all 104Library API in this class, used singleton
* At one system,there just only one instance of class IEC104_API,which are got by function of instance()
*
*/


struct SApi_Link_Para
{
	b_ui8		T0; 
	b_ui8		T1;
	b_ui8		T2;
	b_ui8		T3;
	b_ui8		T4;
	b_ui8		K;
	b_ui8		W;
	char              *HostAddr;
	int                Port;
	SApi_Link_Para()
	{
		K= IEC104_STD_K;
		W= IEC104_STD_W;
		T0= IEC104_STD_T0 ;
		T1= IEC104_STD_T1;
		T2= IEC104_STD_T2;
		T3= IEC104_STD_T3;
		T4 = IEC104_EX_T4;
		char tmpString[]="127.0.0.1";
		HostAddr = new char[MAX_PATH];
		strcpy(HostAddr,tmpString);
		Port = 2404;
	}
	~SApi_Link_Para()
	{
		delete[] HostAddr;
	}
};


class IEC104_API: public CThread  
{
friend class CIec104;
friend class ConnectThread;
friend class CAdaptee;
public:
	static IEC104_API * getInstance();
	static IEC104_API * instance();
	~IEC104_API();
	
#ifdef WIN32
	virtual DWORD Main();
#else
	virtual b_ui64 Main();
#endif
	//
	static void init();
	static void cleanUp();

	//stop 104library
	void stop();

	//set local ip addr used to communicate with peer.
	void setLocalAddr(std::string strLocalAddr);

	//get version of 104library
	std::string getVersion();

	//set 104library work mode
	void setWorkMode(int mode);

	//set 104library parameter T0
	void setT0(b_ui8 T0=30);

	//set 104library parameter T1
	void setT1(b_ui8 T1=15);

	//set 104library parameter T2
	void setT2(b_ui8 T2=10);

	//set 104library parameter T3
	void setT3(b_ui32 T3=20);

	//set 104library parameter T4
	void setT4(b_i32 T4=-1);

	//set 104library parameter K
	void setK(b_ui8 k=12);

	//set 104library parameter W
	void setW(b_ui8 w=8);

	//set 104library length of CAUSE OF TRANSMISSION
	void setCotLen(b_ui8 CotLen=2);

	void setRtuAddr(b_ui16 rtuAddr);
	
	//set 104library length of COMMON ADDRESS OF ASDU
	void setAsduAddrLen(b_ui8 AsduAddrLen=2);

	//set 104library length of INFORMATION OBJECT ADDRESS
	void setInfoAddrLen(b_ui8 InfoAddrLen=3);

	//add pscada host parameter,same host use the same groupid; 
	int addPScadaLink(int groupId,const std::string strIpAddr,int port=2404);

	//register handler for 104library
	int registerHandler(IEC104_Event_Handler *handler);

	//if socked read fail, it will retry tryTimes
	int setSockReadRetries(int tryTimes);

//------------------------------------commands are belong to GuangFo----------------------

	//used to send single command select,asdu type 45,58
	int singleCommandSelect(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 Scs,b_ui8 QU=0);

	//used to send single command execute,asdu type 45,58
	int singleCommandExec(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 Scs, b_ui8 QU=0);

	//to send single command cancel,asdu type 45,58
	int singleCommandUndo(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 Scs,b_ui8 QU=0);

	//to send Clock synchronization command,asdu type 103
	int syncTime(int iRtuAddr);

	//to send Counter interrogation command,asdu type 101
	int interrogateCounter(b_ui16 iRtuAddr,b_ui8 bCot =6, b_ui8 bRqt=1,b_ui8 bFrz=0);

	//to send Interrogation command,asdu type 100
	int interrogateGeneral(b_ui16 iRtuAddr,b_ui8 bCot =6, b_ui8 bQoi=20);

	//follow are used for file transfer.
	int call_file_dir(int rtuAddr,int infoAddr,b_ui16 fileName=0,b_ui8 secName=0);
	
	int call_file(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 secName);
	
	int call_file_section(int rtuAddr,int infoAddr, b_ui16 fileName,b_ui8 secName);
	
	int select_file(int rtuAddr,int infoAddr, b_ui16 fileName,b_ui8 secName);
	
	int select_file_section(int rtuAddr, int infoAddr, b_ui16 fileName, b_ui8 secName);
	
	int affirm_file(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 secName,b_ui8 affirmFlag);
	//103 extension
	int call_value(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo);//145
	
	int call_value_area_no(int rtuAddr, b_ui8 protectDevNo);//145

	int modify_value_select(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo,float *fBuf, int count);
	
	int modify_value_exec(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo);
	
	int modify_value_cancle(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo);
	
	int modify_value_are_no_select(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo);
	
	int modify_value_area_no_exec(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo);
	
	int modify_value_area_no_cancle(int rtuAddr, b_ui8 protectDevNo, b_ui8 areaNo);

//-------------------------------------not belong to GuangFo----------------------

	//to send double command select,asdu type 46,59
	int select_Double_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bDcs,b_bool tmflag=b_false,b_ui8 bQu=0);

	//to send double command excute,asdu type 46,59
	int exec_Double_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bDcs,b_bool tmflag =b_false,b_ui8 bQu=0);

	//to send double command cancle,asdu type 46,59
	int undo_Double_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bDcs,b_bool tmflag=b_false,b_ui8 bQu=0);

	//to send Regulating step command select ,asdu type 47,60
	int select_RegulatingStep_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bRcs,b_bool tmflag=b_false,b_ui8 bQu=0);

	//to send Regulating step command excute ,asdu type 47,60
	int exec_RegulatingStep_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bRcs,b_bool tmflag=b_false,b_ui8 bQu=0);

	//to send Regulating step command cancel ,asdu type 47,60
	int undo_RegulatingStep_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui8 bRcs,b_bool tmflag=b_false,b_ui8 bQu=0);
	
	//to send Set-point command select, normalized value,asdu type 48,61
	int select_SetPointNVA_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 Nva,b_bool tmflag=b_false,b_ui8 bQl=0);

	//to send Set-point command excute, normalized value,asdu type 48,61
	int exec_SetPointNVA_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 Nva,b_bool tmflag=b_false,b_ui8 bQl=0);

	//to send Set-point command cancel, normalized value,asdu type 48,61
	int undo_SetPointNVA_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 Nva,b_bool tmflag=b_false,b_ui8 bQl=0);

	//to send Set-point command select, scaled value,asdu type 49,62
	int select_SetPointSVA_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 Sva,b_bool tmflag=b_false,b_ui8 bQl=0);

	//to send Set-point command excute, scaled value,asdu type 49,62
	int exec_SetPointSVA_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 Sva,b_bool tmflag=b_false,b_ui8 bQl=0);

	//to send Set-point command cancel, scaled value,asdu type 49,62
	int undo_SetPointSVA_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 Sva,b_bool tmflag=b_false,b_ui8 bQl=0);

	//to send Set-point command select, Set-point command, short floating point number,asdu type 50,63
	int select_SetPointSFA_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float Sfa,b_bool tmflag=b_false,b_ui8 bQl=0);

	//to send Set-point command excute, Set-point command, short floating point number,asdu type 50,63
	int exec_SetPointSFA_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float Sfa,b_bool tmflag=b_false,b_ui8 bQl=0);

	//to send Set-point command cancel, Set-point command, short floating point number,asdu type 50,63
	int undo_SetPointSFA_DO(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float Sfa,b_bool tmflag=b_false,b_ui8 bQl=0);

	//to send Read command,asdu type 102
	int read_command(b_ui16 iRtuAddr,b_ui32 iInfoAddr);

	//to send Reset process command,asdu type 105
	int reset_process(b_ui16 iRtuAddr,b_ui8 bQrp);

	//to send Test command with time tag,asdu type 107
	int test_with_time(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 bTcs);


	//to send Parameter of measured values, normalized value,asdu type 110
	int parameter_measured_value_NVA(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_ui16 nva,b_ui8 bQpm);

	//to send Parameter of measured values, scaled value,asdu type 111
	int parameter_measured_value_SVA(b_ui16 iRtuAddr,b_ui32 iInfoAddr,b_i16 sva,b_ui8 bQpm);

	//to send Parameter of measured values, short floating point number,asdu type 112
	int parameter_measured_value_SFA(b_ui16 iRtuAddr,b_ui32 iInfoAddr,float sfa,b_ui8 bQpm);

	// to send Parameter activation,asdu type 113
	int parameter_activation(b_ui16  rtuAddr,b_ui8 bCot,b_ui8 bQpa);
	//void LogDebug(const char* fmt, ...);
	int close();
	//call back function
	int setLogCallBackFunc(void (*logCallBack)(int level,std::string &str))
	{
		if(logCallBack==NULL)
			return -1;
		this->logCallBackFunc = logCallBack;
		return 0;
	}
	void logRecord(int level,std::string &str)
	{
		if(logCallBackFunc!=NULL)
			(*logCallBackFunc)(level,str);
	}
	
	
//-----------------------------------end------------------------
private:
	IEC104_API(); 
	int getCommand(CCommand &cmd);
	void putCommand(const CCommand &cmd);
private:
	static IEC104_API *api_;
	static esca::CMutex* m_api_mutex;
	IEC104_Event_Handler *handler_;
	CAdaptee* ade_;
	CCommandManager *cmdMng_;
	esca::CMutex         m_mngMutex;
	
	void (*logCallBackFunc)(int level,std::string &str);	
public:
	SIec104_LINK_PARA m_linkPara;	
	SIec104_APP_PARA	  m_AppPara;
	
	
	
};

#endif

