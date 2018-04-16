/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/PAProgressReportSubscriber.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * PaProgressReportSubscriber bridges communications between the PA Manager 
  * and CORBA broadcasts
  */


#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "app/pa/PAManager/src/StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PAAgentComms_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/PaProgressReportSubscriber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ExceptionChecked
PaProgressReportSubscriber::PaProgressReportSubscriber(IWindow& pageView, const std::string& broadcastId, LONG lParam) : 
m_receivingView(pageView),
m_notificationAcknowledged(true),
m_lParamValue(lParam)
{
    FUNCTION_ENTRY("Constructor");

    // Begin subscription immediately
    try
    {
        resetSubscription(broadcastId);
    }
    catch (const std::exception& expWhat)
    {
        // If unable to subscribe, do so at later date   
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
   
    FUNCTION_EXIT;
}

// ExceptionChecked
PaProgressReportSubscriber::~PaProgressReportSubscriber()
{
    FUNCTION_ENTRY("Destructor");

    TA_Base_Core::ThreadGuard sync(m_packetDataAndNotificationLock);

    // This is not a safe condition, we were still posting notifications to this window
	if ( !::IsWindow(m_receivingView.getHWnd()))
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Window no longer valid while in destructor" );
		FUNCTION_EXIT;
		return;
	}

    try
    {
        resetSubscription("");
    }        
    catch (...)
    {
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught in destructor");
    }

    FUNCTION_EXIT;
}


// Verified to be threadsafe - discussions with CamR
// ExceptionChecked
void PaProgressReportSubscriber::resetSubscription(const std::string& broadcastId)
{
    FUNCTION_ENTRY("resetSubscription");

    m_broadcastId = broadcastId;

    // Harmless to unsubscribe if not subscribed
	TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    // Only need to resubscribe provided a valid name exists (if we take this code
    // out, a crash occurs during stopping of DVA playbacks, found while resolving TD #2966)
    if (!broadcastId.empty())
    {
        // Location key = region key
        long regionKey = CachedConfig::getInstance()->getAgentLocKey();

        TA_Base_Core::NameValuePair nvp( TA_Base_Bus::IPAAgentCorbaDef::BROADCAST_ID_FILTER_STRING,
            TA_Base_Core::NameValuePair::EQUALS, broadcastId );
    
        std::vector<TA_Base_Core::NameValuePair*> extraFilterableData;
        extraFilterableData.push_back(&nvp);
    
        // Subscribe for updates from the local PA Agent
		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
            TA_Base_Core::PAAgentComms::BroadcastProgressUpdate,	               //context
            this,				                                    //SpecialisedMessageSubscriber
            0,	                                                    //unsigned long entityKey,
     		0,		    											//unsigned long subsystemKey,
    		regionKey,                                              //unsigned long regionKey);
            &extraFilterableData);    								//broadcastId
    }

    FUNCTION_EXIT;
}


// ExceptionChecked
void PaProgressReportSubscriber::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef&  message)
{
    FUNCTION_ENTRY("receiveSpecialisedMessage");

    try
    {	    
        const TA_Base_Bus::IPAAgentCorbaDef::BroadcastProgressUpdateData* progressReport = 0;

	    if ( ((message.messageState) >>= progressReport) == 0 )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to interpret message");
            return;
        }

        processProgressReport(*progressReport);
	}    
    catch (...)
	{
        //submitAuditMessage("Failed to process information received from PA");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception thrown while receiving event");
	}

    FUNCTION_EXIT;
}


void PaProgressReportSubscriber::forceProgressUpdate(PAAgentNamedObject& paAgent)
{
    FUNCTION_ENTRY("forceProgressUpdate");

    if (!m_broadcastId.empty())
    {
        CWaitCursor hourGlassCursor;
        // Call the agent directly, and pass result directly to process function
        // it will be treated identically to a report that was received asynchronously
        // The function is thread safe, so it's safe to call this forceProgressUpdate from
        // a separate thread - that's where it'll usually be called from..
        TA_Base_Bus::IPAAgentCorbaDef::BroadcastProgressUpdateData* ret = NULL;
        CORBA_CALL_RETURN( ret, paAgent, getProgressReport, ( m_broadcastId.c_str() ) );
        std::auto_ptr<TA_Base_Bus::IPAAgentCorbaDef::BroadcastProgressUpdateData> data( ret );
    
        processProgressReport(*data.get());
    }

    FUNCTION_EXIT;
}


void PaProgressReportSubscriber::processProgressReport(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastProgressUpdateData& progressReport)
{    
    FUNCTION_ENTRY("processProgressReport");
    TA_Base_Core::ThreadGuard sync(m_packetDataAndNotificationLock);

    // We will make a local copy of the message, if for whatever reason
    // the WM_PA_STN_STATUS_REPORT_UPDATE is not acknowledged, the packet will
    // be dropped (and overwritten) - fortunately we don't have to worry about this
    // because each packet provides full refresh information (we can just read
    // the latest packet and be up to date)   
    m_progressReportCachedData.setValue(progressReport);

    // Post the relevant message to the window that constructed this subscriber
    // The local copy has been created and is now ready for processing by the thread
    // that created this subscribe

    // Only post the notification if the last notification has been responded to
    if (m_notificationAcknowledged)
    {
        if (0 != m_receivingView.getHWnd() && ::IsWindow(m_receivingView.getHWnd()))
        {
            // Invalidate the acknowledged state, now that a new process request has been issued
            m_notificationAcknowledged = false;
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Posting message to window %d", m_receivingView.getHWnd());

            ::PostMessage(m_receivingView.getHWnd(), WM_PA_STN_STATUS_REPORT_UPDATE, 0, m_lParamValue);
        }
        else
        {
            // User didn't destroy subscriber before destroying attached window
            // The client code needs to be fixed
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "pecified window does not exist" );
        }
    }
    else
    {
        // We get in here if the update requests are not being processed fast enough by the client
        // That is, we're receiving updates over the network faster than they can be processed        
        // All that happens is we update the to the latest packet, and processing of current packet
        // doesn't occur
    }

    FUNCTION_EXIT;
}

        
const LockedPacketProgressReport PaProgressReportSubscriber::getProgressReport(bool bPeek)
{
    FUNCTION_ENTRY("getProgressReport");
    // Acquire the locked progress report immediately, initiating the lock on m_packetDataAndNotificationLock
    LockedPacketProgressReport lockedProgressReport(m_packetDataAndNotificationLock, m_progressReportCachedData.getValue());

    // Acknowledge the message, to ensure we don't queue up 
    // message arrived notifications if we're slow to respond
    if (!bPeek)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Collecting message for window %d", m_receivingView.getHWnd());

        // If notification already acknowledged, means the update was attempted
        // to be retrieved twice, happens when two 

        m_notificationAcknowledged = true;       
    }

    return lockedProgressReport;
}
