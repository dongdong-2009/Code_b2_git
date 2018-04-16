// ISCSAlarm.cpp: implementation of the CISCSAlarm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ISCSAlarm.h"
 
//#include "ShmemQueue.h"
#include "ISCSAlarmQueueItem.h"
#include "ATSAckQueueItem.h"
#include "ATSMmsQueueItem.h"
#include "ShareMemoryDLL.h"
// #include "ace\ACE.h"
// #include "ace\Unbounded_Queue.h"

#include "DataStoreUpdaterATSProfile.h"
#include "DataStoreReceiverISCSProfile.h"
#include "DataStoreReceiverATSAlarmInfo.h"
#include "DataStoreUpdaterACKCommand.h"
#include "DataStoreUpdaterMMSJobCommand.h"
 
 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CISCSAlarm::CISCSAlarm()
{
}

CISCSAlarm::~CISCSAlarm()
{

}

unsigned long CISCSAlarm::retrieveATSAlarm( std::list<ATSAlarmQueueItem*>& details )
{
  
	std::vector<boost::shared_ptr<ATSAlarmQueueItem> >* vec;
	vec=TA_Base_Bus::DataStoreReceiverATSAlarmInfo::getInstance()->getData();

	if (vec==NULL)
	{
		return 0;
	}

	std::vector<boost::shared_ptr<ATSAlarmQueueItem> >::iterator iter=vec->begin();
	for (;iter!=vec->end();iter++)
	{
		ATSAlarmQueueItem *pAtsAlarmQueueItem = new ATSAlarmQueueItem();
        pAtsAlarmQueueItem->detail.sourceTime = (*iter)->detail.sourceTime;
		pAtsAlarmQueueItem->detail.alarmID = (*iter)->detail.alarmID.c_str();
		pAtsAlarmQueueItem->detail.ackTime = (*iter)->detail.ackTime;
		pAtsAlarmQueueItem->detail.closeTime = (*iter)->detail.closeTime;
		pAtsAlarmQueueItem->detail.assetName = (*iter)->detail.assetName.c_str();
		pAtsAlarmQueueItem->detail.alarmSeverity = (*iter)->detail.alarmSeverity;
		pAtsAlarmQueueItem->detail.alarmDescription = (*iter)->detail.alarmDescription.c_str();
		pAtsAlarmQueueItem->detail.alarmAcknowledgeBy = (*iter)->detail.alarmAcknowledgeBy.c_str();
		pAtsAlarmQueueItem->detail.state = (*iter)->detail.state;
		pAtsAlarmQueueItem->detail.locationId = (*iter)->detail.locationId.c_str() ;
		//std::string profileLocaiton=(*iter)->detail.m_loginLocation;
		//pAtsAlarmQueueItem->detail.locationId =pAtsAlarmQueueItem->detail.locationId ;// + " " +  profileLocaiton;	 
	    pAtsAlarmQueueItem->detail.parentAlarmID = (char*)(*iter)->detail.parentAlarmID.c_str();	 
		pAtsAlarmQueueItem->detail.avalancheHeadID = (char*)(*iter)->detail.avalancheHeadID.c_str();		 
		pAtsAlarmQueueItem->detail.isHeadOfAvalanche = (*iter)->detail.isHeadOfAvalanche;
		pAtsAlarmQueueItem->detail.mmsState = (*iter)->detail.mmsState;
		pAtsAlarmQueueItem->detail.alarmValue = (*iter)->detail.alarmValue.c_str();
		pAtsAlarmQueueItem->detail.omAlarm = (*iter)->detail.omAlarm;

        pAtsAlarmQueueItem->detail.alarmType=(char*)(*iter)->detail.alarmType.c_str();
		pAtsAlarmQueueItem->detail.systemkey=(char*)(*iter)->detail.systemkey.c_str();
		pAtsAlarmQueueItem->detail.subsytemkey=(char*)(*iter)->detail.subsytemkey.c_str();

		pAtsAlarmQueueItem->profileInfo.loginLocation="";
		pAtsAlarmQueueItem->profileInfo.operatorId="";
		pAtsAlarmQueueItem->profileInfo.profileId="";
 	
		details.push_back(pAtsAlarmQueueItem);
	} 
	delete vec;
	boost::shared_ptr<ISCSProfileInfo> profile;
	profile=TA_Base_Bus::DataStoreReceiverISCSProfile::getInstance()->getProfile();	 
	return profile->updateTime;
}

void CISCSAlarm::setProfileInfo(const ProfileInfo& profileInfo)
{
	std::vector<boost::shared_ptr<ATSProfileInfo> > batch;	
	boost::shared_ptr<ATSProfileInfo> atsProfile (new ATSProfileInfo() );
	atsProfile->loginLocation=profileInfo.loginLocation;
	atsProfile->operatorId=profileInfo.operatorId;
	atsProfile->profileId=profileInfo.profileId;
	atsProfile->updateTime=profileInfo.updateTime;
	atsProfile->displayOnly=profileInfo.displayOnly;
	//TA_Base_Bus::DataStoreUpdaterATSProfile::getInstance()->Initialize(batch);
	batch.push_back(atsProfile);
	TA_Base_Bus::DataStoreUpdaterATSProfile::getInstance()->onDispatchObjects(batch);
}

void CISCSAlarm::submitATSAlarmAckUpdate( const AlarmAckUpdate& update )//May catch 
{ 
	std::vector<boost::shared_ptr<AlarmACKCommand> > batch;	
	boost::shared_ptr<AlarmACKCommand> ackCommand (new AlarmACKCommand() );

	ackCommand->ackTime=update.ackTime;
	ackCommand->alarmAcknowledgeBy=update.alarmAcknowledgeBy.c_str();
	ackCommand->alarmId=update.alarmId.c_str();
 
	//TA_Base_Bus::DataStoreUpdaterACKCommand::getInstance()->Initialize(batch);
	batch.push_back(ackCommand);
	TA_Base_Bus::DataStoreUpdaterACKCommand::getInstance()->onDispatchObjects(batch);
 
}

void CISCSAlarm::submitATSMmsJobRequest( const MmsJobRequest& update )
{
	std::vector<boost::shared_ptr<AlarmMmsJobCommand> > batch;	
	boost::shared_ptr<AlarmMmsJobCommand> mmsJobCommand (new AlarmMmsJobCommand() );

	mmsJobCommand->assetName=update.AssetName.c_str();
	mmsJobCommand->operatorId=update.OperatorId.c_str();
	mmsJobCommand->alarmId=update.AlarmId.c_str();
	mmsJobCommand->problemReport=update.problemReport.c_str();
	mmsJobCommand->targetDate=update.targetDate;
   	//TA_Base_Bus::DataStoreUpdaterMMSJobCommand::getInstance()->Initialize(batch);	 
	batch.push_back(mmsJobCommand);
	TA_Base_Bus::DataStoreUpdaterMMSJobCommand::getInstance()->onDispatchObjects(batch);
}
