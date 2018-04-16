/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/BroadcastWorker.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include <time.h>

#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/pa/PAAgent/src/AbstractBroadcast.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/IBroadcastManager.h"
#include "app/pa/PAAgent/src/BroadcastWorker.h"


namespace TA_IRS_App
{

// ExceptionChecked
BroadcastWorker::BroadcastWorker ( AbstractBroadcastCircularList& refObject, IBroadcastManager& refInterface ) : 
m_allBroadcasts( refObject ),
m_refIBroadcastManager( refInterface ),
m_isRunning(false),
m_isEnabled(false),
m_isChangingToControlMode(false)
{
    FUNCTION_ENTRY("BroadcastWorker");

	FUNCTION_EXIT;
}



// ExceptionChecked
BroadcastWorker::~BroadcastWorker()
{
    FUNCTION_ENTRY("~BroadcastWorker");

    if ( getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING )
    {
        terminateAndWait();
    }

	FUNCTION_EXIT;
}



// ExceptionChecked
void BroadcastWorker::run()
{
    FUNCTION_ENTRY("run");

    bool bBusy = false;
    m_isRunning = true;

    // Do not support on-line update for faster responding
    const unsigned long ulIdleTime= CachedConfig::getInstance()->getWorkerThreadsSleepTimeInMSecs();

    while (m_isRunning)
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "m_isChangingToControlMode:%d", m_isChangingToControlMode);
		if( m_isChangingToControlMode )
        {
            Thread::sleep(500);
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
void BroadcastWorker::terminate()
{
    FUNCTION_ENTRY("terminate");

    m_isRunning = false;
    // Wake up the semaphore so we can exit the run loop
    m_allBroadcasts.unBlockList();

    FUNCTION_EXIT;
}

bool BroadcastWorker::processNextBroadcast()
{
    AbstractBroadcast* broadcast = m_allBroadcasts.getNext();

    if (( NULL == broadcast ) || !m_isEnabled )
    {
        return false;
    }

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, PasHelpers::getProgressLogStr(*broadcast).c_str());

    int nState = broadcast->getBroadcastState();
    switch (nState)
    {
    case TA_Base_Bus::IPAAgentCorbaDef::Creation:
        broadcast->prepare();
        sendProgressUpdateIfChanged(*broadcast);
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::Execution:
        broadcast->execute();
        sendProgressUpdateIfChanged(*broadcast);
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::Runtime:
        broadcast->updateRuntimeState();
        sendProgressUpdateIfChanged(*broadcast);
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::Aborting:
        broadcast->aborting();
        sendProgressUpdateIfChanged(*broadcast);
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::Termination:
        broadcast->terminate();
        sendProgressUpdateIfChanged(*broadcast);
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval:
        if (shouldRemoveBroadcast(*broadcast))
        {
            std::string strId = broadcast->getBroadcastId().c_str();
            broadcast = NULL;
            m_refIBroadcastManager.delBroadcast( strId );
        }
        break;
    default:
        // CorruptInternalStateData - we will dispose of this broadcast
        std::string strId = broadcast->getBroadcastId().c_str();
        broadcast = NULL;
        m_refIBroadcastManager.delBroadcast( strId );
        break;
    }

    return m_allBroadcasts.isAnyBroadcastBusy();
}

bool BroadcastWorker::shouldRemoveBroadcast(AbstractBroadcast& broadcast)
{
    if (broadcast.getIsEventTriggered())
    {
        return true;
    }

    if (broadcast.getIsTaggedForRemoval())
    {
        return true;
    }

    switch(broadcast.getBroadcastType())
    {
    case TA_Base_Bus::IPAAgentCorbaDef::StationMusic:
        // All music (and music termination) broadcasts are removed ASAP.
    case TA_Base_Bus::IPAAgentCorbaDef::StationRecording:
        // Same goes for the recording broadcast as there is no option to retry.
        return true;
    case TA_Base_Bus::IPAAgentCorbaDef::StationDva:
    case TA_Base_Bus::IPAAgentCorbaDef::StationLive:
    case TA_Base_Bus::IPAAgentCorbaDef::TrainDva:
    case TA_Base_Bus::IPAAgentCorbaDef::TrainLive:
        // In the case of live broadcasts, these should be removed as soon as
        // the operator closes the status dialog.  In the odd occurrence that 
        // the PA Manager crashes, the terminated live broadcast will be 
        // cleaned up in 24 hours regardless.
        {
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
        }
    default:
        // CorruptInternalStateData - we will dispose of this broadcast
        return true;
    }

    return true;
}

void BroadcastWorker::sendProgressUpdateIfChanged(AbstractBroadcast& broadcast, const bool bNotifyClient )
{
    if (!broadcast.getHasProgressReportChanged())
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No changed");
        return;
    }

    if (broadcast.getIsEventTriggered())
    {
        // No updates for event triggered broadcasts required
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "No triggerred");
        return;
    }

    PasHelpers::getInstance()->sendBroadcastProgressUpdate( broadcast, bNotifyClient );

    // reset the flag
    broadcast.setHasProgressReportChanged(false);
}

void BroadcastWorker::enable( const bool bEnable )
{
    m_isEnabled = bEnable;
}

void BroadcastWorker::setModeChangingFlag(bool flag)
{
    m_isChangingToControlMode = flag;
}

} // namespace TA_IRS_App

