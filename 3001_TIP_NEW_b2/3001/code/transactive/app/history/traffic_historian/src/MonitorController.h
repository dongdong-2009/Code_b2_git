/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/history/traffic_historian/src/MonitorController.h $
  * @author  Katherine Thomson
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: 
  *
  * The CMonitorController object processes CORBA requests to start and stop
  * logging of specific statistics for the traffic stations monitored by 
  * the Historian.
  *
  */

#ifndef MONITOR_CONTROLLER_H
#define MONITOR_CONTROLLER_H

#include "StatisticsDmIf.h"
#include "NamedObjRefBase.h"
#include "EventConsumer.h"

class Historian;

class CMonitorController : public POA_its::StatisticsDmIf  
{
public:
    /** 
      * CMonitorController
      *
      * This method registers this object as a CORBA object so that it can process
      * CORBA requests.
      *
      */
	CMonitorController();

    
    /** 
      * ~CMonitorController
      *
      * This method is the standard destructor.
      *
      */
	virtual ~CMonitorController();


    // Implement StatisticsDmIf interface

    /** 
      * StartCollecting
      *
      * This method processes CORBA requests to start logging the specified 
      * statistics for the traffic stations that are listed.
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
      * This method processes CORBA requests to stop logging the specified 
      * statistics for the traffic stations that are listed.
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
      * This method processes CORBA requests to get the sequence of statistics
      * that are being logged for a particular traffic station.
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
};

#endif // !defined(MONITOR_CONTROLLER_H)
