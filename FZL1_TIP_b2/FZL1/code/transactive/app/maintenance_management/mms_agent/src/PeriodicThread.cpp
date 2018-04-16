
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/maintenance_management/mms_agent/src/PeriodicThread.cpp $
  * @author:  C. DeWolfe
  * @version: $Revision: #7 $
  *
  * Last modification: $DateTime: 2013/07/03 10:01:11 $
  * Last modified by:  $Author: huangjian $
  * 
  * Implimentation of PeriodicThread
  *
  */
#pragma warning(disable:4786)

#include <algorithm>
#include <sstream>
#include <time.h>
#include "ace/OS.h"
#include "core/utilities/src/TAAssert.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/MmsPeriodicEntityData.h"
#include "core/data_access_interface/entity_access/src/MmsAgentEntityData.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/MmsAlarm_MessageTypes.h"

#include "bus/maintenance_management/mmsHelper/src/MMSAPIManager.h"

#include "app/maintenance_management/mms_agent/src/MmsConnectionException.h"

//#include "bus/maintenance_management/mmsCorbaDef/idl/src/IMmsConnectionCorbaDef.h"
#include "app/maintenance_management/mms_agent/src/MmsConnection.h"
#include "app/maintenance_management/mms_agent/src/PeriodicThread.h"
//#include "app/maintenance_management/mms_agent/src/MmsApiWrapper.h"
#include "app/maintenance_management/mms_agent/src/MmsMessageUtil.h"
#include "app/maintenance_management/mms_agent/src/MmsServerState.h"
#include "app/maintenance_management/mms_agent/src/MmsAgent.h"


using  TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{

	const CORBA::Double PeriodicThread::BAD_READ_VALUE = -999999998.0;

	PeriodicThread::PeriodicThread()
		: m_scanPeriodMinutes(0), 
		   m_stopRequest(true) ,m_isControlMode(false)
	{
		FUNCTION_ENTRY("PeriodicThread");

		FUNCTION_EXIT;
	}


	PeriodicThread::~PeriodicThread()
	{
		FUNCTION_ENTRY("~PeriodicThread");
		 
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

        TA_Base_Core::ThreadGuard guard( m_dataLock);
		DPWorkerMap::iterator  iterWorkerMap=m_workerMap.begin();
		for (; iterWorkerMap!=m_workerMap.end(); iterWorkerMap++)
		{
			DPScadaWorker* worker=iterWorkerMap->second;
			worker->terminate();
		}
		
		iterWorkerMap = m_workerMap.begin();
		for (; iterWorkerMap!=m_workerMap.end(); iterWorkerMap++)
		{
			DPScadaWorker* worker=iterWorkerMap->second;
			worker->terminateAndWait();
			delete worker;
		}
		m_workerMap.clear();

		FUNCTION_EXIT;
	}

   
	void PeriodicThread::terminate()
	{
		FUNCTION_ENTRY("terminate");

		m_stopRequest = true;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"terminate( ) called");

		FUNCTION_EXIT;
	}

	void PeriodicThread::initConfiguration(TA_Base_Core::MmsPeriodicEntityDataPtr pData)
	{
		FUNCTION_ENTRY("initConfiguration");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"initConfiguration( ) called");

		m_periodicData = pData;
		//chenlei--
		//move this initialization to the beginning of the run() function
		//setBacklogPeriodHours( pData->getBackLogCheckPeriodHours() );
		//setScanPeriodMinutes( pData->getScanPeriodMinutes() );

		FUNCTION_EXIT;
	}

	//chenlei++
	//this function is used to initialize the configuration
	//using the entity data from the member
	void PeriodicThread::initConfiguration()
	{
		m_periodicData->invalidate();
		//setBacklogPeriodHours( m_periodicData->getBackLogCheckPeriodHours() );
		setScanPeriodMinutes( m_periodicData->getScanPeriodMinutes() );

		TA_Base_Core::ThreadGuard guard( m_dataLock);

		std::vector<TA_Base_Core::ILocation*>  vecLoc= TA_Base_Core::LocationAccessFactory::getInstance().getAllAgentLocations();
		std::vector<TA_Base_Core::ILocation*>::iterator iterLoc=vecLoc.begin();
		for (;iterLoc!=vecLoc.end() && m_stopRequest == false;iterLoc++)
		{
			TA_Base_Core::ILocation* loc=*iterLoc;
			std::vector<ScadaUtilities::MaintenanceData*> vecData= ScadaUtilities::getInstance().getMaintenanceDataObject(loc->getKey());
			unsigned long dataSize=vecData.size();
			if (dataSize>0 ) //&& loc->getKey()==5)   
			{		 
				DPScadaWorker* worker=new DPScadaWorker(loc->getKey(), this);		 
				m_workerMap.insert(std::pair<unsigned long, DPScadaWorker*>
					(loc->getKey(), worker));
				worker->start();

				std::vector<MaintenanceDataWrapper*> vecDataWrapper;
				std::vector<ScadaUtilities::MaintenanceData*>::iterator  iterVecData=vecData.begin();
				for (;iterVecData!=vecData.end();iterVecData++)
				{
					MaintenanceDataWrapper* dataWrapper=new MaintenanceDataWrapper();
					dataWrapper->setData(*iterVecData);
					dataWrapper->initSchedulingTime();
					vecDataWrapper.push_back(dataWrapper);

				}
				m_locationDataMap.insert(std::pair<unsigned long, std::vector<MaintenanceDataWrapper*> >
					(loc->getKey(), vecDataWrapper));
			}		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"get data from loc=%ld, size=%ld.",loc->getKey(), dataSize );
		}
	}


	void PeriodicThread::setScanPeriodMinutes(unsigned int minutes)
	{
		FUNCTION_ENTRY("setScanPeriodMinutes");

		m_scanPeriodMinutes = minutes;

		if(0 == m_scanPeriodMinutes)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"Scanning for due periodic messages is currently disabled.");
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Now scanning for due periodic messages every %u minutes.", minutes);
		}

		FUNCTION_EXIT;
	}


	void  PeriodicThread::dataNotify(unsigned long locationKey,std::vector<MaintenanceDataWrapper*>& vecData)
	{
		//If it is necessary, we can create a dedicated thread for saving database.
		std::vector<MaintenanceDataWrapper*>::iterator iterData=vecData.begin();
		for (;iterData!=vecData.end();iterData++)
		{
			ScadaUtilities::MaintenanceData* data=(*iterData)->getData();
			if (data->readFailed==false)  //read successfully
			{
				TA_Base_Bus::MMSScheduledMessage scheduleMsg;
				scheduleMsg.setEntityKey(data->entityKey);		     
				scheduleMsg.setAssetID(data->assetName);
				scheduleMsg.setMeterCode(data->pointName);
				scheduleMsg.setEquipmentDescription(data->dnDescription);
				scheduleMsg.setDataPointValue(data->value); 		 
				scheduleMsg.setPlanDate((*iterData)->getLatestPlanDate());
				if (data->reportType ==ScadaUtilities::MaintenanceData::CPT)
				{
					scheduleMsg.setMessageType(TA_Base_Bus::MMSMessageType::COUNTER);
				}
				else
				{
					scheduleMsg.setMessageType(TA_Base_Bus::MMSMessageType::MEASUREMENT);
				}
				TA_Base_Bus::MMSAPIManager::getInstance()->submitScheduledMessage(scheduleMsg);
			}
			//delete data;
		}
	}


	bool PeriodicThread::isScheduleOn(MaintenanceDataWrapper* data,timeb timeNow)
	{
		//check whether the data need to create maintenance data
		bool ret=false; 	 
		timeb  planTime=data->getNextPlanDate();
		//monitor mode delays 3 minutes to create data, so if monitor switch to control, it can get data again.
		//constraint of mms_request_scheduling provides function to avoid to create duplicated data.
		if (m_isControlMode==false) planTime.time=planTime.time+3*60; 
		if (planTime.time <= timeNow.time )  
		{
			ret=true;
			data->calcNextSchedulingTime();
		}
		return ret;
	}


	void PeriodicThread::checkScheduleData()
	{
		FUNCTION_ENTRY("checkScheduleData");
		timeb taTime;
		ftime( &taTime );
		TA_Base_Core::ThreadGuard guard( m_dataLock);
		LocationDataMap::iterator iterDataMap=m_locationDataMap.begin();
		for (; iterDataMap!=m_locationDataMap.end();iterDataMap++)
		{
			 unsigned long locationKey=iterDataMap->first;
			 std::vector<MaintenanceDataWrapper*> vecDueData;
			 std::vector<MaintenanceDataWrapper*>& vecData=iterDataMap->second;
			 std::vector<MaintenanceDataWrapper*>::iterator iterVecData=vecData.begin();

			 //find data which is need to get maintenance data
			 for (; iterVecData!=vecData.end();iterVecData++)
			 {
				 MaintenanceDataWrapper*  data=*iterVecData;
				 if (isScheduleOn(data,taTime)==true)  //need to update nextSchedulingTime on both control and monitor mode
				 {
					 if (m_isControlMode==true)   //only control mode need to push into worker;
					 {	 
						 MaintenanceDataWrapper* dataCopy=new MaintenanceDataWrapper(*data);
						 vecDueData.push_back(dataCopy);
					 }
				 }
			 }

			 //put into data point scada worker
			 if (vecDueData.size()>0)
			 {
				 DPWorkerMap::iterator  findItrWorker=m_workerMap.find(locationKey);
				 if (findItrWorker!=m_workerMap.end())
				 {										
					 findItrWorker->second->push(vecDueData);
				 }
				 else
				 {
					 LOG1(SourceInfo, DebugUtil::DebugError,"Can not find worker for location =%ld", locationKey );

					 std::vector<MaintenanceDataWrapper*>::iterator iterVecDueData=vecDueData.begin();
					 for (; iterVecDueData!=vecDueData.end();iterVecDueData++)
					 {
						 delete (*iterVecDueData);
					 }					 
				 }
				 vecDueData.clear();
			 }		 

		}		
		FUNCTION_EXIT;
	}


	void PeriodicThread::run()
	{
		FUNCTION_ENTRY("sendBackLog");
		m_stopRequest = false;

		//chenlei++
		//initiliazing the configuration
		initConfiguration();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"PeriodicThread::run() called");

		//so that other components can init before this thing
		//goes about it's business...
		sleep(1000);

		while(m_stopRequest == false)
		{
            try
            {
			    //note that 0 value disables periodic messages
			    if(m_scanPeriodMinutes != 0)
			    {
				    checkScheduleData();			
			    }
			    else
			    {
				    sleep(2000);
				    continue;
			    }
            }
            catch (TA_Base_Core::TransactiveException& e)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, e.what());
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unknown exception");
            }

			//have a nap for the time remaining
			time_t lastScan;
			time(&lastScan);
			time_t now;
			time(&now);
			while( ( (now - lastScan) / 60 ) < static_cast<long>(m_scanPeriodMinutes) )
			{
				if(m_stopRequest == true) break;						
				sleep(2000);
				time(&now);
			}			
		}
	}


    void PeriodicThread::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
		FUNCTION_ENTRY("processUpdate");
        //need to inform there are changes in the entity
        TA_Base_Bus::ScadaUtilities& scadaUtilities = TA_Base_Bus::ScadaUtilities::getInstance();
        scadaUtilities.processOnlineUpdateOfMaintenanceData();
		FUNCTION_EXIT;
    }


	void PeriodicThread::setToControlMode()
	{
		FUNCTION_ENTRY("setToControlMode");
		LOG0(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "setToControlMode");
		m_isControlMode=true; 
		FUNCTION_EXIT;
	}


	void PeriodicThread::setToMonitorMode()
	{
		FUNCTION_ENTRY("setToMonitorMode");
		LOG0(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "setToMonitorMode");
		m_isControlMode=false;
		FUNCTION_EXIT;
	}

}

