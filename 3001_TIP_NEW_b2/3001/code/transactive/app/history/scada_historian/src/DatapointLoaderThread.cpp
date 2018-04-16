/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/src/DatapointLoaderThread.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#include <stdlib.h>
#include "app/history/scada_historian/src/DatapointLoaderThread.h"
#include "app/history/scada_historian/src/DataPointData.h"
#include "app/history/scada_historian/src/DataPointCache.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/naming/src/Naming.h"
#include "core/exceptions/src/HistoryInitialisationException.h"
#include "core/exceptions/src/NameNotFoundException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataRecordingAgentException.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/message/types/DataRecordingAgentAlarms_MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/StationEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/SimpleDbDatabase.h"
#include "core/data_access_interface/src/IData.h"

using TA_Core::DebugUtil;
using TA_Core::RunParams;
using TA_Core::Naming;
using TA_Core::IEntityData;
using TA_Core::ILocation;
using TA_Core::EntityAccessFactory;
using TA_Core::LocationAccessFactory;
using TA_Core::DataPointEntityData;
using TA_Core::StationEntityData;
using TA_Core::SimpleDbDatabase;
using TA_Core::SimpleDbData;
using TA_Core::IData;
using TA_Core::DatabaseException;
using TA_Core::DataException;
using TA_Core::DataRecordingAgentException;

static const int NUMBER_OF_COLUMNS = 5;

const std::string ENTITY_KEY_COL_NAME = "EntityKey";
const std::string ENTITY_NAME_COL_NAME = "EntityName";
//const std::string STATION_NAME_COL_NAME = "StationName";
//const std::string STATION_KEY_COL_NAME = "StationKey";
const std::string AGENT_NAME_COL_NAME = "AgentName";
const std::string LOCATION_NAME_COL_NAME = "LocationName";
const std::string LOCATION_KEY_COL_NAME = "LocationKey";

static const std::string COLUMN_NAMES[NUMBER_OF_COLUMNS] =
{
    ENTITY_KEY_COL_NAME,
    ENTITY_NAME_COL_NAME,
    LOCATION_KEY_COL_NAME,
    LOCATION_NAME_COL_NAME,
    AGENT_NAME_COL_NAME
    //STATION_NAME_COL_NAME,
    //STATION_KEY_COL_NAME,
};


namespace TA_App
{
    TA_Core::NonReEntrantThreadLockable DatapointLoaderThread::m_threadLock;

    //
	// datapointLoaderThread
	//
	DatapointLoaderThread::DatapointLoaderThread(ScadaHistorianAgentProcess* parentProcess)
		: m_terminated(false),
          m_dbLoadingFinished(false),
          m_maxTrendDatapointsPerStation(DEFAULT_MAX_TREND_DPS_PER_STATION_CFG),
          m_parentProcess(parentProcess)
	{
		LOG(SourceInfo, DebugUtil::FunctionEntry, "DatapointLoaderThread");

        std::string maxTrends = RunParams::getInstance().get(MAX_TREND_DPS_PER_STATION_CFG);
        if(!maxTrends.empty())
        {
            m_maxTrendDatapointsPerStation = atoi(maxTrends.c_str());
        }

		//
		// If the datapoint cannot be loaded from the database, throw an
		// exception as the agent cannot do anything if there are no
		// datapoints to sample.
		//
		if(!loadDataPointsFromDatabase())
		{
			TA_THROW(TA_Core::HistoryInitialisationException(
				"Datapoints could not be loaded from database."));
		}

        checkTrendsPerStationCount();

        m_dbLoadingFinished = true;

        LOG(SourceInfo, DebugUtil::FunctionExit, "DatapointLoaderThread");
	}


	//
	// run
	//
    void DatapointLoaderThread::run()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "run");

		//
        // Loop to resolve data	points
		//
		//while(!m_terminated && (m_datapointsToResolve.size()!=0))
        while(!m_terminated)
		{
			loadDataPointsIntoCache();

			//
			// Have attempted to resolve all datapoints that were loaded from
			// the database. Sleep and then try again for the datapoints that
			// could not be resolved the previous iteration.
			//
			int timeInMillisecs = 200;
			sleep(timeInMillisecs);
		}

		LOG(SourceInfo, DebugUtil::FunctionExit, "run");
    }


	//
	// terminate
	//
	void DatapointLoaderThread::terminate()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "terminate");

		m_terminated = true;

        LOG(SourceInfo, DebugUtil::FunctionExit, "terminate");
    }


    //
	// loadStationNameAndKey
	//
    void DatapointLoaderThread::loadStationNameAndKey(unsigned long entityKey, std::string& stationName, unsigned long& stationKey)
    {
        char buffer[200];
        sprintf(buffer, "%lu", entityKey);
        std::string key(buffer);

        //std::string sql = "SELECT pkey, name FROM ENTITY WHERE pkey = ";
        //sql += "(SELECT parentkey FROM ENTITY WHERE pkey = ";
        //sql += "(SELECT parentkey FROM ENTITY WHERE pkey = ";
        //sql += "(SELECT parentkey FROM ENTITY WHERE pkey = ";
        //sql += "(SELECT parentkey FROM ENTITY WHERE pkey = ";
        //sql += key;
        //sql += " ))))";
        std::string sql = "select l.pkey, l.name from LOCATION l, ENTITY e where ";
        sql += "e.locationkey = l.pkey and e.pkey = ";
        sql += key;

        std::vector<std::string> columnNames;
        columnNames.push_back("StationKey");
        columnNames.push_back("StationName");

        SimpleDbDatabase simpleDbDatabase;
        IData* dbData;
        try
        {
            simpleDbDatabase.connect();
            dbData = simpleDbDatabase.executeQuery(sql, columnNames);
        }
        catch(DatabaseException& ex)
        {
            std::string error("Database problem: ");
            error += ex.what();
            TA_THROW(DataRecordingAgentException(error));
        }

        try
        {
            SimpleDbData* sdbData = dynamic_cast <SimpleDbData*>(dbData);

            if(sdbData->getNumRows() != 1)
            {
                std::string error("Retrieved invalid data from database");
                TA_THROW(DataRecordingAgentException(error));
            }

            stationKey = sdbData->getUnsignedLongData(0, "StationKey");
            stationName = sdbData->getStringData(0, "StationName");
        }
        catch(DatabaseException& ex)
        {
            std::string error("Database problem: ");
            error += ex.what();
            TA_THROW(DataRecordingAgentException(error));
        }
    }


    //
	// isReachedMaxTrendsPerStation
	//
    bool DatapointLoaderThread::isReachedMaxTrendsPerStation(unsigned long stationKey)
    {
        //
        // Make sure initialisation is finished before we find the number of
        // dps loaded per station
        //
        while(!m_dbLoadingFinished)
        {
            sleep(100);
        }

        std::map<unsigned long, DatapointCount>::iterator statNameIt;
        statNameIt = m_stationNames.find(stationKey);
        if((statNameIt->second).count >= m_maxTrendDatapointsPerStation)
        {
            return true;
        }

        return false;
    }


	//
	// loadDataPointFromDatabase
	//
    bool DatapointLoaderThread::loadDataPointsFromDatabase()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "loadDataPointsFromDatabase");

        //
		// Load all Pkey, name, subsystem key, region key, and type name of all datapoints
        // that ONLY have Trending Enabled
		//
	    std::string sql("select e.pkey, e.name, l.pkey, l.name, ae.name "); //, p.name, p.pkey");
        sql += "from entity e, location l, entity ae, "; //entity p ";
	    sql += "entitytype t, entityparameter ep, entityparametervalue epv ";
        sql += "where t.name = '" + std::string(DATAPOINT_TYPENAME);
	    sql += "' and t.pkey = e.typekey ";
        sql += "and l.pkey = e.locationkey ";
        sql += "and ep.name = 'EnableTrending' and ep.typekey = t.pkey and epv.entitykey = e.pkey ";
        sql += "and epv.parameterkey = ep.pkey and (epv.value = 'TRUE' or epv.value = 'true' or epv.value = '1')";

        //
        // Get the agent name for each datapoint
        //
        sql += " and ae.name = (select name from entity where pkey = e.agentkey)";

        //
        // We also want to get the station name the datapoint belongs to
        //
        //sql += " and p.name = ( select name from entity where pkey = ( ";
        //sql += "select station.pkey from entity station, entity stationSystem ";
        //sql += "where stationSystem.pkey = ( select stationSystem.pkey from entity ";
        //sql += "stationSystem, entity stationSubsystem where stationSubsystem.pkey = ( ";
        //sql += "select stationSubsystem.pkey from entity stationSubsystem, entity equipment ";
        //sql += "where equipment.pkey = ( select equipment.pkey from entity equipment, entity datapoint ";
        //sql += "where datapoint.parentkey = equipment.pkey and datapoint.pkey = e.pkey ) ";
        //sql += "and equipment.parentkey = stationSubsystem.pkey ) ";
        //sql += "and stationSubsystem.parentkey = stationSystem.pkey ) ";
        //sql += "and stationSystem.parentkey = station.pkey )) ";
        //sql += "and p.pkey = (select pkey from entity where name = p.name)";

		//
        // If the StationName parameter is set, only retrieve the data points for that
        // particular station (can be more than one station, delimited by comma)
		//
        std::string stationName("");
        stationName = RunParams::getInstance().get("StationName");
        if(stationName != "" )
        {
            std::vector<std::string> stationNames;
            parseStationNames(stationName, stationNames);

            //sql += " and e.parentkey in (select pkey from entity where parentkey in (select pkey ";
            //sql += " from entity where parentkey in (select pkey from entity where parentkey in ";
            //sql += "(select pkey from entity where name in (";
            sql += " and l.name in(";

            std::string loadNames("");
            for(unsigned int i=0; i < stationNames.size(); ++i)
            {
                loadNames += "'";
                loadNames += stationNames[i];
                loadNames += "'";

                if(i != stationNames.size() - 1)
                {
                    loadNames += ",";
                }
            }

            sql += loadNames;
            //sql += ")))))";
            sql += ")";
        }

        m_datapointsToResolve.clear();

        std::vector<std::string> columnNames;
        for(int i = 0; i < NUMBER_OF_COLUMNS; ++i)
        {
            columnNames.push_back(COLUMN_NAMES[i]);
        }

        SimpleDbDatabase simpleDbDatabase;
        IData* dbData;
        try
        {
            simpleDbDatabase.connect();
            dbData = simpleDbDatabase.executeQuery(sql, columnNames);
        }
        catch(DatabaseException&)
        {
            return false;
        }

        int loadedFromDb = 0;
        try
        {
            do
            {
                sortDatapoints(dynamic_cast <SimpleDbData*>(dbData));
                loadedFromDb += dbData->getNumRows();
                delete dbData;
                dbData = NULL;
            }
            while(simpleDbDatabase.moreData(dbData) && !m_terminated);
        }
        catch(DatabaseException& ex)
        {
            if(loadedFromDb > 0)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                    "There was an error loading all datapoints configured for trending from database: %s. "
                    "Only %d datapoints loaded.", ex.what(), loadedFromDb);
            }
        }

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Number of Data Points loaded from database: %d", loadedFromDb);
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Number of Data Points loaded into cache: %lu", m_datapointsToResolve.size());

        return true;
    }


    //
	// sortDatapoints
	//
    void DatapointLoaderThread::sortDatapoints(SimpleDbData* dbData)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "sortDatapoints");

        for(unsigned int i=0; (i < dbData->getNumRows() && !m_terminated); ++i)
        {
            DataPoints data;
            unsigned long stationKey;
            std::string stationName;
            try
            {
                //stationKey = dbData->getUnsignedLongData(i, STATION_KEY_COL_NAME);
                //stationName = dbData->getStringData(i, STATION_NAME_COL_NAME);
                data.pKey = dbData->getStringData(i, ENTITY_KEY_COL_NAME);
                data.name = dbData->getStringData(i, ENTITY_NAME_COL_NAME);
                data.agentName = dbData->getStringData(i, AGENT_NAME_COL_NAME);
                stationName = dbData->getStringData(i, LOCATION_NAME_COL_NAME);
                stationKey = dbData->getUnsignedLongData(i, LOCATION_KEY_COL_NAME);
            }
            catch(DataException& ex)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                    "Error loading datapoint: %s. ", ex.what());
                continue;

            }

            storeDatapointForResolving(data, stationKey, stationName);
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "sortDatapoints");
    }


    //
	// storeDatapointForResolving
	//
    void DatapointLoaderThread::storeDatapointForResolving(DataPoints datapointData, unsigned long stationKey, const std::string& stationName /* = "" */)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "storeDatapointForResolving");

        TA_Core::ThreadGuard guard(m_threadLock);

        //
        // If the datapoint has already been stored, this is probably an update.
        //
        std::vector<DataPoints>::iterator dpIt = m_datapointsToResolve.begin();
        for(; (dpIt != m_datapointsToResolve.end() && !m_terminated); ++dpIt)
        {
            if(dpIt->pKey == datapointData.pKey)
            {
                dpIt->name = datapointData.name;
                dpIt->agentName = datapointData.agentName;
                return;
            }
        }

        //
        // Keep a count of how many datapoints have been inserted per station.
        // There is a limit on this.
        // Once we reach the limit, we will keep counting, but will not
        // insert anymore.
        //
        std::map<unsigned long, DatapointCount>::iterator statNameIt;
        statNameIt = m_stationNames.find(stationKey);
        if(statNameIt == m_stationNames.end())
        {
            //
            // Pkey doesn't exist, add it
            //
            DatapointCount dc;
            dc.count = 1;

            if(stationName.empty())
            {
                //
                // Get station name
                //
                //StationEntityData* stationData = dynamic_cast <StationEntityData*> (EntityAccessFactory::getInstance().getEntity(stationKey));
                //dc.stationName = stationData->getName();

                ILocation* stationData = dynamic_cast <ILocation*> (LocationAccessFactory::getInstance().getLocationByKey(stationKey));
                dc.stationName = stationData->getName();
            }
            else
            {
                dc.stationName = stationName;
            }

            m_stationNames[stationKey] = dc;
        }
        else
        {
            //
            // Station does exist, so increment the count
            //
            if(++(statNameIt->second).count > m_maxTrendDatapointsPerStation)
            {
                //
                // Reached limit, don't insert into cache. If loading has finished, we are
                // here due to a configuration update. Hence send an alarm warning that the
                // new datapoint configured for trending won't be trended.
                //
                if(m_dbLoadingFinished)
                {
                    sendMaxTrendsReachedAlarm((statNameIt->second).stationName, (statNameIt->second).count);
                }

                return;
            }
        }

        m_datapointsToResolve.push_back(datapointData);
    }


    void DatapointLoaderThread::removeDatapointForResolving(unsigned long entitykey)
    {
        TA_Core::ThreadGuard guard(m_threadLock);

        std::vector<DataPoints>::iterator dpIt = m_datapointsToResolve.begin();
        for(; (dpIt != m_datapointsToResolve.end() && !m_terminated); ++dpIt)
        {
            if(strtoul((dpIt->pKey).c_str(), NULL, 10) == entitykey)
            {
                m_datapointsToResolve.erase(dpIt);
                datapointUpdateTrendingEnabled(entitykey, false);
            }
        }
    }


	//
	// loadDataPointsIntoCache
	//
    void DatapointLoaderThread::loadDataPointsIntoCache()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "loadDataPointsIntoCache" );

		//
        // For each data point, resolve the object from the naming service,
	    // determine the current values and store
		//
		int resolvedThisIteration=0;
		std::vector<DataPoints>::iterator dpIterator;
		dpIterator = m_datapointsToResolve.begin();
		while((dpIterator != m_datapointsToResolve.end()) && !m_terminated)
		{
			//
            // Create a new DataPointData instance
			//
		    std::string entityKeyStr = dpIterator->pKey;
		    unsigned long entityKey = strtoul(entityKeyStr.c_str(), NULL, 10);
		    DataPointData dpData(entityKey);

            CorbaName corbaName(dpIterator->agentName, dpIterator->name);

		    try
		    {
			    getDpState(entityKey, dpData, corbaName);
                dpData.setError(false);

                //
                // Create a message subscriber for the datapoint
                //
                m_parentProcess->addDataPointSubscriber(entityKey);

                dpIterator = m_datapointsToResolve.erase(dpIterator);
                ++resolvedThisIteration;
		    }
		    catch(const TA_Core::ObjectResolutionException& ex)
		    {
			    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Cannot resolve object from Naming Service:");
			    LOGMORE(SourceInfo, ex.what());

                dpData.setError(true);

			    ++dpIterator;
                //continue;
		    }
		    catch(...)
		    {
			    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Caught an unexpected exception.");
			    ++dpIterator;
                continue;
		    }

            //
		    // Store DataPointData into the cache
			//
			DataPointCache::getInstance().insert(dpData);
		}

        //LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Resolved %d DataPoints.", resolvedThisIteration);
    }


	//
	// getDpState
	//
    void DatapointLoaderThread::getDpState(unsigned long entityKey, DataPointData& dataPoint, const CorbaName& corbaName)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "GetDpState for entitykey: %lu", dataPoint.dataPointId());

        if(RunParams::getInstance().isSet("Testing"))
	    {
            // Store the current values of the data point
	        dataPoint.dataPointStatus(192);

            TA_Bus::DataPointCorbaDef::DataPointValue dpValue;
            dpValue.floatValue(1);
	        dataPoint.dataPointValue(dpValue);

            time_t temp;
            time(&temp);
	        dataPoint.dataTimeStamp(temp);

            dataPoint.dataCollectionOn(true);

            return;
        }

	    //
		// Resolve the datapoint and get the data that we're interested in.
		// If ResolveDataPoint throws an exception, let it be caught higher up.
		//
	    DataPointNamedObj dataPointNamedObj(corbaName.getAgentName(), corbaName.getObjectName());
        TA_Bus::DataPointCorbaDef::ClientDataPointState* dpState;

        try
        {
            //
		    // Try to get the datapoint state. If this throws an exception, let it
		    // be caught higher up.
		    //
		    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Trying to get state...");
			CORBA_CALL_RETURN( dpState,
			                   dataPointNamedObj,
			                   GetDataPointState,
			                   () );
        }
        catch (...)
        {
	        // Handle exception
            throw;
        }

		dataPoint.dataPointStatus(dpState->dataPointStatus);
	    dataPoint.dataPointValue(dpState->value);
	    dataPoint.dataTimeStamp(dpState->timeStamp);

		//
        // Set data collection to ON. Don't need to call IsTrendingEnabled() as
		// we only loaded the datapoints that had trending set to Enabled.
		//
	    dataPoint.dataCollectionOn(true);
    }


    //
    // parseStationNames
    //
    void DatapointLoaderThread::parseStationNames(const std::string& runParamString, std::vector<std::string>& stationNames)
    {
        TA_ASSERT(!runParamString.empty(), "StationName parameter string is empty");

        stationNames.clear();

        unsigned int startPos = 0;
        int delimPos = runParamString.find(",", startPos);

        if(-1 == delimPos)
        {
            std::string stationName = runParamString.substr(startPos, delimPos);
            stationNames.push_back(stationName);
            startPos = runParamString.size();
        }

        while(startPos != runParamString.size())
        {
            std::string stationName = runParamString.substr(startPos, delimPos);
            stationNames.push_back(stationName);

            startPos = delimPos + 1;
            delimPos = runParamString.find(",",startPos);

            if(delimPos == -1)
            {
                delimPos = runParamString.size() - 1;
            }
        }

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Number of stations specified: %d", stationNames.size());
    }


    //
    // checkTrendsPerStationCount
    //
    void DatapointLoaderThread::checkTrendsPerStationCount()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "checkTrendsPerStationCount" );

        TA_Core::ThreadGuard guard(m_threadLock);

        std::map<unsigned long, DatapointCount>::iterator statNameIt;
        statNameIt = m_stationNames.begin();

        for(; statNameIt != m_stationNames.end() && !m_terminated; ++statNameIt)
        {
            if((statNameIt->second).count > m_maxTrendDatapointsPerStation)
            {
                //
                // Send an alarm to indicated that not all configured
                // datapoints will be trended
                //
                sendMaxTrendsReachedAlarm((statNameIt->second).stationName, (statNameIt->second).count);
            }
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "checkTrendsPerStationCount" );
    }


    //
    // sendMaxTrendsReachedAlarm
    //
    void DatapointLoaderThread::sendMaxTrendsReachedAlarm(const std::string& stationName, int configuredCount)
    {

        try
        {
            char buffer[100];

            TA_Core::DescriptionParameters dp;
	        dp.push_back(new TA_Core::NameValuePair("Location", stationName));
	        sprintf(buffer, "%d", m_maxTrendDatapointsPerStation);
            dp.push_back(new TA_Core::NameValuePair("MaxCount", buffer));

            TA_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarm(TA_Core::DataRecordingAgentAlarms::MaxTrendsPerLocationReached,
                strtoul(TA_Core::RunParams::getInstance().get("EntityKey").c_str(), NULL, 10),
				strtoul(TA_Core::RunParams::getInstance().get("TypeKey").c_str(), NULL, 10),
				dp,
				TA_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME),
				0,
				0);
        }
        catch(...)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                "Error: Couldn't submit alarm: %s has %d datapoints configured for trending. "
                "A maximum of %d datapoints per station configured for trending allowed. Not all "
                "configured datapoints will be trended",
                stationName.c_str(), configuredCount, m_maxTrendDatapointsPerStation);
        }

		TA_Core::AlarmHelperManager::getInstance().getAlarmHelper().cleanUp();
    }


    //
    // datapointUpdateTrendingEnabled
    //
    void DatapointLoaderThread::datapointUpdateTrendingEnabled(unsigned long entitykey, bool newTrendingState)
    {
        std::string stationName;
        unsigned long stationKey;

        loadStationNameAndKey(entitykey, stationName, stationKey);

        TA_Core::ThreadGuard guard(m_threadLock);

        std::map<unsigned long, DatapointCount>::iterator statNameIt;
        statNameIt = m_stationNames.find(stationKey);

        if(newTrendingState)
        {
            ++(statNameIt->second).count;
            m_parentProcess->addDataPointSubscriber(entitykey);
        }
        else
        {
            --(statNameIt->second).count;
            m_parentProcess->removeDataPointSubscriber(entitykey);
        }
    }

} // TA_App
