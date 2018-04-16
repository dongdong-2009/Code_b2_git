/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/history/traffic_historian/src/TrafficDetector.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
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


#ifndef TRAFFIC_DETECTOR_H
#define TRAFFIC_DETECTOR_H

#include <vector>
#include <string>
#include <time.h>

#include <CosEventComm.hh>
#include "TdDeviceDmIf.h"
#include "StatisticsDmIf.h"
#include "MiThreadLockable.h"

class TrafficDetector
{
public:
    /** 
      * TrafficDetector
      *
      * The standard constructor.
      *
      */
	TrafficDetector();

    /** 
      * TrafficDetector
      *
      * The constructor that initialises the entityId, number of lanes and
      * the vector of active statistics.  Note: That all statistics are
      * initially active.
      *
      * @param The entity key of the traffic station (detector).
      * @param The number of lanes for the traffic station.
      *
      */
    TrafficDetector( long entityId, unsigned short numLanes );


    /** 
      * ~TrafficDetector
      *
      * The standard destructor.
      *
      */
    ~TrafficDetector(){}
    

    /**
      * WriteStatistics
      *
      * This method parses the device data to extract the statistical information
      * for each lane.  It then logs the data one lane at a time to the database.
      *
      * @param The device data for that traffic station. 
      *
      */
    void WriteStatistics( const its::TdDeviceDmIf::TrafficDetectorDevice& tdIn );


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
    void WriteLaneStatistics( const int lane, 
                              const its::TdDeviceDmIf::Detector& statistics, 
                              const char* date );


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
    void SetActiveStatistics( bool isCollecting, 
                              const its::StatisticsDmIf::t_Statistics& stats );

    
    /** 
      * GetActiveStatistics
      *
      * This method returns the sequence of statistics that are being logged for a 
      * this traffic station.
      *
      * @return The sequence of active statistics for the this traffic station.
      *
      */
    its::StatisticsDmIf::t_Statistics* GetActiveStatistics() const;
   
private:

  	// The entity key id for this device.

	long m_EntityId;

    // The number of lanes (detectors) for this traffic station.

    unsigned short m_numLanes;

    // Specifies which data should be collected and logged for
    // which lanes of this traffic station.

    std::vector<std::string> m_activeStatistics;

    // Used to guard the m_activeStatistics structure while
    // the data is being logged.

    MiThreadLockable m_securityGuard;
};

#endif  // TRAFFIC_DETECTOR_H
