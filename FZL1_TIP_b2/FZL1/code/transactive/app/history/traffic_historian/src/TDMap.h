/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/history/traffic_historian/src/TDMap.h $
  * @author Craig Grant
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: Katherine Thomson
  *
  * Implements the TDMap class which is a singleton class containing a map of
  * all the traffic stations in the system.  This class controls the monitoring
  * of the traffic stations, by telling them which statistics to log and
  * when to log them.
  *
  * NOTE: The word detector is incorrectly used in the Historian.  It actually
  * monitors traffic stations.  Also currently the device configuration 
  * should not change while the historian is running.  That is, the current 
  * mode of operation is that the historian (together with other programs using 
  * the configuration) will be shut down before traffic station configuration 
  * changes and restarted afterwards.
  *
  */

#ifndef TDMAP_H
#define TDMAP_H

#include <map>
#include "TrafficDetector.h"
#include "StatisticsDmIf.h"
#include "TdDeviceDmIf.h"

class TDMap
{
	typedef std::map<long, TrafficDetector*> TD_MAP;

public:
    /**
      * GetInstance
      *
      * This method retrieves a pointer to the singleton TDMap object and  
      * creates the object if it does not already exist.
      *
      * @return A pointer to the TDMap object.
      *
      */
	static TDMap* GetInstance();

    /**
      * ~TDMap
      * 
      * The destructor which deletes all the traffic detector objects from the map.
      *
      */
    ~TDMap();


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
	bool RetrieveAllTrafficDetectors();


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
	TrafficDetector* GetTrafficDetector( long entityId );


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
	void WriteStatistics( const long& entityId,
					      const its::TdDeviceDmIf::TrafficDetectorDevice& tdIn );
	

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
    void StartCollecting(const its::StatisticsDmIf::t_StatsInfoSeq& info)
        throw ( its::StatisticsDmIf::InvalidEntityKeyException );
 

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
    void StopCollecting(const its::StatisticsDmIf::t_StatsInfoSeq& info)
        throw ( its::StatisticsDmIf::InvalidEntityKeyException );


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
    its::StatisticsDmIf::t_Statistics* GetActiveStatistics( unsigned long entityKey )
        throw ( its::StatisticsDmIf::InvalidEntityKeyException );

protected:
    /**
      * TDMap
      * 
      * The standard contructor.  This is protected because the class is a singleton.
      *
      */
    TDMap(){}

private:

	// Map of traffic detector objects.

	TD_MAP m_TDMap;
};

#endif // TDMAP_H
