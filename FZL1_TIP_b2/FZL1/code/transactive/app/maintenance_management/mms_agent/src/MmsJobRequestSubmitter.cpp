/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/maintenance_management/mms_agent/src/MmsJobRequestSubmitter.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #3 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2012/09/21 14:49:12 $
  * Last mofified by:  $Author: huangjian $
  *
  * MmsJobRequestSubmitter class implimentation. See header file for class descrption
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786 4503)
#endif // defined (_MSC_VER)



#include "app/maintenance_management/mms_agent/src/MmsJobRequestSubmitter.h"

#include "core/data_access_interface/entity_access/src/MmsJobRequestSubmitterEntityData.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/utilities/src/TAAssert.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/IDL/src/AlarmAgentCorbaDef.h"
#include "core/exceptions/src/NoAlarmAgentException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "app/maintenance_management/mms_agent/src/MmsConnection.h"
#include "app/maintenance_management/mms_agent/src/MmsServerState.h"

#include "app/maintenance_management/mms_agent/src/MmsMessageUtil.h"

#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/maintenance_management/mmsHelper/src/MMSUtility.h"
#include "bus/maintenance_management/mmsHelper/src/MMSAPIManager.h"
#include <algorithm>

namespace TA_IRS_App
{

	MmsJobRequestSubmitter::MmsJobRequestSubmitter()
	{
		FUNCTION_ENTRY("MmsJobRequestSubmitter");

		

		FUNCTION_EXIT;
	}


	MmsJobRequestSubmitter::~MmsJobRequestSubmitter()
	{
		FUNCTION_ENTRY("~MmsJobRequestSubmitter");

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
		FUNCTION_EXIT;
	}

 //IMmsJobRequestSubmitterCorbaDef methods:


	TA_Base_Bus::JobRequestDetailCorbaDef* MmsJobRequestSubmitter::getDataFromAlarm(
		const char* alarmUID, CORBA::ULong alarmLocationKey, const char* sessionID)
	{

		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_lock );

		FUNCTION_ENTRY("getDataFromAlarm");

		checkOperationMode();
		
		TA_Base_Core::AlarmDetailCorbaDef_var alarm;

		std::string errorMessage = "NO_ERROR";
		TA_Base_Bus::EDependencyFailureError eType;
        
  		try //try and get the alarm from an alarm agent
		{
			alarm = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().getAlarmAtLocation(alarmUID, alarmLocationKey);
		}
		catch(TA_Base_Core::NoAlarmAgentException)
		{
			errorMessage = "The Alarm Agent could not be contacted";
			eType = TA_Base_Bus::DEPENDENCYFAILURE_ALARM_AGENT_NO_CONTACTED;
		}
		catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException)
		{
			errorMessage = "The specified alarm could not be retrieved from the Alarm Agent";
			eType = TA_Base_Bus::DEPENDENCYFAILURE_NO_SPECIFIED_ALARM;
		}
		catch(...)
		{
			errorMessage = "The Alarm Agent could not be contacted";
			eType = TA_Base_Bus::DEPENDENCYFAILURE_ALARM_AGENT_NO_CONTACTED;
		}

		if("NO_ERROR" != errorMessage)
		{

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.c_str());
			throw TA_Base_Bus::DependencyFailure(errorMessage.c_str(),eType);
		}

		TA_Base_Bus::JobRequestDetailCorbaDef* details = new TA_Base_Bus::JobRequestDetailCorbaDef();

		details->assetId =CORBA::string_dup( alarm->assetName);//string assetId;

		//details->dataNodeKey = MmsMessageUtil::getInstance()->getNodeKey(alarm->assocEntityKey);

		TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(alarm->assocEntityKey);

		std::auto_ptr<TA_Base_Core::DataNodeEntityData> nodeData(dynamic_cast<TA_Base_Core::DataNodeEntityData*> (entityData));

		if(0 == nodeData.get())  
		{
			details->dataNodeKey =0;
			details->alarmFaultCode= CORBA::string_dup("");
		}
		else
		{
			details->dataNodeKey =nodeData->getKey();
			std::string faultcode="";
			std::string  nodeName=nodeData->getName();
			faultcode=TA_Base_Bus::MMSUtility::getAlarmFaultCode(nodeName);
			details->alarmFaultCode=CORBA::string_dup(faultcode.c_str());
		}

        // ++AZ : corrected corba string usage
		details->requestor = CORBA::string_dup(getOperatorDesc(sessionID).c_str());//string requestor;

		details->targetDateEnabled  = CORBA::Boolean(true);//boolean targetDateEnabled;

		time_t now;
		time(&now);
		details->targetDate = now + (24*60*60);//TA_Base_Core::DateTime targetDate;

		std::string alarmDesc;
		std::string probReport;

		MmsConnection::getInstance()->getDescAndReportFields(alarm.in(), alarmDesc, probReport);

		
 

		details->alarmDescField = CORBA::string_dup(alarmDesc.c_str());//string alarmDescField;

		details->problemReport  = CORBA::string_dup(probReport.c_str());//string problemReport;
        // AZ++

		//the following values are populated by the agent
		//and should not be modified, or even used by the client:

		//note that the assetId above is editable and may not
		//coorospond to this entity
		details->dataPointKey = alarm->assocEntityKey;//unsigned long entityKey;

		//these values are used if the JRM was launched via an alarm
		details->alarmUID = alarm->alarmID;//string alarmUID;
		details->locationKey = alarmLocationKey;//unsigned long locationKey;

        details->alarmTime =alarm->sourceTime.time;
		details->isAutoTrigger=false;
		details->alarmSeverity=alarm->alarmSeverity;
		
		details->alarmValue= CORBA::string_dup(alarm->alarmValue);


		FUNCTION_EXIT;

		return details;
	}




	TA_Base_Bus::JobRequestDetailCorbaDef* MmsJobRequestSubmitter::getDataFromSymbol(const char* entityName, 
		const char* sessionID)
	{

		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_lock );

		FUNCTION_ENTRY("getDataFromSymbol");

		checkOperationMode();

		TA_Base_Bus::JobRequestDetailCorbaDef* details = new TA_Base_Bus::JobRequestDetailCorbaDef();

		try
		{
			TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
			
			std::auto_ptr<TA_Base_Core::DataNodeEntityData> nodeData(dynamic_cast<TA_Base_Core::DataNodeEntityData*> (entityData));

			if(0 == nodeData.get()) throw "bummer";

            // ++AZ : corrected corba string usage
			details->assetId = CORBA::string_dup(nodeData->getAssetName().c_str());

			//note that the assetId above is editable and may not
			//coorospond to this entity
			details->dataNodeKey = nodeData->getKey();//unsigned long entityKey;

		}
		catch(...)
		{
			std::string errorMessage = "Configuration for the specified datapoint could not be retrieved.";
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.c_str());
			throw TA_Base_Bus::DependencyFailure(errorMessage.c_str(),TA_Base_Bus::DEPENDENCYFAILURE_NO_SPECIFIED_DATAPOINT);
		}


		details->requestor = getOperatorDesc(sessionID).c_str();//string requestor;

		details->targetDateEnabled  = CORBA::Boolean(true);//boolean targetDateEnabled;

		time_t now;
		time(&now);
		details->targetDate = now + (24*60*60);//TA_Base_Core::DateTime targetDate;

		details->alarmDescField = CORBA::string_dup("");//string alarmDescField;

		details->problemReport  = CORBA::string_dup("");//string problemReport;

		//these values are used only if the JRM was launched via an alarm
		details->alarmUID = CORBA::string_dup("NOT_FROM_ALARM");//string alarmUID;
        // AZ++
		details->locationKey = 0;//unsigned long locationKey;
		details->dataPointKey = 0;
		details->isAutoTrigger=false;
        details->alarmSeverity=0;
		details->alarmFaultCode= CORBA::string_dup("");
		details->alarmValue= CORBA::string_dup("");
	  
		 

		FUNCTION_EXIT;

		return details;
	}


	void MmsJobRequestSubmitter::submitJobRequest(const TA_Base_Bus::JobRequestDetailCorbaDef& jobRequest, const char* sessionID)	 
	{

		//Thread guard this method
		TA_Base_Core::ThreadGuard guard( m_lock );

		FUNCTION_ENTRY("submitJobRequest");

		checkOperationMode();

        bool isJobRequestPermitted;
        try
        {
            isJobRequestPermitted = MmsConnection::getInstance()->isRaiseJobRequestPermitted(sessionID, jobRequest.dataNodeKey);
        }
        catch(TA_Base_Core::AccessDeniedException&)
        {
			throw TA_Base_Bus::SubmissionDenied("Rights could not be determined for this action.", true, TA_Base_Bus::SUBMISSIONDENIED_NO_PRIVILEGE);
        }

		if(!isJobRequestPermitted)
		{
			std::string message = "Operator does not have sufficient privileges to raise Job Request.";
			throw TA_Base_Bus::SubmissionDenied(message.c_str(), true , TA_Base_Bus::SUBMISSIONDENIED_NO_PRIVILEGE);
		}

		//if this is being submitted from an alarm...
		if( 0 != strcmp("NOT_FROM_ALARM", jobRequest.alarmUID.in()) )
		{
			//check if the alarm state is ok
			std::string reason;
			TA_Base_Bus::ESubmissionDeniedError eType;
			if(false == alarmStateIsOk(jobRequest, reason, eType) )
			{				
				throw TA_Base_Bus::SubmissionDenied(reason.c_str(), false, eType);
			}
			
			//check if the MMS inhibit state is ok
			bool isInhibited = true;
			try
			{
				isInhibited = TA_Base_Bus::ScadaUtilities::getInstance().isDataPointMmsInhibited(jobRequest.dataPointKey);
			}
			catch(...)
			{
				throw TA_Base_Bus::SubmissionDenied("Could not determine the MMS inhibition state.", false, TA_Base_Bus::SUBMISSIONDENIED_NO_DETERMINE_INHIBITION_STATE);
			}
			if( isInhibited )
			{	
				
				throw TA_Base_Bus::SubmissionDenied("MMS inhibition is applied.", false, TA_Base_Bus::SUBMISSIONDENIED_MMS_INHIBITED_APPLYED);
			}

		}
		else //not from alarm so just check global inhibit state
		{
			if( MmsServerState::getInstance()->getGlobalInhibit() )
			{
				throw TA_Base_Bus::SubmissionDenied("Global MMS inhibition is applied.", false, TA_Base_Bus::SUBMISSIONDENIED_GLOBAL_MMS_INHIBITED);
			}
		}

		try
		{
			//MmsConnection::getInstance()->submitJobRequestMessage(jobRequest);
			TA_Base_Bus::MMSAPIManager::getInstance()->submitJobRequestMessage(jobRequest);
		}
		catch(TA_Base_Bus::MmsConnectionFailure& ex)
		{
			MmsServerState::getInstance()->reportMmsServerFailure(ex);
			std::string msg = "MMS Server communication failure";
			throw TA_Base_Bus::DependencyFailure(msg.c_str(),TA_Base_Bus::DEPENDENCYFAILURE_MMS_SERVER_NO_CONTACTED);
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"MmsConnection::submitJobRequestMessage was successful");


		//if here the submission was OK 
		//MmsServerState::getInstance()->reportMmsServerSucess();

		//if this is being submitted from an alarm...
		if( 0 != strcmp("NOT_FROM_ALARM", jobRequest.alarmUID.in()) )
		{
            try //try and set the state of the alarm to show that a JR has been raised on it
			{

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"next call is AlarmModificationHelper::setJobRequestSubmitted()");
				TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper().setJobRequestSubmitted(
					jobRequest.alarmUID.in(), jobRequest.locationKey); 
                // This interface is different from which is used in MMSSubmitterDatabase,
                // because that one's for the manually submit job request, but this is not the case.
			}
			catch(...)
			{
  				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"next call is AlarmModificationHelper::setJobRequestSubmitted()");

				//set the state of the alarm to show that a JR has been raised on it
				std::string reason = "The Job Request submission was successful but the associated alarm could not be updated.";
				
				throw TA_Base_Bus::SubmissionDenied(reason.c_str(), false, TA_Base_Bus::SUBMISSIONDENIED_ALARM_ALREADY_RAISED);
			}
		}
		else //the Job Request was not from an alarm... nothing to do but log
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Job Request was not submitted via an alarm");
		}
		

		FUNCTION_EXIT;
	}

	bool MmsJobRequestSubmitter::alarmStateIsOk(const TA_Base_Bus::JobRequestDetailCorbaDef& jobRequest, 
		std::string& reason, TA_Base_Bus::ESubmissionDeniedError& eType)
	{
		FUNCTION_ENTRY("alarmStateIsOk");
		
		//get the alarm from the alarm agent
		std::string alarmUID = jobRequest.alarmUID.in();
		unsigned long alarmLocationKey = jobRequest.locationKey;
		TA_Base_Core::AlarmDetailCorbaDef_var alarm;

		std::string errorMessage = "NO_ERROR";
		TA_Base_Bus::EDependencyFailureError eErrorType;
		try //try and get the alarm from an alarm agent
		{
			alarm = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().getAlarmAtLocation(alarmUID, alarmLocationKey);
		}
		catch(TA_Base_Core::NoAlarmAgentException)
		{
			errorMessage = "The Alarm Agent could not be contacted";
			eErrorType = TA_Base_Bus::DEPENDENCYFAILURE_ALARM_AGENT_NO_CONTACTED;
		}
		catch(TA_Base_Core::AlarmAgentCorbaDef::AlarmException)
		{
			errorMessage = "The specified alarm could not be retrieved from the Alarm Agent";
			eErrorType = TA_Base_Bus::DEPENDENCYFAILURE_NO_SPECIFIED_ALARM;
		}
		catch(...)
		{
			errorMessage = "The Alarm Agent could not be contacted";
			eErrorType = TA_Base_Bus::DEPENDENCYFAILURE_ALARM_AGENT_NO_CONTACTED;
		}

		if("NO_ERROR" != errorMessage)
		{

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, errorMessage.c_str());
			throw TA_Base_Bus::DependencyFailure(errorMessage.c_str(),eErrorType);
		}

		//so now we have the alarm, so see if the state is OK
		if( TA_Base_Core::MMS_JR_SENT == alarm->mmsState)
		{
			reason = "A Job Request has already been raised on this alarm.";
			eType = TA_Base_Bus::SUBMISSIONDENIED_ALARM_ALREADY_RAISED;
			FUNCTION_EXIT;
			return false;
		}

		// marvin++
		// bug 640 (TD10258)
        if(TA_Base_Core::AlarmClosed == alarm->state)
		{
			reason = "The alarm is normalized.";
			eType = TA_Base_Bus::SUBMISSIONDENIED_ALARM_NORMALIZED;
			FUNCTION_EXIT;
			return false;
		}

        if(TA_Base_Core::AlarmSuppressed == alarm->state)
		{
			reason = "The alarm is suppressed.";
			eType = TA_Base_Bus::SUBMISSIONDENIED_ALARM_SUPPRESSED;
			FUNCTION_EXIT;
			return false;
		}
		// ++marvin
		// bug 640 (TD10258)

		FUNCTION_EXIT;

		return true;
	}

 //end IMmsJobRequestSubmitterCorbaDef methods

    
	void MmsJobRequestSubmitter::initConfiguration(TA_Base_Core::MmsJobRequestSubmitterEntityDataPtr pData)
	{
		m_entityName = pData->getName(); 
	}

	void MmsJobRequestSubmitter::checkOperationMode()
	{
		TA_Base_Bus::GenericAgent::ensureControlMode();
	}


	std::string MmsJobRequestSubmitter::getOperatorDesc(const std::string& sessionID)
	{
		FUNCTION_ENTRY("getOperatorDesc");

		std::string name = "";

		try
		{
			TA_Base_Bus::AuthenticationLibrary lib;
			TA_Base_Bus::SessionInfo si = lib.getSessionInfo(sessionID, sessionID);
			std::string fullname = MmsMessageUtil::getInstance()->getOperatorName(si.UserId);
			name.assign(fullname, 0, 20);
		}
		catch(...)
		{
			name = "";
		}

		if(name.empty())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The operator details could not be retrieved.");
			throw TA_Base_Bus::DependencyFailure("The operator details could not be retrieved.",TA_Base_Bus::DEPENDENCYFAILURE_NO_OPERATOR_DETAIL);
		}

		FUNCTION_EXIT;
		return name;
	}


	//IEntity methods:

    bool MmsJobRequestSubmitter::isValid()
	{
		FUNCTION_ENTRY("isValid");


		FUNCTION_EXIT;

		//Can't think why this would ever not be valid
		return true;
	}


    void MmsJobRequestSubmitter::start()
	{
		FUNCTION_ENTRY("start");

		activateServantWithName(m_entityName);

		FUNCTION_EXIT;
	}


    void MmsJobRequestSubmitter::stop()
	{
		FUNCTION_ENTRY("stop");
		
		deactivateServant();

		FUNCTION_EXIT;
	}


    void MmsJobRequestSubmitter::update(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("update");

        //nothing to do. no data to update

		FUNCTION_EXIT;
	}


    void MmsJobRequestSubmitter::remove()
	{
		FUNCTION_ENTRY("remove");

        //does nothing
		FUNCTION_EXIT;
	}


    void MmsJobRequestSubmitter::setToControlMode()
	{
		FUNCTION_ENTRY("setToControlMode");

		//see class comments
		FUNCTION_EXIT;
	}


    void MmsJobRequestSubmitter::setToMonitorMode()
	{
		FUNCTION_ENTRY("setToMonitorMode");

		//see class comments
		FUNCTION_EXIT;
	}

	//end IEntity methods
} //end namespace TA_IRS_App



