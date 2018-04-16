/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/OperatorTerminalDvaPlaybackModel.cpp $
  * @author Jade Lee
  * @version $Revision: #5 $
  * Last modification: $DateTime: 2013/09/30 11:36:59 $
  * Last modified by: $Author: huangjian $
  * 
  * Provides a basic model which allows playback of a single DVA message
  *     at the operators workstation (using the special assigned zone)
  *
  */


#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/OperatorTerminalDvaPlaybackModel.h"
#include "app/pa/PAManager/src/PaProgressReportSubscriber.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/PAManagerModel.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

OperatorTerminalDvaPlaybackModel::OperatorTerminalDvaPlaybackModel()
:
m_playDvaMessageProgressReportSubscriber(0)
{
}


OperatorTerminalDvaPlaybackModel::~OperatorTerminalDvaPlaybackModel()
{
    // Unsafe to leave this alive until destruction (since client
    // window is expected to be destroyed before we get here)
    if ( isPlayingDvaMessage() )
    {
        stopDvaPlayback();
    }

	if ( NULL != m_playDvaMessageProgressReportSubscriber)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Client failed to clean up properly" );
	}
}


bool OperatorTerminalDvaPlaybackModel::isPlayingDvaMessage()
{
    return 0 != m_playDvaMessageProgressReportSubscriber;
}


bool OperatorTerminalDvaPlaybackModel::processPlayDvaNotification(
    UINT wParam, bool& refStartTiming,
    TA_Base_Core::PrimitiveWrapper<TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState>* broadcastState)
{
    if (0 == m_playDvaMessageProgressReportSubscriber)
    {
        // Progress report subscriber is destroyed, nothing to respond
        // to for this notification
        if (0 != broadcastState)
        {
            broadcastState->resetValue();
        }
        return true;
    }

    // Note: Important we don't hold onto the locked packet
    // retrieved here (because we delete m_playDvaMessageProgressReportSubscriber
    // when we call stopDvaPlayback)
    const TA_Base_Bus::IPAAgentCorbaDef::BroadcastProgressUpdateData& lockedData = m_playDvaMessageProgressReportSubscriber->getProgressReport().getLockedData();
    TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState bcState = lockedData.broadcastState;
    TA_Base_Bus::IPAAgentCorbaDef::EGlobalState bgState = lockedData.globalState;

    if (0 != broadcastState)
    {
        broadcastState->setValue(bcState);
    }

    if ( TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval == bcState )
    {
        // The progress report indicates playback is complete, perform cleanup
        stopDvaPlayback();

        switch(bgState)
        {
        case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT:
            PaErrorHandler::displayError( PaErrorHandler::FEPC_HARDWARE_FAILURE );
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT:
            PaErrorHandler::displayError( PaErrorHandler::PA_PROTOCOL_FAILURE );
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FAILURE:
            break;
        default:
            PaErrorHandler::displayError( PaErrorHandler::UNSPECIFIED_ERROR );
            break;
        }

        return true;
    }
    else if ( TA_Base_Bus::IPAAgentCorbaDef::Runtime == bcState )
    {
        if ( 1 == lockedData.localProgresses.length() )
        {
            if ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_PTT_ACTIVATED == lockedData.localProgresses[0].localState )
            {
                refStartTiming = true;
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Local state is not initialized" );
        }
    }
    else
    {
        // Progress report indicates playback continuing as normal
    }

    return false;
}


bool OperatorTerminalDvaPlaybackModel::stopDvaPlayback()
{    
    if (!isPlayingDvaMessage())
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Already stopped" );
        return true;
    }

    bool success = false;

    try
    {
        std::string broadcastId = m_playDvaMessageProgressReportSubscriber->getBroadcastId();
        
        if (0 == broadcastId.length())
        {
            // This happens when someone (maybe at another terminal) deleted broadcast
            // - although the broadcast should have self-terminated before getting here..
            // In any case, it means broadcast already deleted
            success = true;
        }
        else
        {
            // terminate and remove the broadcast 
            std::vector<std::string> broadcastIds;
            broadcastIds.push_back(broadcastId);

            if (0 == PaManagerModel::terminateAndRemoveBroadcastIds(broadcastIds, 
                                                                    CachedConfig::getInstance()->getAgentObject(),
                                                                    BO_PLAYBACK_BROADCAST))
            {
                // No removal failures
                success = true;
            }
         }
    }
    catch (const TA_Base_Core::TransactiveException&)
    {       
    }
    catch (const CORBA::Exception&)
    {
    }

    if (0 != m_playDvaMessageProgressReportSubscriber)
    {
        // We catch all exceptions above - if request failed, we ignore the failure
        // (ensuring we still removing subscriber)
        // - Leave it up to the user to attempt to remove broadcast manually (if required)
        delete m_playDvaMessageProgressReportSubscriber;
        m_playDvaMessageProgressReportSubscriber = 0;
    }

    return success;
}


void OperatorTerminalDvaPlaybackModel::playDvaMessage(unsigned long messageKey, IWindow& notificationWindow)
{
    if (!PaRightsManager::getInstance().hasRightToPlaybackDvaMessage())
    {
        PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
        return;
    }

    if (isPlayingDvaMessage())
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Already playing a DVA message" );
        return;
    }

	if ( !TA_Base_Core::DatabaseKey::isValidKey(messageKey))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid input message key" );
		return;
	}
    
    CWaitCursor hourGlassCursor;
    
    TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq messages;
    messages.length(1);
    messages[0] = messageKey;

    // Play the single message as would be done for a normal broadcast
    // (with parameters to have it play immediately, once off)
    std::string broadcastId;
    CORBA_CALL_RETURN( broadcastId, CachedConfig::getInstance()->getAgentObject(), playbackStationDva, 
                                                            ( messageKey, 
															  CachedConfig::getInstance()->getLocalConsoleRecordingSourceId(), 
                                                              CachedConfig::getInstance()->getSessionId().c_str() ) );

    // Upon success instantiate a new progress report subscriber
    // to check on the progress.. once it reaches PendingRemoval, it
    // should be removed with removeBroadcast
    m_playDvaMessageProgressReportSubscriber = new
        PaProgressReportSubscriber(notificationWindow,
                                    broadcastId,
                                    reinterpret_cast<LONG>(this));
}


void OperatorTerminalDvaPlaybackModel::onStatusNameChange(const std::string from, const std::string to)
{
    if (0 != m_playDvaMessageProgressReportSubscriber)
    {
        if (from == m_playDvaMessageProgressReportSubscriber->getBroadcastId())
        {
            // Our broadcast ID is changing, refresh subscription to the new broadcast ID
            // (in doing so this will update the broadcast ID, as returned by getBroadcastId())
            // If target name is empty string, this broadcast is being removed (no more 
            // progress reports to listen out for)
            // resetSubscription handles empty length strings properly (by cancelling subscription)        
            try
            {
                m_playDvaMessageProgressReportSubscriber->resetSubscription(to);
            }
            catch (...)
            {
                LOG(SourceInfo, 
                    TA_Base_Core::DebugUtil::GenericLog,  
                    TA_Base_Core::DebugUtil::DebugError, 
                    "onStatusNameChange: error updating playback object subscription to ", to.c_str());

            }
        }
    }
}


void OperatorTerminalDvaPlaybackModel::requestTermination()
{
    if (0 != m_playDvaMessageProgressReportSubscriber)
    {
        if (m_playDvaMessageProgressReportSubscriber->getBroadcastId().length() > 0)
        {
            CWaitCursor hourGlassCursor;
        
            CORBA_CALL( CachedConfig::getInstance()->getAgentObject(), terminateBroadcast, (
                  m_playDvaMessageProgressReportSubscriber->getBroadcastId().c_str(),
                  CachedConfig::getInstance()->getSessionId().c_str() ) );

        }
    }
}
