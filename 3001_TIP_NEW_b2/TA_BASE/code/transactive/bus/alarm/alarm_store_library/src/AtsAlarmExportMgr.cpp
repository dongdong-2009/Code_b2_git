
#include "AtsAlarmExportMgr.h"
#include "AtsAlarmXmlWriter.h"
#include "core/utilities/src/DebugUtil.h"
#include <sstream>

using namespace std;

namespace TA_Base_Bus
{
	AtsAlarmExportMgr::AtsAlarmExportMgr(const std::string& folderPath, unsigned long exportInterval)
		: m_pAtsXmlWriter(0)
	{
		m_pAtsXmlWriter = new AtsAlarmXmlWriter(folderPath, exportInterval);
	}

	AtsAlarmExportMgr::~AtsAlarmExportMgr()
	{
		stopProcessing();
	}

	void AtsAlarmExportMgr::startProcessing()
	{
		start();
		m_pAtsXmlWriter->start();
	}

	void AtsAlarmExportMgr::stopProcessing()
	{
		m_pAtsXmlWriter->terminateAndWait();
		terminateAndWait();
	}

	void AtsAlarmExportMgr::processEvent( boost::shared_ptr<ATSAlarmQueueItem> newEvent )
	{
		FUNCTION_ENTRY("processEvent");

		AtsXmlDataPtr newData(new AtsXmlData);
		stringstream temp;
		
		// Source Time
		temp << newEvent->detail.sourceTime.time;
		newData->sourceTime_time = temp.str();
		temp.str("");

		temp << newEvent->detail.sourceTime.millitm;
		newData->sourceTime_mili = temp.str();
		temp.str("");
		// Alarm ID
		newData->alarmID = newEvent->detail.alarmID;    
		// Acknowledge time
		temp << newEvent->detail.ackTime;
		newData->ackTime = temp.str();
		temp.str("");
		// Close Time
		temp << newEvent->detail.closeTime;
		newData->closeTime = temp.str();
		temp.str("");
		// Asset Name
		newData->assetName = newEvent->detail.assetName;  
		// Severity
		temp << newEvent->detail.alarmSeverity;
		newData->alarmSeverity = temp.str();
		temp.str("");
		// Alarm Description
		newData->alarmDescription = newEvent->detail.alarmDescription;
		// Acknowledge by
		newData->alarmAcknowledgeBy = newEvent->detail.alarmAcknowledgeBy;
		// State
		temp << (int)newEvent->detail.state;
		newData->state = temp.str();
		temp.str("");
		// Locaiton ID
		newData->locationId = newEvent->detail.locationId;
		// Parent alarm ID
		newData->parentAlarmID = newEvent->detail.parentAlarmID;   
		// Avalance Head ID
		newData->avalancheHeadID = newEvent->detail.avalancheHeadID; 
		// Is Head of Avalanche
		temp << (int)newEvent->detail.isHeadOfAvalanche;
		newData->isHeadOfAvalanche = temp.str();
		temp.str("");
		// Is Child of Avalanche
		temp << (int)newEvent->detail.isChildOfAvalanche;
		newData->isChildOfAvalanche = temp.str();
		temp.str("");
		// MMS State
		temp << (int)newEvent->detail.mmsState;
		newData->mmsState = temp.str();
		temp.str("");
		// Alarm Value
		newData->alarmValue = newEvent->detail.alarmValue;
		// OM Alarm
		temp << (int)newEvent->detail.omAlarm;
		newData->omAlarm = temp.str();
		temp.str("");
		// Alarm Type
		temp << (int)newEvent->detail.alarmType;
		newData->alarmType = temp.str();
		temp.str("");
		// SubSystem Key
		temp << newEvent->detail.subsytemkey;
		newData->subsytemkey = temp.str();
		temp.str("");
		// System Key
		temp << newEvent->detail.systemkey;
		newData->systemkey = temp.str();
		temp.str("");
		// Alarm Comments
		newData->alarmComments = newEvent->detail.alarmComments;
		// String Alarm Type
		newData->strAlarmType = newEvent->detail.strAlarmType;
		// Subsystem Type
		newData->subsytemType = newEvent->detail.subsytemType;
		// System Type
		newData->systemkeyType = newEvent->detail.systemkeyType;
		// Location Key
		temp << newEvent->detail.locationKey;
		newData->locationKey = temp.str();
		temp.str("");

		m_pAtsXmlWriter->insert(newData);

		FUNCTION_EXIT;
	}

}