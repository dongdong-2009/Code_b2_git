// ATSAlarmObserver.cpp: implementation of the ATSAlarmObserver class.
//
//////////////////////////////////////////////////////////////////////

#include "core/utilities/src/DebugUtil.h"
 

#include "ATSAlarmObserver.h"
#include "SharedAlarmAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DataException.h"

#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreReceiverISCSProfile.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreReceiverATSAlarmInfo.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreUpdaterATSProfile.h"
#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreReceiverATSProfile.h"

using namespace I_ATS_ISCS_ALARM;
using TA_Base_Core::DebugUtil;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus
{

	ATSAlarmObserver::ATSAlarmObserver(bool bExportEnable, const std::string& exportPath, unsigned long exportInterval)
		: m_exportMgr(0)
	{
		m_lastUpdateTime=0;		 
		m_Running=true;
		notifyStoreRestart();
		
		// Initialize locations map
		initLocationsMap();

		if (bExportEnable)
		{
			m_exportMgr = new AtsAlarmExportMgr(exportPath, exportInterval);
			m_exportMgr->startProcessing();
		}
	}

	ATSAlarmObserver::~ATSAlarmObserver()
	{
		if (0 != m_exportMgr)
		{
			m_exportMgr->stopProcessing();
			delete m_exportMgr;
			m_exportMgr = 0;
		}
		
		terminateAndWait();
	}

	void ATSAlarmObserver::notifyStoreRestart()
	{
		//send  update details information to ATS ( C952 )
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "checking profile");
		boost::shared_ptr<ATSProfileInfo>  returnProfile;		
		returnProfile=TA_Base_Bus::DataStoreReceiverATSProfile::getInstance()->getProfile();
		if (returnProfile->operatorId != "")
		{			
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "update restart time");
			returnProfile->updateTime=time(NULL);
			std::vector<boost::shared_ptr<ATSProfileInfo> > ATSbatch;
			ATSbatch.push_back(returnProfile);
			TA_Base_Bus::DataStoreUpdaterATSProfile::getInstance()->onDispatchObjects(ATSbatch);	
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "don't have profile info");
		}
	}

	void ATSAlarmObserver::run()
	{
		while (m_Running)
		{
			//LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"ATS Receive data-begin");		
			boost::shared_ptr<ISCSProfileInfo>  returnProfile;		
			std::vector< boost::shared_ptr<ATSAlarmQueueItem> >* cachedatas=NULL;

			{	
				TA_Base_Core::ProcessGuard  guard1(*(DataStoreReceiverATSAlarmInfo::getInstance()->getlock()));
				TA_Base_Core::ProcessGuard  guard2(*(DataStoreReceiverISCSProfile::getInstance()->getlock()));
				returnProfile=DataStoreReceiverISCSProfile::getInstance()->getProfileWithoutLock();
				cachedatas=DataStoreReceiverATSAlarmInfo::getInstance()->getDataWithoutLock();
			}

			//if (returnProfile->updateTime!=m_lastUpdateTime && (!returnProfile->loginLocation.empty()))
			if (returnProfile->updateTime!=m_lastUpdateTime)
			{
				//ats restart, so we clean all ats alarm;
				m_lastUpdateTime=returnProfile->updateTime;
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"removeATSAlarms %d %s" ,m_lastUpdateTime, returnProfile->profileId.c_str());
				SharedAlarmAccessFactory::getInstance().removeATSAlarms();
			}		

            if (cachedatas !=NULL)
			{
				std::vector<boost::shared_ptr<ATSAlarmQueueItem> >::iterator iter=cachedatas->begin();
				std::vector<AlarmDetail*> alarms;		
				int l=0;
				for (;iter!= cachedatas->end();iter++)
				{
					l++;
					I_ATS_ISCS_ALARM::AlarmDetail* pDetail = NULL;
					boost::shared_ptr<ATSAlarmQueueItem> value=(*iter);
					pDetail = &(value->detail);
					// Convert string location to key location
 					unsigned long ulLocation = 0;
 					LocationMapT::iterator itLoc = m_locationMap.find(pDetail->locationId);
					if (itLoc != m_locationMap.end())
					{
						ulLocation = itLoc->second;
					}
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable find location key for location name: %s with AlarmID: %s", pDetail->locationId.c_str(), pDetail->alarmID.c_str());
					}

					pDetail->locationKey = ulLocation;
					alarms.push_back(pDetail);	
					if (l>=500)
					{
						SharedAlarmAccessFactory::getInstance().insertOrUpdateAlarm(alarms,true);
						alarms.clear();
						sleep(200);
						l=0;
					}
					if (0 != m_exportMgr)
						m_exportMgr->insert(value);

					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"[%d] ATS alarm  id=[%s] ",l ,value->detail.alarmID.c_str());					
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"desc=[%s]  state=[%d]",value->detail.alarmDescription.c_str(),value->detail.state);
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Ack by=[%s]  mmsState=[%d]",value->detail.alarmAcknowledgeBy.c_str(),value->detail.mmsState);
				}

				if (alarms.size()>0) 
				{
					SharedAlarmAccessFactory::getInstance().insertOrUpdateAlarm(alarms,true );
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"Receive ATS Alarm=%d", alarms.size() );
				}
				alarms.clear();		
				cachedatas->clear();
				delete cachedatas;
		        
			}
			if (m_Running)	sleep(150);
		}
	}

	void ATSAlarmObserver::terminate()
	{
		m_Running=false;	
	}

	void ATSAlarmObserver::initLocationsMap()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "initLocationsMap coming in");
		std::vector<TA_Base_Core::ILocation*> allLocationData;
		try
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Retrieving operator locations.");
			allLocationData = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"DatabaseException caught whilst retrieving operator locations. There were problems communicating with the database");
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve location information from database. Aborting."));
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"DataException caught whilst retrieving operator locations. The data could not be converted to the required format");
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve location information from database. Aborting."));
		}
		catch(...)
		{	
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Caught unknown exception whilst retrieving Operator locations.");
			TA_THROW(TA_Base_Core::TransactiveException("Unknown exception caught whilst retrieving location information."));
		}

		// store the locations into the map and delete the temporary structure
		m_locationMap.clear();
		for (unsigned int i = 0;i < allLocationData.size(); ++i)
		{
			if(allLocationData[i]->getKey() != 0)
			{
				m_locationMap.insert(LocationMapT::value_type(allLocationData[i]->getName(), allLocationData[i]->getKey()));

				delete allLocationData[i];
				allLocationData[i] = NULL;
			}
		}		

		allLocationData.clear();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "initLocationsMap coming out");
	}
}
