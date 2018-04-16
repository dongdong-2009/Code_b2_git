/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/DataPointQualityController.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include <algorithm>

#include "ace/Guard_T.h"
#include "ace/OS.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasConnectionAgency.h"
#include "app/pa/PAAgent/src/IPasPollingObserver.h"
#include "app/pa/PAAgent/src/DataPointQualityController.h"

namespace TA_IRS_App
{

DataPointQualityController::DataPointQualityController( 
    ACE_RW_Mutex& refDataPointsListLock, std::vector<TA_Base_Bus::DataPoint*>& refDataPoints ) : 
TA_Base_Core::Thread(),
m_refDataPointsListLock( refDataPointsListLock ),
m_refDataPoints( refDataPoints ),
m_mtxUpdatesLock(),
m_lstQualityUpdates(),
m_bRunning( false ),
m_bLatestQualityState( false ),
m_bForceUpdate( false ),
m_smaUpdateRequest( 0 )
{
    start();
}

DataPointQualityController::~DataPointQualityController()
{
    if ( getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
    {
        terminateAndWait();
    }
}

void DataPointQualityController::run()
{
    m_bRunning = true;

    while ( m_bRunning )
    {
        try
        {
            m_smaUpdateRequest.acquire();

            if ( !m_bRunning )
            {
                break;
            }

            processLatestUpdate();
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
                "Caught unknown exception when PasTransactionManager running" );
        }
    }
}

void DataPointQualityController::terminate()
{
    m_bRunning = false;
    m_smaUpdateRequest.release();
}

void DataPointQualityController::processLatestUpdate()
{
    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxUpdatesLock );
        if ( m_lstQualityUpdates.empty() )
        {
            return;
        }
    }

    {
        ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxUpdatesLock );

        // Save old state and get new state
        bool bOldState = m_bLatestQualityState;
        m_bLatestQualityState = m_lstQualityUpdates.back();

        m_lstQualityUpdates.clear();

        // Check the latest state with previous latest state
        if (( m_bLatestQualityState == bOldState ) && !m_bForceUpdate )
        {
            return;
        }

        m_bForceUpdate = false;
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "Connection Update to %d, begin to update DataPoints quality", m_bLatestQualityState );

    if ( m_bLatestQualityState )
    {
        ACE_Guard<ACE_RW_Mutex> guard( m_refDataPointsListLock );

        for ( std::vector<TA_Base_Bus::DataPoint*>::iterator itLoop = m_refDataPoints.begin(); m_refDataPoints.end() != itLoop; ++itLoop )
        {
            if ( NULL == *itLoop )
            {
                continue;
            }

            if ( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON != (*itLoop)->getFieldValueQualityStatus() )
            {
                TA_Base_Bus::DpValue& dpValue = (*itLoop)->getCurrentFieldValue();
                TA_Base_Bus::DataPointState dpsNewState( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

                if ( 0 == dpValue.getValueAsString().compare( TA_Base_Bus::QUALITY_BAD_VALUE_UNKNOWN_STRING ) )
                {
                    (*itLoop)->setFieldValue( dpsNewState );
                }
                else
                {
                    // Ensure once quality good, the alarm / event could be raised if needed.
                    (*itLoop)->updateFieldState( dpsNewState );
                }

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[DataPoint] Update DataPoint %s quality to %d", 
                    (*itLoop)->getDataPointName().c_str(), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
            }
        }
    }
    else
    {
        ACE_Guard<ACE_RW_Mutex> guard( m_refDataPointsListLock );

        for ( std::vector<TA_Base_Bus::DataPoint*>::iterator itLoop = m_refDataPoints.begin(); m_refDataPoints.end() != itLoop; ++itLoop )
        {
            if ( NULL == *itLoop )
            {
                continue;
            }

            if ( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON == (*itLoop)->getFieldValueQualityStatus() )
            {
                TA_Base_Bus::DataPointState dpsNewState( (*itLoop)->getCurrentFieldValue(), 
                    time( NULL ), TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE );
                (*itLoop)->updateFieldState( dpsNewState );

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[DataPoint] Update DataPoint %s quality to %d", 
                    (*itLoop)->getDataPointName().c_str(), TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE );
            }
            else if ( TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED != (*itLoop)->getFieldValueQualityStatus() )
            {
                TA_Base_Bus::DataPointState dpsNewState( (*itLoop)->getCurrentFieldValue(), 
                    time( NULL ), TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED );
                (*itLoop)->updateFieldState( dpsNewState );

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[DataPoint] Update DataPoint %s quality to %d", 
                    (*itLoop)->getDataPointName().c_str(), TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED );
            }
        }
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Updating DataPoints quality finished" );
}

void DataPointQualityController::connectionUpdated( bool bConnectionAvailable )
{
    {
        ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxUpdatesLock );
        m_lstQualityUpdates.push_back( bConnectionAvailable );
    }

    m_smaUpdateRequest.release();
}

void DataPointQualityController::enable( bool bEnable )
{
    if ( bEnable )
    {
        m_bForceUpdate = true;
    }
}

}
