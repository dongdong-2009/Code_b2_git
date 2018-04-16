#ifndef DATAPOINT_LOADER_THREAD_H
#define DATAPOINT_LOADER_THREAD_H

/**
  * DatapointLoaderThread.h
  *
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/scada_historian/src/DatapointLoaderThread.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class handles the datapoint initialisation for trending. It
  * loads the datapoints configured for trending from the database and
  * stores the required datapoint attributes in a cache for sampling.
  *
  */

#include <vector>
#include <map>
#include <string>
#include "core/threads/src/Thread.h"
#include "app/history/scada_historian/src/ScadaHistorianAgentProcess.h"
#include "bus/scada/DataPointCorbaDef/src/DataPointCorbaDef.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/SimpleDbData.h"
#include "core/naming/src/NamedObject.h"
#include "core/naming/src/INamedObject.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


static const char* DATAPOINT_TYPENAME = "DataPoint";
static const char* MAX_TREND_DPS_PER_STATION_CFG = "MaxTrendDatapointsPerStation";
static int DEFAULT_MAX_TREND_DPS_PER_STATION_CFG = 40;


using TA_Core::CorbaName;


namespace TA_App
{
    //
	// Forward declaration
	//
    class DataPointData;

    class DatapointLoaderThread : public TA_Core::Thread
    {

    public:

        typedef struct
        {
            std::string pKey;
            std::string name;
            std::string agentName;
        } DataPoints;

	    /**
		  * datapointLoaderThread
		  *
		  * Constructor.
		  *
		  * @exception TA_Core::HistoryInitialisationException
		  *			   Thrown if the datapoints could not be loaded.
		  *
		  */
	    DatapointLoaderThread(ScadaHistorianAgentProcess* parentProcess);

	    /**
		  * ~datapointLoaderThread
		  *
		  * Destructor.
		  *
		  */
	    virtual ~DatapointLoaderThread() {}

	    /**
		  * run
		  *
		  * Resolves the datapoints loaded from the database and stores
		  * the datapoints and their attributes in a cache. If datapoints
		  * cannot be resolved, resolution of those datapoints will be
		  * attempted for the lifecycle of this thread.
		  *
		  */
	    virtual void run();

	    /**
		  * terminate
		  *
		  * Terminates the thread.
		  *
		  */
	    virtual void terminate();

		/**
		  * loadStationNameAndKey
		  *
		  * Retrieves the name and pkey of the datapoint's associated station
		  *
		  * @param entitykey    Pkey of the datapoint
          *        stationName  OUT parameter - stores the name of station
          *        stationKey   OUT parameter - stores the pkey of station
		  *
		  * @exception TA_Core::DataRecordingAgentException Thrown if
		  *			   there is a problem retrieving the station name and/or key
		  *
		  */
        void loadStationNameAndKey(unsigned long entityKey, std::string& stationName, unsigned long& stationKey);

        /**
		  * isReachedMaxTrendsPerStation
		  *
          * Determines whether the maximum allowed trends for the given station
          * has been reached.
		  *
		  * @return TRUE - max reached, FALSE - max not reached
		  *
		  * @param stationKey   Pkey of the station
		  *
		  * @exception TA_Core::NameNotFoundException Thrown if
		  *			   there is a problem resolving the object
		  *
		  */
        bool isReachedMaxTrendsPerStation(unsigned long stationKey);

        /**
		  * storeDatapointForResolving
		  *
          * Stores datapoint information needed to resolve the datapoint and
          * insert into the cache.
		  *
		  * @param datapointData    The datapoint information
          *        stationKey       Pkey of station
          *        stationName      Name of station
		  *
		  */
        void storeDatapointForResolving(DataPoints datapointData, unsigned long stationKey, const std::string& stationName = "");

        /**
		  * removeDatapointForResolving
		  *
          * Remove the datapoint so that it will not be resolved and inserted
          * into the cache.
		  *
		  * @param entitykey    Pkey of datapoint
		  *
		  * @exception TA_Core::DataRecordingAgentException Thrown if
		  *			   there is no station associated with the datapoint
          *            configured in the database.
		  *
		  */
        void removeDatapointForResolving(unsigned long entitykey);

        /**
		  * datapointUpdateTrendingEnabled
		  *
          * Updates the count for trended datapoints configured per station
		  *
		  * @param entitykey        Pkey of datapoint
          *        newTrendingState New trending status of datapoint
		  *
		  * @exception TA_Core::DataRecordingAgentException Thrown if
		  *			   there is no station associated with the datapoint
          *            configured in the database.
		  *
		  */
        void datapointUpdateTrendingEnabled(unsigned long entitykey, bool newTrendingState);

    private:

        bool loadDataPointsFromDatabase();

        void loadDataPointsIntoCache();

        void getDpState(unsigned long entityKey, DataPointData& dataPoint, const CorbaName& corbaName);

        void parseStationNames(const std::string& runParamString, std::vector<std::string>& stationNames);

        void checkTrendsPerStationCount();

        void sortDatapoints(TA_Core::SimpleDbData* dbData);

        void sendMaxTrendsReachedAlarm(const std::string& stationName, int configuredCount);

	    std::vector<DataPoints> m_datapointsToResolve;

	    bool m_terminated;

        bool m_dbLoadingFinished;

        struct DatapointCount
        {
            std::string stationName;
            int count;
        };

        std::map<unsigned long, DatapointCount> m_stationNames;

        int m_maxTrendDatapointsPerStation;

        static TA_Core::NonReEntrantThreadLockable m_threadLock;

        typedef TA_Core::NamedObject<TA_Bus::DataPointCorbaDef,
                             TA_Bus::DataPointCorbaDef_ptr,
                             TA_Bus::DataPointCorbaDef_var> DataPointNamedObj;

        ScadaHistorianAgentProcess* m_parentProcess;

    }; // class DatapointLoaderThread

} // TA_App


#endif // DATAPOINT_LOADER_THREAD_H



