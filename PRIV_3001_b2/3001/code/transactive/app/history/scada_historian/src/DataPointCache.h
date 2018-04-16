#ifndef DATAPOINTCACHE_H
#define DATAPOINTCACHE_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/history/scada_historian/src/DataPointCache.h $
  * @author:   Anita Lee
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class represents a cache for which to store the datapoints. It 
  * is this cache from which the samples are taken from.
  *
  */

#include <vector>
#include <map>
#include <string>

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "app/history/scada_historian/src/DataPointData.h"


namespace TA_App
{
    class DataPointCache : public TA_Core::NonReEntrantThreadLockable
    {

    public:
	    class DataPointNoExistException;

	    /**
          * getInstance
          *
          * Get Instance method for this singleton.
		  *
		  */
	    static DataPointCache& getInstance();

	    /**
          * ~DataPointCache
          *
          * Destructor.
		  *
		  */
        ~DataPointCache() {}

        /**
          * insert
          *
          * Insert data into the cache.
		  *
          * @param dpData Object containing datapoint data
          *
		  */
	    void insert(const DataPointData& dpData);

	    /**
          * updateDataPointCache
          *
          * Update data in the cache.
		  *
          * @param updatedDataPoint Object containing datapoint data
          *
		  */
	    void updateDataPointCache(const DataPointData& updatedDataPoint);

	    /**
          * dataPointExists
          *
          * Check if a datapoint exists in the cache.
		  *
          * @param dataPointId Pkey of datapoint
          *
		  */
	    bool dataPointExists(unsigned long dataPointId);

	    /**
          * dataCollectionOn
          *
          * Check if a particular datapoint has trending enabled.
		  *
          * @param dataPointId Pkey of datapoint
          *
          * @return TRUE trending is enabled, FALSE trending is disabled
          *
		  */
	    bool dataCollectionOn(unsigned long dataPointId);
            //throw DataPointCache::DataPointNoExistException;

	    /**
          * dataCollectionOn
          *
          * Set trending status for a particular data point.
		  *
          * @param dataPointId Object containing datapoint data
          * @param collectOn TRUE trending enabled, FALSE trending disabled
          *
		  */
	    void dataCollectionOn(unsigned long dataPointId, bool collectOn);

	    /**
          * getAll
          *
          * Get all of the datapoints in the cache.
		  *
          * @param dataPointDataVector INOUT parameter to store the retrieved datapoints
          * @param dataCollectOnOnly TRUE only retrieve datapoint with trending enabled
          *                          FALSE retrieve all datapoints
          *                          Default: FALSE
          *
		  */
	    void getAll(std::vector<DataPointData>& dataPointDataVector, bool dataCollectOnOnly = false);

	    /**
          * getCacheSize
          *
          * Get the number of datapoints in the cache.
		  *
          * @return Size of cache
          *
		  */
	    int getCacheSize();
	    
        /**
          * remove
          *
          * Delete a datapoint from the cache
		  *
          * @param Pkey of datapoint
          *
		  */
        void remove(unsigned long dataPointId);

    private:

        static DataPointCache* m_theCache;

	    static TA_Core::NonReEntrantThreadLockable m_singletonLock;

	    typedef std::map<long, DataPointData> DPDataMap;
	    DPDataMap m_dpDataMap;

        DataPointCache() {}

        // TODO: Put this in separate class
    public:

	    class DataPointNoExistException
	    {
	    public:

		    DataPointNoExistException(const std::string& description)
			    : m_description(description) {}

		    ~DataPointNoExistException() {}

		    std::string Description() { return m_description; }
	    
	    private:

		    std::string m_description;
	    };

    };

} // TA_App

#endif // DATAPOINTCACHE_H
