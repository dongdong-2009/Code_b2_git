/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/CyclicBroadcastWorker.cpp $
  * @author:  Ripple
  * @version: $Revision: #8 $
  *
  * Last modification: $DateTime: 2015/04/08 14:49:45 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#include "ace/OS.h"

#include <time.h>
#include "core/utilities/src/DebugUtil.h"
#include "app/pa/PAAgent/src/GlobalStructDefine.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/AbstractBroadcast.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/NonTrainDVABroadcast.h"
#include "app/pa/PAAgent/src/ScheduledDvaHistory.h"
#include "app/pa/PAAgent/src/IBroadcastManager.h"
#include "app/pa/PAAgent/src/CyclicBroadcastWorker.h"


namespace TA_IRS_App
{

// ExceptionChecked
CyclicBroadcastWorker::CyclicBroadcastWorker ( AbstractBroadcastCircularList& refObject, 
    ScheduledDvaHistory& refCyclicDvaHistory, IBroadcastManager& refInterface ) : 
m_allBroadcasts( refObject ),
m_refIBroadcastManager( refInterface ),
m_refCyclicDvaHistory( refCyclicDvaHistory ),
m_isRunning(false),
m_isEnabled(false),
m_isChangingToControlMode(false)
{
    FUNCTION_ENTRY("CyclicBroadcastWorker");

	FUNCTION_EXIT;
}


// ExceptionChecked
CyclicBroadcastWorker::~CyclicBroadcastWorker()
{
    FUNCTION_ENTRY("~CyclicBroadcastWorker");

    if ( getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
    {
        terminateAndWait();
    }

	FUNCTION_EXIT;
}



// ExceptionChecked
void CyclicBroadcastWorker::run()
{
    FUNCTION_ENTRY("run");

    bool bBusy = false;
    m_isRunning = true;

    // Do not support on-line update for faster responding
    const unsigned long ulIdleTime= CachedConfig::getInstance()->getWorkerThreadsSleepTimeInMSecs();

    while (m_isRunning)
    {
		if( m_isChangingToControlMode )
        {
            Thread::sleep(10000);
        }

		try
		{
			// This call will block if nothing is there
			bBusy = processNextBroadcast();
		}
		catch (...)
		{
			// Not expecting any failures...
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
		}

        if ( !bBusy )
        {
            Thread::sleep ( ulIdleTime );
        }
    }

    FUNCTION_EXIT;
}


// ExceptionChecked
void CyclicBroadcastWorker::terminate()
{
    FUNCTION_ENTRY("terminate");

    m_isRunning = false;
    // Wake up the semaphore so we can exit the run loop
    m_allBroadcasts.unBlockList();

    FUNCTION_EXIT;
}

bool CyclicBroadcastWorker::processNextBroadcast()
{
    AbstractBroadcast* broadcast = m_allBroadcasts.getNext();

    if (( NULL == broadcast ) || !m_isEnabled )
    {
        return false;
    }

    int nState = broadcast->getBroadcastState();
    switch (nState)
    {
    case TA_Base_Bus::IPAAgentCorbaDef::Creation:
        {
            broadcast->prepare();
            sendProgressUpdateIfChanged(*broadcast);
            break;
        }
    case TA_Base_Bus::IPAAgentCorbaDef::Execution:
        {
            broadcast->execute();
            sendProgressUpdateIfChanged(*broadcast);

            // Check whether execution state changed
            if ( TA_Base_Bus::IPAAgentCorbaDef::Execution != broadcast->getBroadcastState() )
            {
                if ( !broadcast->getWasExecutedSuccessfully() )
                {
                    break;
                }

                TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* pConfigCopy = broadcast->getBroadcastConfigCopy();
                if ( NULL == pConfigCopy )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unexpected NULL config copy" );
                    break;
                }

                ScheduleDvaConfig stConfig;
				stConfig.strScheuleName = broadcast->getBroadcastId();

                PasHelpers::getInstance()->corbaConfigToStdConfig( pConfigCopy->theBroadcastConfigSpecifics.theStationDvaConfig(), stConfig );

                delete pConfigCopy;
                pConfigCopy = NULL;

                m_refCyclicDvaHistory.storeScheduledDva( stConfig );
            }
            break;
        }
    case TA_Base_Bus::IPAAgentCorbaDef::Runtime:
        {
            broadcast->updateRuntimeState();
            sendProgressUpdateIfChanged(*broadcast);
            break;
        }
    case TA_Base_Bus::IPAAgentCorbaDef::Aborting:
        {
            broadcast->aborting();
            // Do not update zone state until state turn into termination
            sendProgressUpdateIfChanged(*broadcast);

            // Check whether aborting state changed
            if ( TA_Base_Bus::IPAAgentCorbaDef::Aborting != broadcast->getBroadcastState() )
            {
                if ( !broadcast->getWasExecutedSuccessfully() )
                {
                    break;
                }

                TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* pConfigCopy = broadcast->getBroadcastConfigCopy();
                if ( NULL == pConfigCopy )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unexpected NULL config copy" );
                    break;
                }

                unsigned short usEventId = pConfigCopy->theBroadcastConfigSpecifics.theStationDvaConfig().stCyclicParams.usScheduleId;
                delete pConfigCopy;
                pConfigCopy = NULL;

                m_refCyclicDvaHistory.removeScheduledDva( usEventId );
            }
            break;
        }
    case TA_Base_Bus::IPAAgentCorbaDef::Termination:
        {
            broadcast->terminate();
            sendProgressUpdateIfChanged(*broadcast);
            break;
        }
    case TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval:
        {
            if (shouldRemoveBroadcast(*broadcast))
            {
                std::string strId = broadcast->getBroadcastId().c_str();
                broadcast = NULL;
                m_refIBroadcastManager.delBroadcast( strId );
            }
            break;
        }
    default:
        {
            // CorruptInternalStateData - we will dispose of this broadcast
            std::string strId = broadcast->getBroadcastId().c_str();
            broadcast = NULL;
            m_refIBroadcastManager.delBroadcast( strId );
            break;
        }
    }

    return m_allBroadcasts.isAnyBroadcastBusy();
}

bool CyclicBroadcastWorker::shouldRemoveBroadcast(AbstractBroadcast& broadcast)
{
    if (broadcast.getIsEventTriggered())
    {
        return true;
    }

    if (broadcast.getIsTaggedForRemoval())
    {
        return true;
    }

    time_t now = time(NULL);
    unsigned long howOld = static_cast<unsigned long>(now - broadcast.getLastUpdate());
    if (howOld > CachedConfig::getInstance()->getKeepTerminatedBroadcastsInSecs())
    {
        return true;
    }
    else
    {
        return false;
    }

    return true;
}

void CyclicBroadcastWorker::sendProgressUpdateIfChanged(AbstractBroadcast& broadcast, const bool bNotifyClient )
{
    if (!broadcast.getHasProgressReportChanged())
    {
        return;
    }

    if (broadcast.getIsEventTriggered())
    {
        // No updates for event triggered broadcasts required
        return;
    }

    PasHelpers::getInstance()->sendBroadcastProgressUpdate( broadcast, bNotifyClient );

    // reset the flag
    broadcast.setHasProgressReportChanged(false);
}

void CyclicBroadcastWorker::enable( const bool bEnable )
{
    m_isEnabled = bEnable;
}

void CyclicBroadcastWorker::setModeChangingFlag(bool flag)
{
    m_isChangingToControlMode = flag;
}

} // namespace TA_IRS_App

