#ifndef _WF_EVENT_HANDLER__
#define _WF_EVENT_HANDLER__

#include "core/timers/src/SingletonTimerUtil.h"
#include "bus/scada/104_library/src/Iec104Event.h"
#include "bus/scada/104_library/src/Btype.h"
#include "bus/scada/104_library/src/FepDataStruct.h"
#include "bus/scada/104_library/src/Iec104.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "app/scada/WFAgent/src/WFMsgTimer.h"
#include "app/scada/WFAgent/src/WFUnit.h"
#include "app/scada/WFAgent/src/WFDpProcessor.h"

#include <map>

namespace TA_Base_Bus
{
	class DataPoint;
};

namespace TA_IRS_App
{
	class WfMsgTimer;
	class WfUnit;
};

namespace TA_IRS_App
{
	class WfEventHandler : public IEC104_Event_Handler,
                           public TA_Base_Core::ITimeoutCallback
	{ 
		typedef std::map < unsigned long, TA_Base_Bus::DataPoint * > WfDataPointMap;

	private:
		WfDataPointMap			m_InputDataPoints;
		WfDataPointMap			m_OutputDataPoints;
		TA_IRS_App::WfMsgTimer* m_wfMsgTimer;
		TA_IRS_App::WfUnit*		m_wfUnit;
		TA_IRS_App::WfDpProcessor* m_wfDpProcessor;
		volatile bool				m_isConnectionTimerStarted;
		unsigned long				m_lastUpdateTimeSec;
		unsigned long				m_updateLostPeriodTime;
		long						m_monitorConnectionTimeId;
		long						m_monitorLastUpdateTimeId;
		volatile bool				m_linkStatusBadToGood; 
		volatile bool				m_isInControl; 
		static const int		    s_UpdateMonitorTimeSlice;
		TA_Base_Core::ReEntrantThreadLockable  m_linkStatusLock;

	private:
		void handleUpdateTimeOut();
		
	public:
		WfEventHandler();
		~WfEventHandler();
		/* 
		* This operation will be called when the registered timeout period has expired.
		*/
		virtual void timerExpired(long timerId, void* userData);
		
		void setWfUnit(TA_IRS_App::WfUnit* wfUnit);
		void setMsgTimer(TA_IRS_App::WfMsgTimer* wfMsgTimer);
		//For Boolean Data Point
		void handleSinglePoint(int rtuAddr,b_ui8 bCot, vector<SSinglePoint> &vSinglePoint);
		void handleSinglePointWithTimeTag(int rtuAddr,b_ui8 bCot, vector<SSinglePointTimeTag> &vSinglePointTm);

		//For Analog DataPoint 
		void handleMeasureValueSVA(int rtuAddr, b_ui8 bCot,vector<SMeasuredValueSVA> &vMeasuredValueSVA);
		void handleMeasureValueSVAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSVATimeTag> &vMeasuredValueSVATm);

		//for GZL6 Analog DataPoint use 
		void handleMeasureValueSFA(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSFA> &vMeasuredValueSFA);
		void handleMeasureValueSFAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSFATimeTag> &vMeasuredValueSFATm);

		//For MI Data Point
		void handleIntegratedTotal(int rtuAddr,b_ui8 bCot,vector<SCounterValue>&vCounterValue);
		void handleIntegratedTotalWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SCounterValueTimeTag>&vCounterValueTm);

		//For DO Data Point
		void handleSingleCommand(int rtuAddr,int iInfoAddr,b_ui8 btCot,b_ui8 bSco,b_ui8 bOriAddr);

		void handleInterrogateGeneral(b_ui16 iRtuAddr,b_ui8 bCot , b_ui8 bQoi);

		void handleInterrogateCounter(b_ui16 iRtuAddr,b_ui8 bCot , b_ui8 bRqt,b_ui8 bFrz);

		bool addDataPoint( TA_Base_Bus::DataPoint * dp );

		void removeDataPoint( TA_Base_Bus::DataPoint * dp );


		///This method will be called when one link is close by connectThread actively
		//connectStatus=0,disconnect;
		//connectStatus=1,connect;
		void handleLinkConnectStatusChanged(b_ui8 groupID,string &linkIP,int connectStatus);

		void handleLinkModeStatus(b_ui8 groupID,string &linkIP,int mode);

		///This method will be called when link which is used to transfer data changes
		//changeReason=0, link connect,                
		//changeReason=1, link disconnect,		
		//changeReason=2, pscada status SLAVE2MASTER	
		//changeReason=3, pscada status MASTER2SLAVE	
		void handleDataTransferLinkChanged(b_ui8 newGroupID,string &newLinkIP,b_ui8 oldGroupID,string&oldLinkIP,int changeReason);
		
		void setOperationMode(bool bControl);

		void stop();
	};
}
#endif
