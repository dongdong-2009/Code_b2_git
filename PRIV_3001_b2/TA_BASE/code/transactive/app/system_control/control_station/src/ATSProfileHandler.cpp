
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/control_station/src/ATSProfileHandler.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #21 $
  *
  * Last modification: $DateTime: 2013/12/13 11:06:51 $
  * Last modified by:  $Author: haijun.li $
  * 
  */
#include "app\system_control\control_station\src\SessionManager.h"

#include "bus/alarm/ATS_alarm_bridge_library/src/DataStructInfo.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreUpdaterATSProfile.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreReceiverISCSProfile.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreUpdaterISCSProfile.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreUpdaterATSAlarmInfo.h"

#include "core/utilities/src/DebugUtil.h"

#include "ATSProfileHandler.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace I_ATS_ISCS_ALARM;


ATSProfileHandler::ATSProfileHandler()
{

}

ATSProfileHandler::~ATSProfileHandler()
{

}

void ATSProfileHandler::updateProfile()
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Clear ATS alarm queue");
	TA_Base_Bus::DataStoreUpdaterATSAlarmInfo::getInstance()->clearAllData();
	TA_Base_Bus::DataStoreUpdaterATSAlarmInfo::getInstance()->release();

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Clear ATS alarm cache begin");
	
	//clean up ATS alarm
	boost::shared_ptr<I_ATS_ISCS_ALARM::ISCSProfileInfo>  returnProfile;		
	returnProfile=TA_Base_Bus::DataStoreReceiverISCSProfile::getInstance()->getProfile();
	returnProfile->displayOnly = SessionManager::getInstance().getSessionDetails().getDisplayMode();
	returnProfile->updateTime=time(NULL);
	std::vector<boost::shared_ptr<I_ATS_ISCS_ALARM::ISCSProfileInfo> > ISCSbatch;
	ISCSbatch.push_back(returnProfile);
	TA_Base_Bus::DataStoreUpdaterISCSProfile::getInstance()->onDispatchObjects(ISCSbatch);	
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Clear ATS alarm cache end");


	//send  update details information to ATS ( C952 )
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Send profile to ATS operator  before");
	std::vector<boost::shared_ptr<ATSProfileInfo> > batch;	
	boost::shared_ptr<ATSProfileInfo> atsProfile (new ATSProfileInfo() );
	atsProfile->loginLocation=SessionManager::getInstance().getSessionDetails().getLocationName();
	atsProfile->operatorId=TA_Base_Core::toString(SessionManager::getInstance().getSessionDetails().getOperatorName());
	atsProfile->profileId=SessionManager::getInstance().getSessionDetails().getProfileName();
	atsProfile->updateTime=time(NULL);
	atsProfile->displayOnly = SessionManager::getInstance().getSessionDetails().getDisplayMode();


	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "updateProfile operatorId=%s, profile=%s, Location=%s  updateTime=%ld, displayOnly=%d", atsProfile->operatorId.c_str(),atsProfile->profileId.c_str(),atsProfile->loginLocation.c_str(),atsProfile->updateTime,atsProfile->displayOnly);


	//TA_Base_Bus::DataStoreUpdaterATSProfile::getInstance()->Initialize(batch);
	batch.push_back(atsProfile);
	TA_Base_Bus::DataStoreUpdaterATSProfile::getInstance()->onDispatchObjects(batch);
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Send profile to ATS operator  end");

}

void ATSProfileHandler::clearProfile()
{
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Clear ATS alarm queue");
	TA_Base_Bus::DataStoreUpdaterATSAlarmInfo::getInstance()->clearAllData();
	TA_Base_Bus::DataStoreUpdaterATSAlarmInfo::getInstance()->release();

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Clear ATS alarm cache begin");

	//clean up ATS alarm
	boost::shared_ptr<I_ATS_ISCS_ALARM::ISCSProfileInfo>  returnProfile;		
	returnProfile=TA_Base_Bus::DataStoreReceiverISCSProfile::getInstance()->getProfile();
	returnProfile->displayOnly = SessionManager::getInstance().getSessionDetails().getDisplayMode();
	returnProfile->updateTime=time(NULL);
	std::vector<boost::shared_ptr<I_ATS_ISCS_ALARM::ISCSProfileInfo> > ISCSbatch;
	ISCSbatch.push_back(returnProfile);
	TA_Base_Bus::DataStoreUpdaterISCSProfile::getInstance()->onDispatchObjects(ISCSbatch);	
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Clear ATS alarm cache end");


	//send  update details information to ATS ( C952 )
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Send profile to ATS operator  before");
	std::vector<boost::shared_ptr<ATSProfileInfo> > batch;	
	boost::shared_ptr<ATSProfileInfo> atsProfile (new ATSProfileInfo() );
	atsProfile->loginLocation="";
	atsProfile->operatorId="";
	atsProfile->profileId="";
	atsProfile->updateTime=time(NULL);
	atsProfile->displayOnly = true;


	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "clearProfile operatorId=%s, profile=%s, Location=%s  updateTime=%ld, displayOnly=%d", atsProfile->operatorId.c_str(),atsProfile->profileId.c_str(),atsProfile->loginLocation.c_str(),atsProfile->updateTime,atsProfile->displayOnly);


	//TA_Base_Bus::DataStoreUpdaterATSProfile::getInstance()->Initialize(batch);
	batch.push_back(atsProfile);
	TA_Base_Bus::DataStoreUpdaterATSProfile::getInstance()->onDispatchObjects(batch);
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Send profile to ATS operator  end");

}