/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/history/traffic_historian/src/TrafficDetector.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: 
  *
  * This class implements a traffic station (incorrectly named a detector!). 
  * It handles all the processing of statistical information for a particular
  * traffic station
  *
  * NOTE: The word detector is incorrectly used in the Historian.  It actually
  * monitors traffic stations.
  *
  */

// Ignore STL warnings on Windows.

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include "DebugUtil.h"
#include "Historian.h"
#include "TrafficDetector.h"
#include "HistoryParams.h"
#include "DBHistory.h"

/** 
  * TrafficDetector
  *
  * The standard constructor.
  *
  */
TrafficDetector::TrafficDetector() 
    : m_EntityId( 0 ),
      m_numLanes( 0 )
{
    // Do nothing.
}


/** 
  * TrafficDetector
  *
  * The constructor that initialises the entityId, number of lanes and
  * the vector of active statistics.  
  *
  * @param The entity key of the traffic station (detector).
  * @param The number of lanes for the traffic station.
  *
  */
TrafficDetector::TrafficDetector( long entityId, unsigned short numLanes )
    : m_EntityId( entityId ),
      m_numLanes( numLanes )
{
   	//  Logging statistics should be initially inactive. 
   	/*  
	m_activeStatistics.push_back("Headway");
    m_activeStatistics.push_back("Occupancy");
    m_activeStatistics.push_back("Speed");
    m_activeStatistics.push_back("VolumeType1");
    m_activeStatistics.push_back("VolumeType2");
    m_activeStatistics.push_back("VolumeType3");
	*/
}


/**
  * WriteStatistics
  *
  * This method parses the device data to extract the statistical information
  * for each lane.  It then logs the data one lane at a time to the database.
  *
  * @param The device data for that traffic station. 
  *
  */
void TrafficDetector::WriteStatistics( const its::TdDeviceDmIf::TrafficDetectorDevice& tdIn )
{
    // Get the number of lanes and detectors.  If they don't match,
    // then ignore the data.
    const unsigned short numDetectors = tdIn.detectors.length();
    if( numDetectors != m_numLanes )
    {
        char lvBuffer[ 257 ];
        sprintf( lvBuffer, "Expecting %d detectors, however received %d.  Ignoring data.",
                 numDetectors, m_numLanes );
        LOG( DebugUtil::DebugWarn, lvBuffer );
        return;
    }

    // Get the current time from the system as time_t 
    // Convert the time to a string of the form 'DD/MM/YYYY HH24:MI:SS'
    // suitable for inserting into the database.
    time_t dbWriteTime;
    time( &dbWriteTime );
    struct tm* currTimeTM;
    currTimeTM = localtime( &dbWriteTime ); // Convert to local time.
    char sysDate[101];
    strftime( sysDate, 100, "%d/%m/%Y %H:%M:%S", currTimeTM );

    // Write the new statistics to the database.  
    for( int lane = 0; lane < m_numLanes; lane++ )
    {
	// The lane numbers start from 1, so we need to add one to the
	// lane to get the right number.
        WriteLaneStatistics( lane + 1, tdIn.detectors[lane], sysDate );    
    }
}


/**
  * WriteLaneStatistics
  *
  * Write the statistics for the specified lane to the database.
  * The date is passed in, because the statistics for all lanes will arrive
  * at the same time, so they should be logged for the same time.
  *
  * @param The lane number.
  * @param The statistical data for that lane. 
  * @param The date/time the data is logged at.
  *
  */
void TrafficDetector::WriteLaneStatistics( const int lane, 
                                           const its::TdDeviceDmIf::Detector& statistics, 
                                           const char* date )
{
    CDBHistory* dbHistory = CDBHistory::Instance();
    CTDHistoryParams* tdHistParams = CTDHistoryParams::Instance();
    CTDHistoryParams::VarPos statName;
    unsigned short statValue;
    
    // Only log the statistics for which logging is currently turned on.

    std::vector<std::string>::const_iterator it;
    for ( it = m_activeStatistics.begin(); it != m_activeStatistics.end(); it++ )
    {
        if ( *it == "Headway" )
        {
            statName = CTDHistoryParams::HEADWAY;
            statValue = statistics.headway;
        }
        else if ( *it == "Occupancy" )
        {
            statName = CTDHistoryParams::OCCUPANCY;
            statValue = statistics.occupancy;
        }
        else if ( *it == "Speed" )
        {
            statName = CTDHistoryParams::SPEED;
            statValue = statistics.speed;
        }
        else if ( ( *it == "VolumeType1" ) || ( *it == "Volume Type 1" ) )
        {
            statName = CTDHistoryParams::VOLUMETYPE1;
            statValue = statistics.volumeType1;
        }
        else if ( ( *it == "VolumeType2" ) || ( *it == "Volume Type 2" ) )
        {
            statName = CTDHistoryParams::VOLUMETYPE2;
            statValue = statistics.volumeType2;
        }
        else if ( ( *it == "VolumeType3" ) || ( *it == "Volume Type 3" ) )
        {
            statName = CTDHistoryParams::VOLUMETYPE3;
            statValue = statistics.volumeType3;
        }
        else
        {
            std::string log = "Invalid active statistic stored: " + (*it); 
            LOG( DebugUtil::DebugWarn, log.c_str() );
            continue;
        }

        // Statistics are logged by their primary key from the 
        // ENTITYHISTORYPARAMS table.

        const int statKey = tdHistParams->GetKeyValue( statName );

        // If it is a valid active statistic, write it to the database.

        if ( statKey >= 0 )
        {
            dbHistory->WriteHistoryRecord( date, m_EntityId, statKey,
                                           lane, statValue);
        }
    }
}


/** 
  * SetActiveStatistics
  *
  * This method adds or removes statistics from the vector of active statistics
  * depending on the value of isCollecting.
  *
  * @param A bool which if true means the statistics passed in should be added
  *        to the active statistics vector.  If it is false, the statistics should
  *        be removed from the vector.
  *  
  * @param The names of the statistics to add/remove.
  *
  */
void TrafficDetector::SetActiveStatistics( bool isCollecting, 
                                           const its::StatisticsDmIf::t_Statistics& stats )
{
    char entityIdStr[20];
    sprintf( entityIdStr, "%ld", m_EntityId );
    std::string log;
    if ( isCollecting )
    {
        log = "Adding statistics for ";
    }
    else
    {
        log = "Removing statistics from ";
    }
    log += "collection for " + std::string( entityIdStr ) + ":";


    // Prevent these structs from being changed while data is being logged.
    MiThreadGuard threadGuard( m_securityGuard );
        
    // Add each statistic from stats to the vector of active statistics
    // if isCollecting is true or remove them if isCollecting is false.
    // If a statistic is not listed in stats, do not change its current status.
    std::vector<std::string>::iterator it;
    for ( int i = 0; i < stats.length(); i++ )
    {
        for ( it = m_activeStatistics.begin(); it != m_activeStatistics.end(); it++ )
        {
            if ( *it == stats[i].in() )
            {
                // Remove the statistic if isCollecting is false.
                if ( !isCollecting )
                {
                    m_activeStatistics.erase( it );
                }
                break;
            }
        }

        // Add the statistic if isCollecting is true and it is not already active.
        if ( ( it == m_activeStatistics.end() ) && isCollecting )
        {
            m_activeStatistics.push_back( stats[i].in() );
        }

        log += "\n" + std::string( stats[i].in() );
    }

    LOG( DebugUtil::DebugDebug, log.c_str() );

	log = "Current active statistics for " + std::string( entityIdStr ) + ":\n"; 
    for ( it = m_activeStatistics.begin(); it != m_activeStatistics.end(); it++ )
    {
		log += (*it);
		log += "\n";
    }
    LOG(DebugUtil::DebugDebug, log.c_str() );
}


/** 
  * GetActiveStatistics
  *
  * This method returns the sequence of statistics that are being logged for a 
  * this traffic station.
  *
  * @return The sequence of active statistics for the this traffic station.
  *
  */
its::StatisticsDmIf::t_Statistics* TrafficDetector::GetActiveStatistics( ) const
{
    its::StatisticsDmIf::t_Statistics* stats = new its::StatisticsDmIf::t_Statistics;
    
    // Copy the vector of active statistics into stats.
    (*stats).length(m_activeStatistics.size());
    
    std::vector<std::string>::const_iterator it;
    int i = 0;
    for ( it = m_activeStatistics.begin(); it != m_activeStatistics.end(); it++ )
    {
        (*stats)[i++] = CORBA::string_dup( (*it).c_str() );
    }

    return stats;
}
