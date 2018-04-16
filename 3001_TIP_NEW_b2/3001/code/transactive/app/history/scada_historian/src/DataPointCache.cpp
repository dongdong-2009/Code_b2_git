/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/src/DataPointCache.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#include "app/history/scada_historian/src/DataPointCache.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


using TA_Core::DebugUtil;
using TA_Core::RunParams;


namespace TA_App
{
    //
    // Initialise statics
    //
    TA_Core::NonReEntrantThreadLockable DataPointCache::m_singletonLock;
    DataPointCache* DataPointCache::m_theCache = NULL;


    //
    // getInstance
    //
    DataPointCache& DataPointCache::getInstance()
    {
        //
        // If null guard against multiple threads
        //
	    TA_Core::ThreadGuard guard(m_singletonLock);

        //
		// If guard acquired then make sure the singleton is still NULL
        //
		if ( m_theCache == NULL )
		{
            //
			// Create the one & only object
            //
			m_theCache = new DataPointCache();
		}

	    return *m_theCache;

    }


    //
    // insert
    //
    void DataPointCache::insert(const DataPointData& dpData)
    {
        //
	    // Make it all thread safe - Don't want to allow retrieval from the cache
        // while we are trying to insert into the cache.
        //
	    TA_Core::ThreadGuard guard(*this);

	    unsigned long entityKey = dpData.dataPointId();

	    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
            "DataPointCache::insert(): key = %lu", entityKey);

	    //// Debug
	    std::string val = dpData.getValueAsString();
        unsigned long status = dpData.dataPointStatus();
        LOGMORE(SourceInfo, "value = %s", val.c_str());
        LOGMORE(SourceInfo, "status = %lu", status);
	    if(dpData.dataCollectionOn())
        {
            LOGMORE(SourceInfo, "DC = ON");
        }
	    else
        {
		    LOGMORE(SourceInfo, "DC = OFF");
        }
        if(dpData.getError())
        {
            LOGMORE(SourceInfo, "CommsError = TRUE");
        }
        else
        {
            LOGMORE(SourceInfo, "CommsError = FALSE");
        }
	    ////////////

        //
	    // Check if the datapoint is already in the cache
        //
	    DPDataMap::iterator dmIter = m_dpDataMap.find(entityKey);
	    if(dmIter != m_dpDataMap.end())
	    {
            //
            // Because the datapoint is already in the cache, we would normally do
            // nothing here. The only occasion that the existing datapoint details
            // would be overwritten is if the error flag has changed.
            //
            if(dmIter->second.getError() != dpData.getError())
            {
                m_dpDataMap.erase(dmIter);
                m_dpDataMap[entityKey] = dpData;

                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                    "A data point with this key has already been cached: %lu. "
                    "The error flag has changed so the original was overwritten",
                    entityKey);
            }
            else
            {
                //
		        // Data Point already exists in the cache
                //
		        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                    "A data point with this key has already been cached: %lu",
                    entityKey);
            }
	    }
	    else
	    {
		    m_dpDataMap[entityKey] = dpData;
        }
    }


    //
    // updateDataPointCache
    //
    void DataPointCache::updateDataPointCache(const DataPointData& updatedDataPoint)
    {
        //
	    // Make it all thread safe
        //
	    TA_Core::ThreadGuard guard(*this);

	    unsigned long entityKey = updatedDataPoint.dataPointId();

        //
	    // If the data point is not in cache, then don't worry about it
        //
	    DPDataMap::iterator dmIter = m_dpDataMap.find(entityKey);
	    if(dmIter == m_dpDataMap.end())
	    {
            //
		    // Data Point is not in cache, no need to update
            //
		    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Datapoint not found in cache");
		    return;
	    }
	    else
	    {
		    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Updating DataPointCache: Pkey: %lu", entityKey);
            LOGMORE(SourceInfo, "Status: %lu", updatedDataPoint.dataPointStatus());

            if(updatedDataPoint.dataPointValue()._d() == TA_Bus::DataPointCorbaDef::dpvText)
		    {
                std::string tempStr("");
                TA_Bus::DataPointCorbaDef::ByteSequence seq = updatedDataPoint.dataPointValue().stringValue();
                for(unsigned int i = 0; i < seq.length(); i++)
                {
                    tempStr += (char) seq[i];
                }

                LOGMORE(SourceInfo, "Value: %s", tempStr.c_str());
		    }
		    else
		    {
			    LOGMORE(SourceInfo, "Value: %f", updatedDataPoint.dataPointValue().floatValue());
		    }

            //
		    // Update data point in cache
            //
		    dmIter->second.dataPointStatus(updatedDataPoint.dataPointStatus());
		    dmIter->second.dataPointValue(updatedDataPoint.dataPointValue());
		    dmIter->second.dataTimeStamp(updatedDataPoint.dataTimeStamp());

            dmIter->second.setError(updatedDataPoint.getError());

		    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "New value is: %s",
                dmIter->second.getValueAsString().c_str());
	    }
    }


    //
    // dataCollectionOn
    //
    bool DataPointCache::dataCollectionOn(unsigned long dataPointId)
    //throw(DataPointNoExistException)
    {
	    DPDataMap::iterator dmIter = m_dpDataMap.find(dataPointId);
	    if(dmIter == m_dpDataMap.end())
	    {
            //
		    // Datapoint not found in cache
            //
		    char buffer[256];
		    sprintf(buffer, "%lu", dataPointId);
		    std::string dpId = buffer;
		    std::string error("Datapoint with this key not found in cache: ");
		    error += dpId;

		    throw DataPointNoExistException(error);
	    }
	    else
	    {
		    return dmIter->second.dataCollectionOn();
	    }
    }


    //
    // dataCollectionOn
    //
    void DataPointCache::dataCollectionOn(unsigned long dataPointId, bool collectOn)
    {
        //
	    // Make it thread safe
        //
	    TA_Core::ThreadGuard guard(*this);

	    DPDataMap::iterator dmIter = m_dpDataMap.find(dataPointId);
	    if(dmIter != m_dpDataMap.end())
	    {
		    (dmIter->second).dataCollectionOn(collectOn);
	    }

        //
	    // else if the data point cannot be found, don't worry about it.
        //
    }


    //
    // dataPointExists
    //
    bool DataPointCache::dataPointExists(unsigned long dataPointId)
    {
	    DPDataMap::iterator dmIter = m_dpDataMap.find(dataPointId);
	    if(dmIter != m_dpDataMap.end())
        {
		    return true;
        }
	    else
        {
		    return false;
        }
    }


    //
    // getAll
    //
    void DataPointCache::getAll(std::vector<DataPointData>& dataPointDataVector, bool dataCollectOnOnly)
    {
        //
	    // Make it all thread safe - don't want to allow insertion into the cache
        // while we are trying to retrieve all the datapoint that are currently
        // in the cache.
        //
	    TA_Core::ThreadGuard guard(*this);

        dataPointDataVector.clear();

	    DPDataMap::iterator dmIter;
	    for(dmIter = m_dpDataMap.begin(); dmIter != m_dpDataMap.end(); ++dmIter)
	    {
		    if(dataCollectOnOnly)
		    {
			    if(dmIter->second.dataCollectionOn())
                {
				    dataPointDataVector.push_back(dmIter->second);
                }
		    }
		    else
		    {
			    dataPointDataVector.push_back(dmIter->second);
		    }
	    }
    }


    //
    // getCacheSize
    //
    int DataPointCache::getCacheSize()
    {
        TA_Core::ThreadGuard guard(*this);

        return m_dpDataMap.size();
    }


    //
    // getCacheSize
    //
    void DataPointCache::remove(unsigned long dataPointId)
    {
        TA_Core::ThreadGuard guard(*this);

        DPDataMap::iterator dmIter = m_dpDataMap.find(dataPointId);
        if(dmIter != m_dpDataMap.end())
        {
            m_dpDataMap.erase(dmIter);
        }
    }

} // TA_App



