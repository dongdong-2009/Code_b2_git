/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/AdhocMessageRecordModel.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Class to model the process of recording an ad-hoc message
  *
  */

#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/pamanager.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/AdhocMessageRecordModel.h"
#include "app/pa/PAManager/src/IAdhocMessageRecordPage.h"
#include "app/pa/PAManager/src/PaProgressReportSubscriber.h"
#include "app/pa/PAManager/src/PAManagerModel.h"
#include "app/pa/PAManager/src/PaRightsManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AdhocMessageRecordModel::AdhocMessageRecordModel(const unsigned long pKey, 
    IAdhocMessageRecordPage& adhocMessageRecordPage) :
m_adhocMessageKey(pKey),
m_bEmergencyMsg( false ),
m_progressReportSubscriber(0),
m_adhocMessageRecordPage(adhocMessageRecordPage),
m_bRuntimeAcked( false ),
m_bPttActiveAcked( false ),
m_bRecordingFailed( false )
{
}


AdhocMessageRecordModel::~AdhocMessageRecordModel()
{
    // User should destroy attached window before destroying model, and
    // it's good practice to not rely on destructor to stop the recording
    // (because while recording, notifications sent out to attached window)
	if ( true == isRecording())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "User should stop recording prior to destruction" );
	}
}


bool AdhocMessageRecordModel::isRecording() const
{
    // If we have a valid subscriber, must still be recording
    return 0 != m_progressReportSubscriber;
}


void AdhocMessageRecordModel::beginRecording(IWindow& notificationWnd)
{
    if (!PaRightsManager::getInstance().hasRightToRecordDvaMessage())
    {
        PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
        return;
    }

    if (isRecording())
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot begin recording" );
        return;
    }
 
    CWaitCursor hourGlassCursor;

    m_bRuntimeAcked = false;
    m_bPttActiveAcked = false;
    m_bRecordingFailed = false;

    // Allow exceptions to propogate
    std::string broadcastId = "";
    unsigned short usMsgType = m_bEmergencyMsg ? 
        TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_EMERGENCY : 
        TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_NORMAL;
    CORBA_CALL_RETURN ( broadcastId, CachedConfig::getInstance()->getAgentObject(), recordAdhocMessage,
        ( CachedConfig::getInstance()->getLocalConsoleRecordingSourceId(), m_adhocMessageKey, usMsgType,
        CachedConfig::getInstance()->getSessionId().c_str() ) );
 
    // Begin listening out for updates
    m_progressReportSubscriber = new PaProgressReportSubscriber(
        notificationWnd, broadcastId, reinterpret_cast<LONG>(this));
}


bool AdhocMessageRecordModel::processRecordAdhocNotification(
    UINT wParam, bool& refStartTiming,
    TA_Base_Core::PrimitiveWrapper<TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState>* broadcastState)
{
    refStartTiming = false;

    if (0 == m_progressReportSubscriber)
    {
        // Progress report subscriber is destroyed, nothing to respond
        // to for this notification (happens when recording is stopped, but
        // a few messages still lingering and get sent through)
        if (0 != broadcastState)
        {
            broadcastState->resetValue();
        }
        return true;
    }

    // Note: Important we don't hold onto the locked packet
    // retrieved here (because we delete m_progressReportSubscriber
    // when we call stopRecording - which is origin/owner of the lock)
    const TA_Base_Bus::IPAAgentCorbaDef::BroadcastProgressUpdateData& lockedData = m_progressReportSubscriber->getProgressReport().getLockedData();
    TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState bcState = lockedData.broadcastState;

    TA_Base_Bus::IPAAgentCorbaDef::EGlobalState bgState = lockedData.globalState;

    if (0 != broadcastState)
    {
        broadcastState->setValue(bcState);
    }

    if ( TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval == bcState )
    {
        // Delete by agent automatically, should not call agent to remove broadcast
        if ( 0 != m_progressReportSubscriber )
        {
            delete m_progressReportSubscriber;
            m_progressReportSubscriber = 0;
        }

        if (TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FAILURE != bgState)
        {
            m_bRecordingFailed = true;
        }

        // The progress report indicates recording is complete, perform cleanup
        return true;
    }
    else if ( TA_Base_Bus::IPAAgentCorbaDef::Runtime == bcState )
    {
        if ( !m_bRuntimeAcked )
        {
            m_bRuntimeAcked = true;
            m_adhocMessageRecordPage.addStringToStatusWindow( s_pressPttPrompt );
        }

        if ( 1 == lockedData.localProgresses.length() )
        {
            if ( TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_PTT_ACTIVATED == lockedData.localProgresses[0].localState )
            {
                if ( !m_bPttActiveAcked )
                {
                    m_bPttActiveAcked = true;
                    m_adhocMessageRecordPage.addStringToStatusWindow( s_releasePttPrompt );
                }

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
        // Progress report indicates recording still in progress
    }

    return false;
}


bool AdhocMessageRecordModel::stopRecording()
{
    if (!isRecording())
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Not currently recording" );
        return true;
    }

    bool success = false;

    try
    {
        // terminate and remove the broadcast 
        std::vector<std::string> broadcastIds;
        broadcastIds.push_back(m_progressReportSubscriber->getBroadcastId());
    
        if (0 == PaManagerModel::terminateAndRemoveBroadcastIds(broadcastIds, 
                                                                CachedConfig::getInstance()->getAgentObject(),
                                                                BO_RECORDING_BROADCAST))
        {
            // No removal failures
            success = true;
        }
    }
    catch (const TA_Base_Core::TransactiveException&)
    {       
    }
    catch (const CORBA::Exception&)
    {
    }

    // We catch all exceptions above - if request failed, we ignore the failure
    // (ensuring we still removing subscriber)
    // - Leave it up to the user to attempt to remove broadcast manually (if required)
    delete m_progressReportSubscriber;
    m_progressReportSubscriber = 0;

    return success;
}


void AdhocMessageRecordModel::requestTermination()
{
    if (0 != m_progressReportSubscriber)
    {
        if (m_progressReportSubscriber->getBroadcastId().length() > 0)
        {
            CWaitCursor hourGlassCursor;
        
            CORBA_CALL( CachedConfig::getInstance()->getAgentObject(), terminateBroadcast, (
                m_progressReportSubscriber->getBroadcastId().c_str(),
                CachedConfig::getInstance()->getSessionId().c_str() ) );

        }
    }
}

const char* AdhocMessageRecordModel::getBroadcastId()
{
	return m_progressReportSubscriber->getBroadcastId().c_str();
}

bool AdhocMessageRecordModel::isRecordingFailed()
{
    return m_bRecordingFailed;
}

bool AdhocMessageRecordModel::isPttActivated()
{
    return m_bPttActiveAcked;
}

bool AdhocMessageRecordModel::isEmergencyMsg()
{
    return m_bEmergencyMsg;
}

void AdhocMessageRecordModel::setEmergencyMsg( const bool bEmergency )
{
    m_bEmergencyMsg = bEmergency;
}