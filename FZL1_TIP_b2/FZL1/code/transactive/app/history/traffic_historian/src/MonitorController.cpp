/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/history/traffic_historian/src/MonitorController.cpp $
  * @author  Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: 
  *
  * The CMonitorController object processes CORBA requests to start and stop
  * logging of specific statistics for the traffic stations monitored by 
  * the Historian.
  *
  */ 

// Ignore STL warnings on Windows.

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include "MonitorController.h"
#include "TDMap.h"
#include "RunParams.h"
#include "CorbaUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** 
  * CMonitorController
  *
  * This method registers this object as a CORBA object so that it can process
  * CORBA requests.
  *
  */
CMonitorController::CMonitorController()
{
    std::string registerName;
    registerName = RunParams::GetInstance().Get("SubsystemName");
    registerName+= "/";
    registerName+= RunParams::GetInstance().Get("TypeName");
    registerName+= "/";
    registerName+= RunParams::GetInstance().Get("EntityName");

    CorbaUtil::GetInstance().RegisterName( this->_this(), registerName.c_str() );
}


/** 
  * ~CMonitorController
  *
  * This method is the standard destructor.
  *
  */
CMonitorController::~CMonitorController()
{
    // Do nothing.
}


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
void CMonitorController::StartCollecting(const its::StatisticsDmIf::t_StatsInfoSeq& info)
    throw ( its::StatisticsDmIf::InvalidEntityKeyException )
{
    TDMap::GetInstance()->StartCollecting( info );
}


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
void CMonitorController::StopCollecting(const its::StatisticsDmIf::t_StatsInfoSeq& info)
    throw ( its::StatisticsDmIf::InvalidEntityKeyException )
{
    TDMap::GetInstance()->StopCollecting( info );
}


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
its::StatisticsDmIf::t_Statistics* CMonitorController::GetActiveStatistics( unsigned long entityKey )
    throw ( its::StatisticsDmIf::InvalidEntityKeyException )
{
    return TDMap::GetInstance()->GetActiveStatistics( entityKey );
}