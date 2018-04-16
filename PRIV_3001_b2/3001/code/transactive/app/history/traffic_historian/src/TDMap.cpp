/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/history/traffic_historian/src/TDMap.cpp $
  * @author Craig Grant
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: Katherine Thomson
  *
  * Implements the TDMap class which is a singleton class containing a map of
  * all the traffic stations in the system.  This class controls the monitoring
  * of the traffic stations, by telling them which statistics to log and
  * when to log them.
  *
  * NOTE: The word detector is incorrectly used in the Historian.  It actually
  * monitors traffic stations.
  *
  */ 

// Ignore STL warnings on Windows.

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include <stdio.h>
#include "DebugUtil.h"
#include "TDMap.h"
#include "GetDevices.h"
#include "DBConnection.h"

/**
  * GetInstance
  *
  * This method retrieves a pointer to the singleton TDMap object and  
  * creates the object if it does not already exist.
  *
  * @return A pointer to the TDMap object.
  *
  */
TDMap* TDMap::GetInstance()
{
    static TDMap tdMap;
    return &tdMap;
}


/**
  * ~TDMap
  * 
  * The destructor which deletes all the traffic detector objects from the map.
  *
  */
TDMap::~TDMap()
{
    TD_MAP::iterator it;
    for ( it = m_TDMap.begin(); it != m_TDMap.end(); it++ )
    {
        delete it->second;
    }
}


/**
  * RetrieveAllTrafficDetectors
  *
  * This method retrieves all the traffic stations from the database and 
  * creates a Traffic Detector object for them all.
  *
  * @return A bool which is true if the traffic stations were retrieved
  *         successfully.
  *
  */
bool TDMap::RetrieveAllTrafficDetectors()
{
    LOG(DebugUtil::DebugInfo, "Retrieve all traffic detectors from the database..." );

    // Open the database connection instance.

    DBConnection* dbc = DBConnection::Instance();
    DBDeviceView dbDeviceView( dbc->GetDBName(), dbc->GetDBName(),
                               dbc->GetUsername(), dbc->GetPassword() );
    dbDeviceView.open();
    if ( !dbDeviceView.isOpen() )
    {
        LOG( DebugUtil::DebugFatal, "Error: Cannot open database connection." );
        return false;
    }

    // Do select to return all rows.

    DBDeviceViewRow dbDeviceRow;
    bool rc = dbDeviceView.AllRows( dbDeviceRow);

    // Go through the rows and add them to the list.

    char msg[101];
    int i=0, numLanes;
    while (rc == true) // Add the last row retrieved.
    {
        numLanes = atoi( dbDeviceRow.lanes.c_str() );
        if( numLanes > 0)
        {
            // Add Traffic Detector.
            
            sprintf( msg, "Adding device: %s ( key = %d ), lanes = %d", 
                     dbDeviceRow.name.c_str(), dbDeviceRow.key, numLanes );
            LOG(DebugUtil::DebugInfo, msg );

            m_TDMap[dbDeviceRow.key] = new TrafficDetector( dbDeviceRow.key, numLanes );
        }
        else
        {
            sprintf( msg, "ERROR: Device: %s (%d) has no lanes and will be ignored.",
                     dbDeviceRow.name.c_str(), dbDeviceRow.key );
            LOG(DebugUtil::DebugWarn, msg );
        }

        // Retrieve another row.

        rc = dbDeviceView.NextRow(dbDeviceRow);
        i++;
    }

    // Close the database connection
    dbDeviceView.close();

    return true;
}



/**
  * GetTrafficDetector
  *
  * This method parses the map to retrieve a pointer to the traffic detector 
  * object with the specifed entity key.
  *
  * @return A pointer to the traffic detector object with the specifed
  *         entity key.  This will be NULL if the object cannot be found.
  *
  */
TrafficDetector* TDMap::GetTrafficDetector( long entityId )
{
    TD_MAP::iterator i = m_TDMap.find( entityId );
    if( i != m_TDMap.end() )
    {
        // Found it, return it to caller.

        return i->second;
    }

    // Not found in map, return NULL.

    return NULL;
}


/**
  * WriteStatistics
  *
  * This method parses the map to retrieve the traffic station specified by
  * the entityId.  It then passes the statistical information for that
  * traffic station to the associated traffic detector object to be written
  * to the database.
  *
  * @param The entity key of the traffic station.
  * @param The device data for that traffic station. 
  *
  */
void TDMap::WriteStatistics( const long& entityId,
				             const its::TdDeviceDmIf::TrafficDetectorDevice& tdIn )
{
    // Let the Traffic Detector class do the work.

    TrafficDetector* td = GetTrafficDetector(entityId);
    if( td )
        td->WriteStatistics( tdIn );
    else
    {
        char msg[101];
        sprintf( msg, "WriteStatistics failed: %ld not in Traffic detector map.", entityId );
        LOG(DebugUtil::DebugWarn, msg );
    }
}


// Implement StatisticsDmIf interface

/** 
  * StartCollecting
  *
  * This method tells each of the traffic detector objects specified by the
  * entity key in info to start logging the specified statistics.
  *
  * @param The sequence of traffic stations and the statistics that 
  *        should be logged.  These statistics will be added
  *        to the vector of currently active statistics.
  *
  * @exception InvalidEntityKeyException is thrown if any of the
  *            traffic station entity keys are invalid.  If info
  *            contains a mix of valid and invalid keys, the valid
  *            ones will be processed before the exception is thrown.
  *
  */
void TDMap::StartCollecting(const its::StatisticsDmIf::t_StatsInfoSeq& info)
    throw ( its::StatisticsDmIf::InvalidEntityKeyException )
{
    // Go through the traffic detectors and turn on collecting for
    // the specified detectors, lanes and data types.

    its::StatisticsDmIf::InvalidEntityKeyException invalidEntityKeyEx;
    int length = 0;
    
    for( int i = 0; i < info.length(); i++)
    {
        TrafficDetector* td = GetTrafficDetector( info[i].entityKey );
        if ( td == NULL )
        {
            invalidEntityKeyEx.entityKeys.length( length + 1 );
            invalidEntityKeyEx.entityKeys[length++] = info[i].entityKey;
            continue;
        }
        td->SetActiveStatistics( true, info[i].statistics );
    }

    // If there were any entity keys that could not be identified,
    // notify the calling process.

    if ( length != 0 )
    {
        throw invalidEntityKeyEx;
    }
}


/** 
  * StopCollecting
  *
  * This method tells each of the traffic detector objects specified by the
  * entity key in info to stop logging the specified statistics.
  *
  * @param The sequence of traffic stations and the statistics that 
  *        should no longer be logged.  These statistics will be removed
  *        from the vector of currently active statistics.
  *
  * @exception InvalidEntityKeyException is thrown if any of the
  *            traffic station entity keys are invalid.  If info
  *            contains a mix of valid and invalid keys, the valid
  *            ones will be processed before the exception is thrown.
  *
  */ 
void TDMap::StopCollecting(const its::StatisticsDmIf::t_StatsInfoSeq& info)
    throw ( its::StatisticsDmIf::InvalidEntityKeyException )
{
    // Go through the traffic detectors and turn off collecting for
    // the specified detectors, lanes and data types.

    its::StatisticsDmIf::InvalidEntityKeyException invalidEntityKeyEx;
    int length = 0;
    
    for( int i = 0; i < info.length(); i++)
    {
        TrafficDetector* td = GetTrafficDetector( info[i].entityKey );
        if ( td == NULL )
        {
            invalidEntityKeyEx.entityKeys.length( length + 1 );
            invalidEntityKeyEx.entityKeys[length++] = info[i].entityKey;
            continue;
        }
        td->SetActiveStatistics( false, info[i].statistics );
    }
   
    // If there were any entity keys that could not be identified,
    // notify the calling process.

    if ( length != 0 )
    {
        throw invalidEntityKeyEx;
    }
}


/** 
  * GetActiveStatistics
  *
  * This method gets the sequence of statistics that are being logged for a 
  * the specified traffic station.
  *
  * @param The entity key of the traffic station to retrieve the statistics for.
  *
  * @return The sequence of active statistics for the specified traffic station.
  *
  * @exception InvalidEntityKeyException is thrown if any of the
  *            traffic station entity keys are invalid.  If info
  *            contains a mix of valid and invalid keys, the valid
  *            ones will be processed before the exception is thrown.
  *
  */
its::StatisticsDmIf::t_Statistics* TDMap::GetActiveStatistics( unsigned long entityKey )
    throw ( its::StatisticsDmIf::InvalidEntityKeyException )
{
    its::StatisticsDmIf::t_EntityKeys* invalidEntityKeys = new its::StatisticsDmIf::t_EntityKeys;
    
    TrafficDetector* td = GetTrafficDetector( entityKey );
    if ( td == NULL )
    {
        invalidEntityKeys->length( 1 );
        invalidEntityKeys[0] = entityKey;
        throw its::StatisticsDmIf::InvalidEntityKeyException( entityKey );
    }
    return td->GetActiveStatistics();
}