/*
* The source code in this file is the property of 
* Combuilder and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/job_request_gui/src/ATSMMSSubmitter.cpp $
* @author: Noel R. Tajanlangit
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2012/03/20 14:12:57 $
* Last modified by:  $Noel R. Tajanlangit $
* 
* This is the implementation file of class ATSMMSSubmitter.
*
*/
#include <memory>
#include <time.h>
#include "ATSMMSSubmitter.h"
#include "AlarmStore.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
//#include "bus/alarm/ATS_alarm_bridge_library/src/DataStructInfo.h"
//#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreUpdaterMMSJobCommand.h"
//#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreReceiverATSProfile.h"

//using namespace I_ATS_ISCS_ALARM;

namespace TA_IRS_App
{
	ATSMMSSubmitter::ATSMMSSubmitter(void)
	{
	}

	ATSMMSSubmitter::~ATSMMSSubmitter(void)
	{
	}

	TA_Base_Bus::JobRequestDetailCorbaDef* ATSMMSSubmitter::getDataFromAlarmStore(std::string alarmID)
	{
		FUNCTION_ENTRY("getDataFromAlarmStore");

		TA_Base_Core::AlarmDetailCorbaDef_var alarm;

		std::string errorMessage = "NO_ERROR";
		TA_Base_Bus::EDependencyFailureError eType;

		try //try and get the alarm from an alarm agent
		{
			std::auto_ptr<AlarmStore> pAlarmStore(new AlarmStore);
			alarm = pAlarmStore->getAlarm(alarmID);

		}
		catch(TA_Base_Core::TransactiveException)
		{
			errorMessage = "The specified alarm could not be retrieved from the Alarm Store";
			eType = TA_Base_Bus::DEPENDENCYFAILURE_NO_SPECIFIED_ALARM;
		}
		catch(...)
		{
			errorMessage = "The Alarm Store could not be contacted";
			eType = TA_Base_Bus::DEPENDENCYFAILURE_ALARM_AGENT_NO_CONTACTED;
		}

		if("NO_ERROR" != errorMessage)
		{

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.c_str());
			throw TA_Base_Bus::DependencyFailure(errorMessage.c_str(),eType);
		}

		TA_Base_Bus::JobRequestDetailCorbaDef* details = new TA_Base_Bus::JobRequestDetailCorbaDef();

		details->assetId =CORBA::string_dup( alarm->assetName);

		// Retrieve profile ID
		//boost::shared_ptr<ATSProfileInfo> profile=TA_Base_Bus::DataStoreReceiverATSProfile::getInstance()->getProfile();
		//details->requestor = CORBA::string_dup(profile->operatorId.c_str());//string requestor;

		details->targetDateEnabled  = CORBA::Boolean(true);//boolean targetDateEnabled;

		time_t now;
		time(&now);
		details->targetDate = now + (24*60*60);//TA_Base_Core::DateTime targetDate;

		details->alarmDescField = CORBA::string_dup(alarm->alarmDescription.in());//string alarmDescField;

		details->problemReport  = CORBA::string_dup("");//string problemReport;
		
		//note that the assetId above is editable and may not
		//coorospond to this entity
		details->dataPointKey = alarm->assocEntityKey;//unsigned long entityKey;

		//these values are used if the JRM was launched via an alarm
		details->alarmUID = alarm->alarmID;//string alarmUID;
		details->locationKey = alarm->locationKey;//unsigned long locationKey;

		details->alarmTime =alarm->sourceTime.time;
		details->isAutoTrigger=false;
		details->alarmSeverity=alarm->alarmSeverity;

		details->alarmValue= CORBA::string_dup(alarm->alarmValue);

		FUNCTION_EXIT;
		return details;
	}

	bool ATSMMSSubmitter::submitJobRequest(TA_Base_Bus::JobRequestDetailCorbaDef& jobRequest)
	{
		FUNCTION_ENTRY("submitJobRequest");
		bool bRetVal = false;
        
		/*
		std::vector< boost::shared_ptr<AlarmMmsJobCommand> > selectedMmsJobCommand;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"MMSJOB ATS Alarm %s", jobRequest.alarmUID.in());
		boost::shared_ptr<AlarmMmsJobCommand>  mmsJobCommand (new AlarmMmsJobCommand());
		mmsJobCommand->targetDate=jobRequest.targetDate;
		mmsJobCommand->problemReport=jobRequest.problemReport.in();
		mmsJobCommand->alarmId=jobRequest.alarmUID.in();
		mmsJobCommand->operatorId=jobRequest.requestor.in();
		mmsJobCommand->assetName=jobRequest.assetId.in();
		selectedMmsJobCommand.push_back(mmsJobCommand);	

		bRetVal = TA_Base_Bus::DataStoreUpdaterMMSJobCommand::getInstance()->onDispatchObjects(selectedMmsJobCommand);*/
		
		return bRetVal;
		FUNCTION_EXIT;
	}

}
