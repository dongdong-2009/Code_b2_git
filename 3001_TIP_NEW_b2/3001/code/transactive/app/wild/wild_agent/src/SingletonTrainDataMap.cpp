/* 
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/wild/wild_agent/src/SingletonTrainDataMap.cpp $
 * @author:  <Andy Siow>
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * Implementation file
 */

#include "app/wild/wild_agent/src/SingletonTrainDataMap.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "app/wild/wild_agent/src/WILDConfiguration.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "ace/OS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


namespace TA_IRS_App
{
	SingletonTrainDataMap* SingletonTrainDataMap::m_instance = NULL;
	TA_Base_Core::NonReEntrantThreadLockable SingletonTrainDataMap::m_trainDataLock;
	SingletonTrainDataMap::SingletonTrainDataMap()
		:m_maxItems(0), m_isEmpty(true)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "SingletonTrainDataMap");

		try
		{
			m_maxItems = WILDConfiguration::getInstance()->getAgentATSWILDQueueSize();
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Maximum ATS Queue size is %d", m_maxItems);
		}
		catch(...)
		{

			// DO Nothing. We just want to construct the object and the above can 
			// throw an exception.
		}
		
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "SingletonTrainDataMap");
	}


	SingletonTrainDataMap::~SingletonTrainDataMap()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "~SingletonTrainDataMap");

		for(std::map<time_t, TrainID*>::iterator itr = m_trainDataMap.begin(); itr != m_trainDataMap.end(); itr++)
		{
			delete itr->second;
			itr->second = 0;
		}

		m_trainDataMap.clear();

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "SingletonTrainDataMap");
	}


	SingletonTrainDataMap* SingletonTrainDataMap::getInstance()
	{
		if ( 0 == m_instance )
		{
			// Double checking to prevent multiple threads
			// creating multiple instances.

			// Any lock is okay at this stage.
			TA_Base_Core::ThreadGuard guard( m_trainDataLock );

			if ( 0 == m_instance )
			{
				m_instance = new SingletonTrainDataMap();
    		}
		}

		return m_instance;		
	}


	void SingletonTrainDataMap::removeInstance()
	{
		// 
		// Guard this to prevent multiple threads atempting
		// to delete/create simultaneously
		//
		TA_Base_Core::ThreadGuard guard( m_trainDataLock );  // Any lock will do
		if ( m_instance != NULL )
		{
			delete m_instance;
			m_instance = NULL;
		}
	}


	void SingletonTrainDataMap::extractAndInsertIntoMap(const TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef& ATSTrainMessage)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "extractAndInsertIntoMap");

		// Note: The AtsWildMessageCorbaDef also contains direction information i.e. innerbound/outerbound but
		// it does not change or add any value to our correlation process and so we simply ignore it at this stage
		// We would only use it if the relative axle numbering returned by the WILD changes. Please refer
		// to the latest SICD doc to confirm.
		TA_Base_Core::ThreadGuard guard( m_trainDataLock );

		TrainID* train = new TrainID();
		train->ATCCarNumber = ATSTrainMessage.activeCabNumber;
		train->PVNumber = ATSTrainMessage.physicalTrainNumber;
		train->IsRevenue = ATSTrainMessage.isRevenue;
		time_t time = ATSTrainMessage.timeStamp;

		if(m_maxItems < 1)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
				"ATS Queue size is less than 1! Defaulting to 10..");
			m_maxItems = 10;
		}

		if(m_trainDataMap.size() < m_maxItems)
		{
            const unsigned int bufLen = 27;
            char buf[bufLen];
			m_trainDataMap[time] = train;
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Size of train map is now %d", m_trainDataMap.size());
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Adding train with ATC Car No: %d to ATS Queue for correlation. Timestamp: %s", train->ATCCarNumber, ACE_OS::ctime_r(&time, buf, bufLen));

			for(std::map<time_t, SingletonTrainDataMap::TrainID*>::iterator mapItr = m_trainDataMap.begin(); mapItr != m_trainDataMap.end(); mapItr++)
			{
                const unsigned int bufLen = 27;
                char buf[bufLen];
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "m_trainDataMap[%s] = %d", ACE_OS::ctime_r(&(mapItr->first), buf, bufLen), (mapItr->second)->ATCCarNumber);
			}
			
			m_isEmpty = false;
		}
		else // Delete the oldest item, and add the latest one in
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "ATS Queue size has reached max limit. Oldest train information will be discarded.");
			std::map<time_t, TrainID*>::iterator firstItem = m_trainDataMap.begin();
			
            const unsigned int bufLen = 27;
            char buf[bufLen];

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "Removing train %d of timestamp %s from the queue", (firstItem->second)->ATCCarNumber, ACE_OS::ctime_r(&time, buf, bufLen));
			delete firstItem->second;
			firstItem->second = 0;
			m_trainDataMap.erase(firstItem->first);

            char buf2[bufLen];
            
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Now adding train %d with time %s to queue.", train->ATCCarNumber, ACE_OS::ctime_r(&time, buf2, bufLen));
			m_trainDataMap[time] = train;
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "extractAndInsertIntoMap");
	}


	TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef* SingletonTrainDataMap::retrieveTrainInformation(unsigned long WILDTimestamp)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "retrieveTrainInformation");

		TA_Base_Core::ThreadGuard guard( m_trainDataLock );

		TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef* currentTrainInfo = 0;
		long longedTime = static_cast<long>(WILDTimestamp);
	
		for(std::map<time_t, SingletonTrainDataMap::TrainID*>::iterator mapItr = m_trainDataMap.begin(); mapItr != m_trainDataMap.end(); mapItr++)
		{
            const unsigned int bufLen = 27;
            char buf[bufLen];
			// If the difference in timestamps is within the correlation threshold, then a correlation exists.	
			time_t timeTmp = longedTime;
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "Timestamp from WILD: %s", ACE_OS::ctime_r(&timeTmp, buf, bufLen));
            char buf2[bufLen];
            LOGMORE(SourceInfo, "Timestamp from ATS: %s", ACE_OS::ctime_r(&(mapItr->first), buf2, bufLen) );

			if(static_cast<unsigned int>(abs((int)(mapItr->first - WILDTimestamp))) <= WILDConfiguration::getInstance()->getCorrelationThreshold())
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugNorm, "Correlation exists for the given timestamp and threshold value!");
				LOGMORE(SourceInfo, "Proceeding with Correlation for Train %d", (mapItr->second)->ATCCarNumber);

				currentTrainInfo = new TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef();
				if(currentTrainInfo == NULL)
				{
					TA_THROW(TA_Base_Core::TransactiveException("Unable to allocate memory for AtsWildMessageCorbaDef"));
				}

				currentTrainInfo->physicalTrainNumber = (mapItr->second)->PVNumber;
				currentTrainInfo->activeCabNumber = (mapItr->second)->ATCCarNumber;
				currentTrainInfo->timeStamp = WILDTimestamp;
				currentTrainInfo->isRevenue = (mapItr->second)->IsRevenue;
				
                const unsigned int bufLen = 27;
                char buf[bufLen];
				// We no longer need the train data in the map, so remove entry from the queue.
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "Removing train %d of timestamp %s from the queue", (mapItr->second)->ATCCarNumber, ACE_OS::ctime_r(&(mapItr->first), buf, bufLen));
				delete (mapItr->second);
				mapItr->second = 0;
				m_trainDataMap.erase(mapItr->first);

				if(m_trainDataMap.size() == 0)
				{
					m_isEmpty = true;
				}
				
				// return immediately as there can only be one match for a given timestamp
				LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "retrieveTrainInformation");
				return currentTrainInfo;
			}
		}		
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "retrieveTrainInformation");
		return currentTrainInfo;	
	}
}
