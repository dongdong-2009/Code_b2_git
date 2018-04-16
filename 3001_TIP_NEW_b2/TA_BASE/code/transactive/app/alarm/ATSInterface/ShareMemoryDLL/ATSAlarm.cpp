// ATSAlarm.cpp: implementation of the CATSAlarm class.
//
//////////////////////////////////////////////////////////////////////
 
#include "stdafx.h"
#include "boost/shared_ptr.hpp"
#include "ATSAlarm.h"
 
#include "ISCSAlarmQueueItem.h"
#include "ATSAckQueueItem.h"
#include "ATSMmsQueueItem.h"
#include "ShareMemoryDLL.h"

#include "DataStoreReceiverATSProfile.h"

#include "DataStoreReceiverAlarmACKCommand.h"
#include "DataStoreReceiverAlarmMMSJobCommand.h"
#include "DataStoreUpdaterATSAlarmInfo.h"
#include "DataStoreUpdaterISCSProfile.h"

//#include "ace/Log_Msg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CATSAlarm::CATSAlarm():m_logger(NULL)
{	 
    m_logger = Logger::getLogger("root");	 
}

CATSAlarm::~CATSAlarm()
{
}

bool CATSAlarm::getProfileInfo(ProfileInfo& profileInfo)
{
	boost::shared_ptr<ATSProfileInfo> atsProfile;	
	atsProfile=TA_Base_Bus::DataStoreReceiverATSProfile::getInstance()->getProfile();
	profileInfo.loginLocation=atsProfile->loginLocation;
	profileInfo.operatorId=atsProfile->operatorId;
	profileInfo.updateTime=atsProfile->updateTime;
	profileInfo.profileId=atsProfile->profileId;
	profileInfo.displayOnly=atsProfile->displayOnly;
	return true;
}

unsigned long CATSAlarm::retrieveATSAlarmAckUpdate(std::list<AlarmAckUpdate*>& updates)
{	  
	std::vector<boost::shared_ptr<AlarmACKCommand> >* vec;
	vec=TA_Base_Bus::DataStoreReceiverAlarmACKCommand::getInstance()->getData();	 
	unsigned long nSize = 0;	 
	std::vector<boost::shared_ptr<AlarmACKCommand> >::iterator iter=vec->begin();
	for (;iter!=vec->end();iter++)
	{
		AlarmAckUpdate *pAlarmAckUpdate = new AlarmAckUpdate();		
		pAlarmAckUpdate->alarmId = (*iter)->alarmId.c_str();
		pAlarmAckUpdate->alarmAcknowledgeBy =  (*iter)->alarmAcknowledgeBy.c_str();
		pAlarmAckUpdate->ackTime =  (*iter)->ackTime;
        updates.push_back(pAlarmAckUpdate);	
		std::ostringstream ss;
		ss<<pAlarmAckUpdate->ackTime;             
		std::string  logInfo=ss.str();
		logInfo="retrieveATSAlarmAckUpdate ID=[" +  pAlarmAckUpdate->alarmId + "] by=["  + pAlarmAckUpdate->alarmAcknowledgeBy + "] time=[" + logInfo + "]";
		LOG4CXX_DEBUG(m_logger, logInfo);	 
		++nSize;
	}
	if(vec !=NULL)
	{
		vec->clear();
		delete vec;
	}
	 
	return nSize;
}

unsigned long CATSAlarm::retrieveATSMmsJobRequest(std::list<MmsJobRequest*>& updates)
{
	 
	std::vector< boost::shared_ptr<AlarmMmsJobCommand> >* vec;
	vec=TA_Base_Bus::DataStoreReceiverAlarmMMSJobCommand::getInstance()->getData();
	 
	unsigned long nSize = 0;
 
	std::vector<boost::shared_ptr<AlarmMmsJobCommand> >::iterator iter=vec->begin();
	for (;iter!=vec->end();iter++)
	{
		MmsJobRequest *pMmsJobRequest = new MmsJobRequest();		
		pMmsJobRequest->AlarmId=(*iter)->alarmId.c_str();
		pMmsJobRequest->AssetName=(*iter)->assetName.c_str();
		pMmsJobRequest->OperatorId=(*iter)->operatorId.c_str();
		pMmsJobRequest->problemReport=(*iter)->problemReport.c_str();
		pMmsJobRequest->targetDate=(*iter)->targetDate;	 
		updates.push_back(pMmsJobRequest);	


		std::ostringstream ss;
		 ;             
		std::string  logInfo=ss.str();
		logInfo="retrieveATSMmsJobRequest ID=[" +  pMmsJobRequest->AlarmId + "] by=["  + pMmsJobRequest->OperatorId + "]" ;		 
		LOG4CXX_DEBUG(m_logger, logInfo);
		//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%I %D, %M, [%t], %l retrieveATSMmsJobRequest ID=%s \n"), pMmsJobRequest->AlarmId.c_str()));
		++nSize;
	}
	if(vec !=NULL)
	{
		vec->clear();
		delete vec;
	}
	return nSize;

}

void CATSAlarm::setUpdateTime(time_t updateTime)
{
	
	//clean all alarm in queue
	TA_Base_Bus::DataStoreUpdaterATSAlarmInfo::getInstance()->clearAllData();

	//update update time
	std::vector<boost::shared_ptr<ISCSProfileInfo> > batch;	
	boost::shared_ptr<ISCSProfileInfo> profile (new ISCSProfileInfo() );
	profile->loginLocation="ATS";
	profile->operatorId="ATS";
	profile->profileId="ATS";
	profile->updateTime=updateTime;   
	std::ostringstream ss;
	ss<<updateTime;             
	std::string  logInfo=ss.str();
	logInfo="setUpdateTime" + logInfo;
	LOG4CXX_DEBUG(m_logger, logInfo);
	//ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%I %D, %M, [%t], %l setUpdateTime=%d \n"), updateTime));
	 
	batch.push_back(profile);
	TA_Base_Bus::DataStoreUpdaterISCSProfile::getInstance()->onDispatchObjects(batch);	 

}

void CATSAlarm::submitATSAlarm(const AlarmDetail& details, const ProfileInfo& profileInfo) //May catch ATS_ALARM_QUEUE_OVERFLOW exception
{
	 
	std::vector<boost::shared_ptr<ATSAlarmQueueItem> > batch;

    boost::shared_ptr<ATSAlarmQueueItem> sharedobj (new ATSAlarmQueueItem());

    sharedobj->detail=details;
	batch.push_back(sharedobj);


	std::ostringstream ss;
	ss<<" state=["<<details.state<<"] acktime=["<<details.ackTime<<"]" ;         
	std::string  logInfo=ss.str();	 
	logInfo="submitATSAlarm  id=[" + details.alarmID + "]  desc=[" + details.alarmDescription  + "] value=[" + details.alarmValue + "]  ackby=[" + details.alarmAcknowledgeBy + "]" + logInfo;
	LOG4CXX_DEBUG(m_logger, logInfo);

 	TA_Base_Bus::DataStoreUpdaterATSAlarmInfo::getInstance()->onDispatchObjects(batch);

 
}

void CATSAlarm::submitATSAlarmList(std::list<AlarmDetail*> details, const ProfileInfo& profileInfo) //May catch ATS_ALARM_QUEUE_OVERFLOW exception
{
	std::vector<boost::shared_ptr<ATSAlarmQueueItem> > batch;	

	std::list<AlarmDetail*>::iterator iter = details.begin();

	for (; iter != details.end(); ++iter)
	{
		boost::shared_ptr<ATSAlarmQueueItem> sharedobj (new ATSAlarmQueueItem());
		AlarmDetail* detail=(*iter);
		sharedobj->detail=(*detail);
		sharedobj->profileInfo=profileInfo;
		batch.push_back(sharedobj);
		std::ostringstream ss;
		ss<<" state=["<<detail->state<<"] acktime=["<<detail->ackTime<<"]" ;          
		std::string  logInfo=ss.str();
		 logInfo="submitATSAlarm  id=[" + detail->alarmID + "]  desc=[" + detail->alarmDescription  + "] value=[" + detail->alarmValue + "]  ackby=[" + detail->alarmAcknowledgeBy + "]" + logInfo;
		LOG4CXX_DEBUG(m_logger, logInfo);
	}
	std::string  logInfo;
	
    //ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%I %D, %M, [%t], %l submitATSAlarmList=%d \n"), details.size()));
	TA_Base_Bus::DataStoreUpdaterATSAlarmInfo::getInstance()->onDispatchObjects(batch);
 
}