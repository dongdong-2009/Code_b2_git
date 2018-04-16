#ifndef __IEC104__EVENT__H
#define __IEC104__EVENT__H
#include "Btype.h"
#include "FepDataStruct.h"
#include "Iec104.h"

/*
*class IEC104_Event_Handler
*packet all callback function in the class IEC104_Event_Handler
*define the handling function of each data.
*the meaning of paramters in every function see the corresponding document
*/

class IEC104_Event_Handler
{
public:
enum
{
 YK_SP = 0,
 YK_DP = 1,
 YK_RC = 3,
 YX_SP_NA = 1

};
public:
	 IEC104_Event_Handler(){};
	 ~IEC104_Event_Handler(){};

/*******************************************************************
*
*                         USED IN GUANG FO MRT 
*
********************************************************************/

	/// This method will be called when receive asdu type 1
	virtual void handleSinglePoint(int rtuAddr,b_ui8 bCot, vector<SSinglePoint> &vSinglePoint){};

	/// This method will be called when receive asdu type 30
	virtual void handleSinglePointWithTimeTag(int rtuAddr,b_ui8 bCot, vector<SSinglePointTimeTag> &vSinglePointTm){};

	/// This method will be called when receive asdu type 15
	virtual void handleIntegratedTotal(int rtuAddr,b_ui8 bCot,vector<SCounterValue>&vCounterValue) {};

	/// This method will be called when receive asdu type 11
	virtual void handleMeasureValueSVA(int rtuAddr, b_ui8 bCot,vector<SMeasuredValueSVA> &vMeasuredValueSVA){};

	/// This method will be called when receive asdu type 35
	virtual void handleMeasureValueSVAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSVATimeTag> &vMeasuredValueSVATm){};

	/// This method will be called when receive asdu type 45
	virtual void handleSingleCommand(int rtuAddr,int iInfoAddr,b_ui8 btCot,b_ui8 bSco){};

	///This method will be called when all links are not connected
	//virtual void handleAllLinkDown(){};

	///This method will be called when one link is close by connectThread actively
	//connectStatus=0,disconnect;
	//connectStatus=1,connect;
	virtual void handleLinkConnectStatusChanged(b_ui8 groupID,string &linkIP,int connectStatus){};

	///This method will be called when link which is used to transfer data changes
	//changeReason=0, link connect,                
	//changeReason=1, link disconnect,		
	//changeReason=2, pscada status SLAVE2MASTER	
	//changeReason=3, pscada status MASTER2SLAVE	
	virtual void handleDataTransferLinkChanged(b_ui8 newGroupID,string &newLinkIP,b_ui8 oldGroupID,string&oldLinkIP,int changeReason){};


//for the file transfer
	virtual void handle_File_Dir(int rtuAddr,int infoAddr,SFileInfo *FileInfo,int iCount){};
	virtual void handle_File_Ready(int rtuAddr,int infoAddr,b_ui16 fileName){};
	virtual void handle_Section_Ready(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 bSecName){};
	virtual void handle_Rx_Segment(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 bSecName,b_ui8 *btBuf,b_ui8 bLen){};
	virtual void handle_Last_Section(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 bSecName){};
	virtual void handle_Last_Segment(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 bSecName){};	
//for 103ex
	virtual void handle_Modify_Value(struct SExResult sResult){};
	virtual void handle_Call_Value(int rtuAddr,struct SExCallValueResult sResult){};
	virtual void handle_Protect_Action(SProtectActionRaw *pRaw,int iNum){};


	
//---------------------------------------------- -----------------------

	/// This method will be called when receive asdu type 3
	virtual void handleDoublePoint(int rtuAddr,b_ui8 bCot, vector<SDoublePoint> &vDoublePoint){};

	/// This method will be called when receive asdu type 31
	virtual void handleDoublePointWithTimeTag(int rtuAddr,b_ui8 bCot, vector<SDoublePointTimeTag> &vDoublePointTm){};

	/// This method will be called when receive asdu type 5
	virtual void handleStepPosition(int rtuAddr,b_ui8 bCot, vector<SStepPosition> &vStepPosition){};

	/// This method will be called when receive asdu type 32
	virtual void handleStepPositionWithTimeTag(int rtuAddr, b_ui8 bCot, vector<SStepPositionTimeTag> &vStepPositionTm){};

	/// This method will be called when receive asdu type 7
	virtual void handleBitstring32(int rtuAddr, b_ui8 bCot, vector<SBitstring32> &vBitstring32){};

	/// This method will be called when receive asdu type 33
	virtual void handleBitstring32WithTimeTag(int rtuAddr, b_ui8 bCot, vector<SBitstring32TimeTag> &vBitstring32Tm){};

	/// This method will be called when receive asdu type 9
	virtual void handleMeasureValueNVA(int rtuAddr, b_ui8 bCot,vector<SMeasuredValueNVA> &vMeasuredValueNVA){};

	/// This method will be called when receive asdu type 21
	virtual void handleMeasureValueNVAWithoutQualityDescriptor(int rtuAddr, b_ui8 bCot,vector<SMeasuredValueNVA2> &vMeasuredValueNVA2){};

	/// This method will be called when receive asdu type 34
	virtual void handleMeasureValueNVAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueNVATimeTag> &vMeasuredValueNVATm){};

	/// This method will be called when receive asdu type 13
	virtual void handleMeasureValueSFA(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSFA> &vMeasuredValueSFA){};

	/// This method will be called when receive asdu type 36
	virtual void handleMeasureValueSFAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSFATimeTag> &vMeasuredValueSFATm){};

	

	/// This method will be called when receive asdu type 37
	virtual void handleIntegratedTotalWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SCounterValueTimeTag>&vCounterValueTm) {};

	/// This method will be called when receive asdu type 20
	virtual void handlePackedSinglePoint(int rtuAddr,b_ui8 bCot,vector<SPackedSinglePoint>&vPackedSinglePoint) {};

	/// This method will be called when receive asdu type 28
	virtual void handleEventProtectionWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SEventProtectionTimeTag>&vEventProtectionTimeTag) {};

	/// This method will be called when receive asdu type 39
	virtual void handleStartEventsWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SStartEventsTimeTag>&vStartEventsTimeTag) {};

	/// This method will be called when receive asdu type 40
	virtual void handleCircuitInformationWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SCircuitInformationTimeTag>&vCircuitInformationTimeTag) {};

	/// This method will be called when receive asdu type 110
	virtual void handleParameterNVA(int rtuAddr,b_ui8 bCot,SParameterNVA paraNva){};

	/// This method will be called when receive asdu type 111
	virtual void handleParameterSVA(int rtuAddr,b_ui8 bCot,SParameterSVA paraSva){};

	/// This method will be called when receive asdu type 112
	virtual void handleParameterSFA(int rtuAddr,b_ui8 bCot,SParameterSFA paraSfa){};

	/// This method will be called when receive asdu type 113
	virtual void handleParameterActivation(int rtuAddr,b_ui8 btCot,SParameterAct  paraAct){};

	/// This method will be called when receive asdu type 58
	virtual void handleSingleCommandWithTimeTag(int rtuAddr,int iInfoAddr,b_ui8 btCot,b_ui8 bSco,b_ui8 bresult){};
	
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	virtual void logRecord(const char* fmt, ...){return;};
	virtual void handle_yk(int rtuAddr,int infoAddr,b_ui8 bVal,b_ui8  ykType,b_ui8  bresult,b_bool tmflag=0,SFepTime *ftime=0){};
	virtual void handle_sv(int rtuAddr,int infoAddr,b_ui8 * val,b_ui8 len,b_ui8 svType, b_ui8 bresult,b_bool tmflag=0){};
	//for logRecord
};

#endif
