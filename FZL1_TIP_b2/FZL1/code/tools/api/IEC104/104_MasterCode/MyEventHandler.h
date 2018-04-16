#ifndef _MYEVENT_HANDLER__
#define _MYEVENT_HANDLER__

#include "Iec104Event.h"
#include "Btype.h"
#include "FepDataStruct.h"
#include "Iec104.h"

class MyEvent_Handler:public IEC104_Event_Handler
{
public:
	MyEvent_Handler(){};
	~MyEvent_Handler(){};


/******************************************************************
*
*                                 USED IN GUANG FO MRT
*
******************************************************************/

	/// This method will be called when receive asdu type 1
	void handleSinglePoint(int rtuAddr,b_ui8 bCot, vector<SSinglePoint> &vSinglePoint);

	/// This method will be called when receive asdu type 30
	void handleSinglePointWithTimeTag(int rtuAddr,b_ui8 bCot, vector<SSinglePointTimeTag> &vSinglePointTm);

	/// This method will be called when receive asdu type 11
	void handleMeasureValueSVA(int rtuAddr, b_ui8 bCot,vector<SMeasuredValueSVA> &vMeasuredValueSVA);

	/// This method will be called when receive asdu type 35
	void handleMeasureValueSVAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSVATimeTag> &vMeasuredValueSVATm);

	/// This method will be called when receive asdu type 15
	void handleIntegratedTotal(int rtuAddr,b_ui8 bCot,vector<SCounterValue>&vCounterValue);

	/// This method will be called when receive asdu type 45
	void handleSingleCommand(int rtuAddr,int iInfoAddr,b_ui8 btCot,b_ui8 bSco);

//for link change
	///This method will be called when all links are not connected
	//void handleAllLinkDown();

	///This method will be called when one link is close by connectThread actively
	void handleLinkConnectStatusChanged(b_ui8 groupID,string &linkIP,int connectStatus);

	///This method will be called when link which is used to transfer data changes
	void handleDataTransferLinkChanged(b_ui8 newGroupID,string &newLinkIP,b_ui8 oldGroupID,string&oldLinkIP,int changeReason);

//for file trans
	void handle_File_Dir(int rtuAddr,int infoAddr,SFileInfo *FileInfo,int iCount);
	void handle_File_Ready(int rtuAddr,int infoAddr,b_ui16 fileName);
	void handle_Section_Ready(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 bSecName);
	void handle_Rx_Segment(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 bSecName,b_ui8 *btBuf,b_ui8 bLen);
	void handle_Last_Section(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 bSecName);
	void handle_Last_Segment(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 bSecName);
//for 103ex
	void handle_Modify_Value(struct SExResult sResult);
	void handle_Call_Value(int rtuAddr, struct SExCallValueResult sResult);
	void handle_Protect_Action(SProtectActionRaw *pRaw,int iNum);

//-------------------------------------------------------------------------------------

	/// This method will be called when receive asdu type 3
	void handleDoublePoint(int rtuAddr,b_ui8 bCot, vector<SDoublePoint> &vDoublePoint);

	/// This method will be called when receive asdu type 31
	void handleDoublePointWithTimeTag(int rtuAddr,b_ui8 bCot, vector<SDoublePointTimeTag> &vDoublePointTm);

	/// This method will be called when receive asdu type 5
	void handleStepPosition(int rtuAddr,b_ui8 bCot, vector<SStepPosition> &vStepPosition);

	/// This method will be called when receive asdu type 32
	void handleStepPositionWithTimeTag(int rtuAddr, b_ui8 bCot, vector<SStepPositionTimeTag> &vStepPositionTm);

	/// This method will be called when receive asdu type 7
	void handleBitstring32(int rtuAddr, b_ui8 bCot, vector<SBitstring32> &vBitstring32);

	/// This method will be called when receive asdu type 33
	void handleBitstring32WithTimeTag(int rtuAddr, b_ui8 bCot, vector<SBitstring32TimeTag> &vBitstring32Tm);

	//模拟量

	/// This method will be called when receive asdu type 9
	void handleMeasureValueNVA(int rtuAddr, b_ui8 bCot,vector<SMeasuredValueNVA> &vMeasuredValueNVA);

	/// This method will be called when receive asdu type 21
	void handleMeasureValueNVAWithoutQualityDescriptor(int rtuAddr, b_ui8 bCot,vector<SMeasuredValueNVA2> &vMeasuredValueNVA2);

	/// This method will be called when receive asdu type 34
	void handleMeasureValueNVAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueNVATimeTag> &vMeasuredValueNVATm);



	/// This method will be called when receive asdu type 13
	void handleMeasureValueSFA(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSFA> &vMeasuredValueSFA);

	/// This method will be called when receive asdu type 36
	void handleMeasureValueSFAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSFATimeTag> &vMeasuredValueSFATm);
	//累计量

	/// This method will be called when receive asdu type 37
	void handleIntegratedTotalWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SCounterValueTimeTag>&vCounterValueTm);

	/// This method will be called when receive asdu type 20
	void handleEventProtectionWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SEventProtectionTimeTag>&vEventProtectionTimeTag);

	/// This method will be called when receive asdu type 28
	void handleStartEventsWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SStartEventsTimeTag>&vStartEventsTimeTag);

	/// This method will be called when receive asdu type 39
	void handleCircuitInformationWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SCircuitInformationTimeTag>&vCircuitInformationTimeTag);

	/// This method will be called when receive asdu type 40
	void handlePackedSinglePoint(int rtuAddr,b_ui8 bCot,vector<SPackedSinglePoint>&vPackedSinglePoint);

	/// This method will be called when receive asdu type 110
	void handleParameterNVA(int rtuAddr,b_ui8 bCot,SParameterNVA paraNva);

	/// This method will be called when receive asdu type 111
	void  handleParameterSVA(int rtuAddr,b_ui8 bCot,SParameterSVA paraSva);

	/// This method will be called when receive asdu type 112
	void handleParameterSFA(int rtuAddr,b_ui8 bCot,SParameterSFA paraSfa);

	/// This method will be called when receive asdu type 113
	void handleParameterActivation(int rtuAddr,b_ui8 btCot,SParameterAct  paraAct);


	void handle_yk(int rtuAddr,int infoAddr,b_ui8 bVal,b_ui8  ykType,b_ui8  bresult,b_bool tmflag=0,SFepTime *ftime=0);
	void handle_sv(int rtuAddr,int infoAddr,b_ui8 * val,b_ui8 len,b_ui8 svType, b_ui8 bresult,b_bool tmflag=0);
	//for logRecord
	void displayData(const char* fmt, ...);
private:
	int writeToFile(b_ui8 *buf,int size,int name);


//	void handler_yx(int rtuAddr,int infoAddr,void * val,b_ui8 len,b_ui8 yxType){};
};
#endif
