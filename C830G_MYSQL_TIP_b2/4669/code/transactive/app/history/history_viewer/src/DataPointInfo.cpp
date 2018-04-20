/**
  * DataPointInfo.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/history_viewer/src/DataPointInfo.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class loads the data points and maintains a store of them.
  *
  */

#include "app/history/history_viewer/src/stdafx.h"
#include "app/history/history_viewer/src/historyviewer.h"
#include "app/history/history_viewer/src/DataPointInfo.h"
#include "app/history/history_viewer/src/DataBaseConnection.h"
#include "app/history/history_viewer/src/globals.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/database/src/DBException.h"

#ifdef USE_OCCI
#include <occi.h>
#else
#include "core/database/src/SimpleDb.h"
#endif // USE_OCCI
#include "core/corba/src/CorbaUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//
// These constants are used to calculate how to stack the boolean values
//
const short  numBools = 10;		// Number of booleans to stack
const double boolOffset = 0.5;	// Offset from 0 in percentage units

using TA_Core::DebugUtil;
using TA_Core::RunParams;
using TA_Core::ReEntrantThreadLockable;
using TA_Core::ThreadGuard;

namespace TA_App
{

    ReEntrantThreadLockable CDataPointInfo::m_lock;

    //
    // ~CDataPointInfo
    //
    CDataPointInfo::~CDataPointInfo()
    {
        m_DataPointMap.clear();
        m_GraphingDataPointMap.clear();
    }


    //
    // LoadDataPoints
    //
    void CDataPointInfo::LoadDataPoints()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "LoadDataPoints");

        ThreadGuard guard(m_lock);

        DataPoint tempDataPoint;

        //
        // Only load this once
        //
        if( m_Loaded == true )
        {
	        return;
        }
        m_Loaded = true;

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"Loading DataPoint Information");

        // *** IMPORTANT ***
        // This may be replaced by calls to the DataPoint corba objects later...
        //std::vector<std::string> names;
        //CorbaUtil::GetInstance().ListObjectNames("OPCDevices/DataPoint/", names);

#ifdef USE_OCCI
        //
        // SQL statement to get the required datapoint parameters.
        // NEED to redo this in the new style of loading parameters just when required!!!
        //
        std::string sql( "SELECT DataPointType.pkey EntityKey, DataPointType.name EntityName, "
	        "DataPointType.description Description, DataPointType.value DataPointType, "
	        "EngLow.value EngLow, EngHigh.value EngHigh, EngUnits.value BaseData, "
            "Trending.value TrendingEnabled, DataPointType.subsystemname FROM " // Subsystem
	        "(SELECT e.pkey pkey, e.name name, e.description description, epv.value value, "
            "s.name subsystemname" // Subsystem
	        "FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv, "
            "SUBSYSTEM s "
	        "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND "
	        "e.pkey = epv.entitykey AND ep.pkey = epv.parameterkey AND "
	        "et.name='DataPoint' AND ep.name='DataPointType' AND s.pkey = e.subsystemkey" // Subsystem
            ") DataPointType, "
	        "(SELECT e.pkey pkey, epv.value value "
	        "FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv "
	        "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND "
	        "e.pkey = epv.entitykey AND ep.pkey = epv.parameterkey AND "
	        "et.name='DataPoint' AND ep.name='EnableTrending') Trending, "
	        "(SELECT e.pkey pkey, epv.value value "
	        "FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv "
	        "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND "
	        "e.pkey = epv.entitykey AND ep.pkey = epv.parameterkey AND "
	        "et.name='DataPoint' AND ep.name='EngineeringLimitLow') EngLow, "
	        "(SELECT e.pkey pkey, epv.value value "
	        "FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv "
	        "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND "
	        "e.pkey = epv.entitykey AND ep.pkey = epv.parameterkey AND "
	        "et.name='DataPoint' AND ep.name='EngineeringLimitHigh') EngHigh, "
	        "(SELECT e.pkey pkey, epv.value value "
	        "FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv "
	        "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND "
	        "e.pkey = epv.entitykey AND ep.pkey = epv.parameterkey AND "
	        "et.name='DataPoint' AND ep.name='UnitLabel') EngUnits "
            "WHERE DataPointType.pkey = Trending.pkey AND "
#ifdef USE_OCCI
	        "DataPointType.pkey = EngLow.pkey(+) AND "
	        "DataPointType.pkey = EngHigh.pkey(+) AND "
	        "DataPointType.pkey = EngUnits.pkey(+) AND "
#else // USE_OCCI
	        // Cannot do join with SimpleDb as it can't handle NULLs
	        "DataPointType.pkey = EngLow.pkey AND "
	        "DataPointType.pkey = EngHigh.pkey AND "
	        "DataPointType.pkey = EngUnits.pkey "//AND "
#endif // USE_OCCI
	        //"Trending.value = 'TRUE' "
	        "ORDER BY DataPointType.pkey" );

        //
        // Get the oracle connection.  As errors in the oracle connection will have already
        // resulted in an error being displayed, don't display another one here.
        //
        oracle::occi::Connection *conn;
        try
        {
	        conn = DataBaseConnection::getInstance().GetConnection();
        }
        catch(...)
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Could not create database connection.");
	        return;
        }

        oracle::occi::Statement *stmt = NULL;
        oracle::occi::ResultSet *resultSet = NULL;
        try
        {
            //
	        // Create the statement
            //
	        stmt = conn->createStatement( sql );

            //
	        // Set to get a maximum of 500 rows at a time (doesn't mean that it is
	        // limited to 500 rows)
            //
	        stmt->setPrefetchRowCount( 500 );
	        stmt->setPrefetchMemorySize( 0 ); // To ensure it uses the row count

            //
	        // Execute the statement and put the results into ther result set
            //
	        resultSet = stmt->executeQuery();

	        while( resultSet->next() )
	        {
                //
		        // Add Datapoints
                //
		        tempDataPoint.entityKey = resultSet->getUInt( 1 );
		        tempDataPoint.entityName = resultSet->getString( 2 );
		        tempDataPoint.description = resultSet->getString( 3 );
		        tempDataPoint.dpType = resultSet->getString( 4 );
		        if( resultSet->isNull( 5 ) )
		        {
			        tempDataPoint.engLow = 0.0;
		        }
		        else
		        {
			        tempDataPoint.engLow = resultSet->getDouble( 5 );
		        }
		        if( resultSet->isNull( 6 ) )
		        {
			        if( tempDataPoint.dpType == DP_BOOLEAN_TYPE )
			        {
				        tempDataPoint.engHigh = 1.0;
			        }
			        else
			        {
				        tempDataPoint.engHigh = 100.0;
			        }
		        }
		        else
		        {
			        tempDataPoint.engHigh = resultSet->getDouble( 6 );
		        }
		        tempDataPoint.engUnits = resultSet->getString( 7 );

		        ASSERT( tempDataPoint.engLow >= tempDataPoint.engHigh );
                if(dataPoint.engLow < dataPoint.engHigh)
                {
                    std::ostringstream msg;
                    msg << "Error in configuration data for datapoint: ";
                    msg << dataPoint.entityKey << " : ";
                    msg << "EngineeringLimitHigh value must be greater than EngineeringLimitLow value.";
                    CouldNotLoadException e;
                    e.SetErrorMsg(msg.str());
                    throw e;
                }

		        tempDataPoint.configIsLoaded = false;

                tempDataPoint.trendingEnabled = (stricmp(resultSet->getString( 8 ), "true") == 0) ? true : false; 

                tempDataPoint.subsystem = resultSet->getString( 8 ); // Subsystem

		        m_DataPointMap[tempDataPoint.entityKey] = tempDataPoint;
	        }

	        stmt->closeResultSet( resultSet );
	        conn->terminateStatement( stmt );
        }
        catch ( oracle::occi::SQLException &sqlException )
        {
	        if( resultSet )
	        {
		        stmt->closeResultSet( resultSet );
	        }
	        if( stmt )
	        {
		        conn->terminateStatement( stmt );
	        }

	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Could not load datapoints.");

	        // Log the Oracle Error Message (includes error number)
	        LOGMORE(DebugUtil::DebugError, sqlException.getMessage().c_str());

	        CouldNotLoadException e;
	        e.SetErrorMsg( sqlException.getMessage() );
	        throw e;
        }
#else // USE_OCCI

        /* std::string sql( "SELECT e.pkey pkey, e.name name, e.description description "
	        "FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv "
	        "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND "
	        "e.pkey = epv.entitykey AND ep.pkey = epv.parameterkey AND "
	        "et.name='DataPoint' AND ep.name='EnableTrending' AND "
	        "epv.value = 'TRUE'"); */
        std::string sql( "SELECT e.pkey, e.name, e.description, epv.value, s.name " // Subsystem
	        "FROM ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv, SUBSYSTEM s " // Subsystem
	        "WHERE e.typekey = (SELECT pkey FROM ENTITYTYPE WHERE name = 'DataPoint') "
	        "AND ep.typekey = e.typekey AND e.pkey = epv.entitykey "
	        "AND ep.pkey = epv.parameterkey AND ep.NAME = 'EnableTrending' "
            "AND s.pkey = e.subsystemkey"); // Subsystem
	        //"AND (epv.VALUE = 'TRUE' OR epv.VALUE = 'True')");

        //
        // If the StationName parameter is set, only retrieve the data points for that
        // particular station (can be more than one station, delimited by comma)
		//
        std::string stationName("");
        stationName = RunParams::getInstance().get(RPARAM_STATION_NAME);
        if(stationName != "" )
        {
            std::vector<std::string> stationNames;
            parseStationNames(stationName, stationNames);

            sql += " and e.parentkey in (select pkey from entity where parentkey in (select pkey ";
            sql += " from entity where parentkey in (select pkey from entity where parentkey in ";
            sql += "(select pkey from entity where name in (";
            
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
            sql += ")))))";
        }

        SimpleDb &db = *(SimpleDBConnection::getInstance().GetConnection());
        SimpleDb::Buffer buffer;
        int columns = 5;

		try
		{
			db.Exec( sql.c_str(), columns, buffer );
		}
		catch( const DBException& e)
		{
			LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary().c_str() );
		}

        if (buffer.empty())
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"No datapoints found.");
	        return;
        }

        bool hasMore = true;
        do {
	        int rows = buffer.size();
	        for (int i=0; (i<rows && !m_stopLoad); ++i)
	        {
		        // Add Datapoints
		        tempDataPoint.entityKey = atol(buffer[i][0].c_str());
		        tempDataPoint.entityName = buffer[i][1];
		        tempDataPoint.description = buffer[i][2];
                tempDataPoint.trendingEnabled = (stricmp(buffer[i][3].c_str(), "true") == 0) || 
                    stricmp(buffer[i][3].c_str(), "1") == 0 ? true : false;
                tempDataPoint.subsystem = buffer[i][4];

		        // Load these values later (to save time while loading)
		        tempDataPoint.dpType = "";
		        tempDataPoint.engLow = 0.0;
		        tempDataPoint.engHigh = 100.0;
		        tempDataPoint.engUnits = "";
		        tempDataPoint.configIsLoaded = false;
                
		        m_DataPointMap[tempDataPoint.entityKey] = tempDataPoint;
	        }

			try
			{
				if( !m_stopLoad && (hasMore = db.HasMore()) == true )
				{
					db.FetchMore(columns, buffer);
				}
			}
			catch( const DBException& e )
			{
				LOG(SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary().c_str() );
				hasMore = false;
			}

        } while ( hasMore && !m_stopLoad );
        
#endif // USE_OCCI

        if (m_DataPointMap.size() == 0)
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"No datapoints found.");
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "LoadDataPoints");
    }

    const DataPointMap& CDataPointInfo::GetDataPointMap()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "GetDataPointMap");

        if( !m_Loaded )
        {
	        LoadDataPoints();
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "GetDataPointMap");

        return m_DataPointMap;
    }


    // 
    // AddToGraph
    //
    void CDataPointInfo::AddToGraph(std::vector <unsigned long>& dataPointKeyList)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "AddToGraph");

        ThreadGuard guard(m_lock);

        std::vector <unsigned long>::iterator it;
        for ( it = dataPointKeyList.begin(); it != dataPointKeyList.end(); it++ )
        {
	        DataPointMap::iterator dpIt = m_DataPointMap.find( *it );
	        if( dpIt != m_DataPointMap.end() )
	        {
                //
		        // Found it, move it from m_DataPointMap to m_GraphingDataPointMap
                //

		        // Add it to m_GraphingDataPointMap
		        DataPoint graphDP = dpIt->second;
#ifndef USE_OCCI
                //try
                //{
		            LoadConfig( graphDP );
                //}
                //catch(CouldNotLoadException& ex)
                //{
                //    LOG(SourceInfo, DebugUtil::ExceptionCatch, "CouldNotLoadException", ex.GetErrorMsg().c_str());

                //    std::string msg("Cannot not add data point to graph: ");
                //    msg += ex.GetErrorMsg();
                //    AfxMessageBox(msg.c_str());
                //    continue;
                //}
#endif
		        m_GraphingDataPointMap[*it] = graphDP;

		        // Remove it from m_DataPointMap
		        m_DataPointMap.erase( dpIt );
	        }
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "AddToGraph");
    }


    // 
    // DeleteFromGraph
    //
    void CDataPointInfo::DeleteFromGraph(std::vector <unsigned long>& dataPointKeyList)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "DeleteFromGraph");

        ThreadGuard guard(m_lock);

        std::vector <unsigned long>::iterator it;
        for ( it = dataPointKeyList.begin(); it != dataPointKeyList.end(); it++ )
        {
	        DataPointMap::iterator dpIt = m_GraphingDataPointMap.find( *it );
	        if( dpIt != m_GraphingDataPointMap.end() )
	        {
                //
		        // Found it, move it from m_GraphingDataPointMap to m_DataPointMap
                //

		        // Add it to m_GraphingDataPointMap
		        m_DataPointMap[*it] = dpIt->second;

		        // Remove it from m_DataPointMap
		        m_GraphingDataPointMap.erase( dpIt );
	        }
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "DeleteFromGraph");
    }


    // 
    // DeleteDataPoint
    //
    void CDataPointInfo::DeleteDataPoint(unsigned long dataPointKey)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "DeleteDataPoint");

        ThreadGuard guard(m_lock);

        DataPointMap::iterator dpIt = m_DataPointMap.find(dataPointKey);
        if(dpIt != m_DataPointMap.end())
        {
            m_DataPointMap.erase(dpIt);
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "DeleteDataPoint");
    }


    // 
    // InsertDataPoint
    //
    void CDataPointInfo::InsertDataPoint(const DataPoint& datapoint)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "InsertDataPoint");

        ThreadGuard guard(m_lock);

        //
        // Only insert if datapoint doesn't already exist
        //
        DataPointMap::iterator dpIt = m_DataPointMap.find( datapoint.entityKey );
        if( dpIt == m_DataPointMap.end() )
        {
            m_DataPointMap[datapoint.entityKey] = datapoint;
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "InsertDataPoint");
    }


    // 
    // UpdateDataPoint
    //
    void CDataPointInfo::UpdateDataPoint(const DataPoint& datapoint)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "UpdateDataPoint");

        ThreadGuard guard(m_lock);

        //
        // Most likely asking for a graphed datapoint so do this first
        //
        DataPointMap::iterator dpIt = m_GraphingDataPointMap.find(datapoint.entityKey);
        if( dpIt == m_GraphingDataPointMap.end() )
        {
            //
	        // Check the non-graphed datapoints
            //
	        dpIt = m_DataPointMap.find(datapoint.entityKey);
	        if( dpIt == m_DataPointMap.end() )
	        {
		        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugWarn, "Cannot find datapoint key: %lu. Nothing updated.", datapoint.entityKey );
                return;
	        }
        }

        (dpIt->second).entityName = datapoint.entityName;
        (dpIt->second).entityKey = datapoint.entityKey;
        (dpIt->second).description = datapoint.description;
        (dpIt->second).dpType = datapoint.dpType;
        (dpIt->second).engLow = datapoint.engLow;
        (dpIt->second).engHigh = datapoint.engHigh;
        (dpIt->second).engUnits = datapoint.engUnits;
        (dpIt->second).configIsLoaded = datapoint.configIsLoaded;
        (dpIt->second).trendingEnabled = datapoint.trendingEnabled;
        (dpIt->second).subsystem = datapoint.subsystem;

        LOG(SourceInfo, DebugUtil::FunctionExit, "UpdateDataPoint");
    }


    // 
    // GetDataPoint
    //
    DataPoint& CDataPointInfo::GetDataPoint(const unsigned long& dataPointKey)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "GetDataPoint");

        ThreadGuard guard(m_lock);

        // Most likely asking for a graphed datapoint so do this first
        DataPointMap::iterator dpIt = m_GraphingDataPointMap.find( dataPointKey );
        if( dpIt == m_GraphingDataPointMap.end() )
        {
	        // Check the non-graphed datapoints
	        dpIt = m_DataPointMap.find( dataPointKey );
	        // If we haven't got it, throw exception
	        if( dpIt == m_DataPointMap.end() )
	        {
		        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugWarn, "Cannot find datapoint key" );
		        throw DatapointNotFoundException();
	        }
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "GetDataPoint");

        return dpIt->second;
    }

#ifndef USE_OCCI
    // 
    // LoadConfig
    //
    void CDataPointInfo::LoadConfig( const unsigned long& dpKey )
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "LoadConfig");

        // Find the datapoint in the map
        try
        {
	        DataPoint& dataPoint = GetDataPoint( dpKey );
	        LoadConfig( dataPoint );
        }
        catch ( CDataPointInfo::DatapointNotFoundException )
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,"LoadConfig: Cannot find DataPoint.");
	        return;  // Should never happen...
        }
        catch(CouldNotLoadException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "CouldNotLoadException", ex.GetErrorMsg().c_str());

            std::string msg("Cannot add data point to graph: ");
            msg += ex.GetErrorMsg();
            AfxMessageBox(msg.c_str());
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "LoadConfig");
    }


    // 
    // LoadConfig
    //
    void CDataPointInfo::LoadConfig( DataPoint& dataPoint )
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "LoadConfig");

        // Only load configuration once
        if( dataPoint.configIsLoaded )
        {
	        return;
        }

        dataPoint.configIsLoaded = true;

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Loading configuration for datapoint: %d", dataPoint.entityKey );

        TA_Core::DataPointEntityData* dpData = NULL;
        try
        {
            TA_Core::IEntityData* iData = TA_Core::EntityAccessFactory::getInstance().getEntity(dataPoint.entityKey);
            dpData = dynamic_cast <TA_Core::DataPointEntityData*> (iData);
        }
        catch(const TA_Core::DatabaseException& ex)
        {
            std::ostringstream msg;
            msg << "Error loading configuration data for datapoint: ";
            msg << dataPoint.entityKey << " : " << ex.what(); 
            CouldNotLoadException e;
            e.SetErrorMsg(msg.str());
            throw e;
        }
        catch(const TA_Core::DataException& ex)
        {
            std::ostringstream msg;
            msg << "Error loading configuration data for datapoint: ";
            msg << dataPoint.entityKey << " : " << ex.what(); 
            CouldNotLoadException e;
            e.SetErrorMsg(msg.str());
            throw e;
        }
        catch(...)
        {
            std::ostringstream msg;
            msg << "Error loading configuration data for datapoint: ";
            msg << dataPoint.entityKey << " : Unknown exception caught"; 
            CouldNotLoadException e;
            e.SetErrorMsg(msg.str());
            throw e;
        }

        dataPoint.dpType = dpData->getDataPointType();

        if(dpData->getEngineeringUnitsEnabled())
        {
            dataPoint.engUnits = dpData->getEngineeringUnits();
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,"Engineering Units set: %s", dataPoint.engUnits.c_str());
        }
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"Engineering Units disabled");
        }

        if(dataPoint.dpType != DP_BOOLEAN_TYPE)
        {
            dataPoint.engLow = dpData->getEngineeringLimitLow();
            dataPoint.engHigh = dpData->getEngineeringLimitHigh();
            dataPoint.scaleFactor = 100.0/(dataPoint.engHigh - dataPoint.engLow);

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Eng Low: %lf, Eng high: %lf", dataPoint.engLow, dataPoint.engHigh);

            if(dataPoint.engLow >= dataPoint.engHigh)
            {
                std::ostringstream msg;
                msg << "Error in configuration data for datapoint: ";
                msg << dataPoint.entityKey << " : ";
                msg << "EngineeringLimitHigh value must be greater than EngineeringLimitLow value.";
                CouldNotLoadException e;
                e.SetErrorMsg(msg.str());
                throw e;
            }
        }
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,"Boolean data point.");

            // Make the range slightly greater so that lines at 0 and 1 don't
	        // disappear into the graph borders.
	        dataPoint.engLow = 0.0;
	        dataPoint.engHigh = 1.0;
        
            // PercentSize is the size in percentage unit
	        double percentSize = 100.0/(double)numBools;
	        // The offset is the offset from the line as it needs to be off the line to
	        // avoid overlap.  The minimum size is 2 percent
	        double newOffset = __min(boolOffset + boolOffset*(int)(m_BoolCount/numBools),
							         (percentSize - 2.0)/2.0);
	        // Given the size and offset, calculate the new range
	        double desiredRange = percentSize - 2.0*newOffset;
	        // The resulting scale factor
	        double scaleFactor = 100/desiredRange;
            
	        // Calculate the engineering low
	        dataPoint.scaleLow = -((double)(m_BoolCount%numBools)*percentSize + newOffset)/desiredRange;

	        // Calculate the engineering high
	        dataPoint.scaleHigh = dataPoint.scaleLow + scaleFactor;

            // Calculate the engineering low
	        dataPoint.engLow = -((double)(m_BoolCount%numBools)*percentSize + newOffset)/desiredRange;

	        // Calculate the engineering high
	        dataPoint.engHigh = dataPoint.engLow + scaleFactor;

            dataPoint.scaleFactor = 100.0/(dataPoint.engHigh - dataPoint.engLow);

	        // Increment the counter
	        m_BoolCount++;
        }

        if(dpData != NULL)
        {
            delete dpData;
        }
        
        /*
        //EntityConfig entityConfig;
        std::string entityParam;

        // Convert data point key to string
        char buff[100];
        sprintf(buff, "%lu", dataPoint.entityKey);
        std::string dataPointKey(buff);

        SimpleDb &db = *(SimpleDBConnection::getInstance().GetConnection());

        // Must manually retrieve parameter values from database now 
        // that Entity Config is obsolete
        std::string sql1("");
        //sql1 = "SELECT value DataPointType, EngEnabled.value EngEnabled FROM ";
        sql1 += "SELECT epv.value FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv ";
        sql1 += "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND e.pkey = epv.entitykey AND e.pkey = ";
        sql1 += dataPointKey + " AND ";
        sql1 += "ep.pkey = epv.parameterkey AND et.name='DataPoint' AND ep.name='DataPointType'";


        SimpleDb::Buffer buffer1;
        int columns = 1;
        db.Exec( sql1.c_str(), columns, buffer1 );

        if (buffer1.empty())
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"No parameter value found for DataPointType.");
	        return;
        }

        if( buffer1[0][0].size() > 0 )
        {
	        dataPoint.dpType = buffer1[0][0];
        }

        // Get EngUnitsEnabled parameter value
        std::string sql5("");
        sql5 += "SELECT epv.value value FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv ";
        sql5 += "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND e.pkey = epv.entitykey AND e.pkey = ";
        sql5 += dataPointKey + " AND ";
        sql5 += "ep.pkey = epv.parameterkey AND et.name='DataPoint' AND ep.name='EngUnitsEnabled'";

        SimpleDb::Buffer buffer5;
        columns = 1;
        db.Exec( sql5.c_str(), columns, buffer5 );

        if (buffer5.empty())
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"No parameter value found for EngUnitsEnabled.");
        }
        else if( buffer5[0][0] == "TRUE" ) // Eng Units enabled
        {
            // Get the EngineeringUnit parameter separately because this can be empty
            std::string sql2("");
            sql2 = "SELECT epv.value value	FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv ";
            sql2 += "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND e.pkey = epv.entitykey AND e.pkey = ";
            sql2 += dataPointKey + " AND ";
            sql2 += "ep.pkey = epv.parameterkey AND et.name='DataPoint' AND ep.name='EngineeringUnits'";

            SimpleDb::Buffer buffer2;
            columns = 1;
            db.Exec( sql2.c_str(), columns, buffer2 );

            if (buffer2.empty())
            {
	            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"No parameter values found for EngineeringUnits.");
	            //return;
            }
            else if( buffer2[0][4].size() > 0 )
	        {
		        dataPoint.engUnits = buffer2[0][0];
	        }
        }

            // Get the EngineeringLimitLow parameter separately because this can be empty
            std::string sql3("");
            sql3 = "SELECT epv.value value	FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv ";
            sql3 += "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND e.pkey = epv.entitykey AND e.pkey = ";
            sql3 += dataPointKey + " AND ";
            sql3 += "ep.pkey = epv.parameterkey AND et.name='DataPoint' AND ep.name='EngineeringLimitLow'";

            SimpleDb::Buffer buffer3;
            columns = 1;
            db.Exec( sql3.c_str(), columns, buffer3 );

            if (buffer3.empty())
            {
	            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"No parameter values found for EngineeringLimitLow.");
	            //return;
            }
            else if( buffer3[0][0].size() > 0 )
	        {
		        dataPoint.engLow = atof( buffer3[0][0].c_str() );
	        }

            // Get the EngineeringLimitHigh parameter separately because this can be empty
            std::string sql4("");
            sql4 = "SELECT epv.value value	FROM ENTITYTYPE et, ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv ";
            sql4 += "WHERE et.pkey = e.typekey AND et.pkey = ep.typekey AND e.pkey = epv.entitykey AND e.pkey = ";
            sql4 += dataPointKey + " AND ";
            sql4 += "ep.pkey = epv.parameterkey AND et.name='DataPoint' AND ep.name='EngineeringLimitHigh'";

            SimpleDb::Buffer buffer4;
            columns = 1;
            db.Exec( sql4.c_str(), columns, buffer4 );

            if (buffer4.empty())
            {
	            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"No parameter values found.");
	            //return;
            }
            else if( buffer4[0][0].size() > 0 )
	        {
		        dataPoint.engHigh = atof( buffer4[0][0].c_str() );
	        }
	        ASSERT( dataPoint.engLow < dataPoint.engHigh );
        
        // If no units defined and it is a boolean
        else if (dataPoint.dpType == "BOOL" )
        {
	        // Make the range slightly greater so that lines at 0 and 1 don't
	        // disappear into the graph borders.
	        dataPoint.engLow = 0.0;
	        dataPoint.engHigh = 1.0;
        }

        // Modify the low and high to stack the boolean values
        if (dataPoint.dpType == "BOOL" )
        {
            // PercentSize is the size in percentage unit
	        double percentSize = 100.0/(double)numBools;
	        // The offset is the offset from the line as it needs to be off the line to
	        // avoid overlap.  The minimum size is 2 percent
	        double newOffset = __min(boolOffset + boolOffset*(int)(m_BoolCount/numBools),
							         (percentSize - 2.0)/2.0);
	        // Given the size and offset, calculate the new range
	        double desiredRange = percentSize - 2.0*newOffset;
	        // The resulting scale factor
	        double scaleFactor = 100/desiredRange;

	        // Calculate the engineering low
	        dataPoint.engLow = -((double)(m_BoolCount%numBools)*percentSize + newOffset)/desiredRange;

	        // Calculate the engineering high
	        dataPoint.engHigh = dataPoint.engLow + scaleFactor;

	        // Increment the counter
	        m_BoolCount++;
        }
        //}

  */
        LOG(SourceInfo, DebugUtil::FunctionExit, "LoadConfig");
    }


    //
    // parseStationNames
    //
    void CDataPointInfo::parseStationNames(const std::string& runParamString, std::vector<std::string>& stationNames)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "parseStationNames");

        TA_ASSERT(!runParamString.empty(), "StationName parameter string is empty");

        stationNames.clear();

        int startPos = 0;
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
        LOG(SourceInfo, DebugUtil::FunctionExit, "parseStationNames");
    }

#endif USE_OCCI


    void DataPointLoaderThread::run()
    {
        CDataPointInfo::getInstance().LoadDataPoints();

        ::PostMessage( AfxGetApp()->GetMainWnd()->m_hWnd, WM_LOADING_COMPLETE, NULL, NULL);

        m_loaded = true;
    }


    void DataPointLoaderThread::terminate()
    {
        CDataPointInfo::getInstance().stopLoad();
        m_terminated = true;
    }

}



