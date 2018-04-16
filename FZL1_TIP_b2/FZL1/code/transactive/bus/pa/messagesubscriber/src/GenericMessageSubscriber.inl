#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#pragma warning(disable:4284)   // For std::set warnings
#endif // defined _MSC_VER

//#include "bus/pa/messagesubscriber/src/GenericMessageSubscriber.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PAAgentComms_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "core/exceptions/src/ValueNotSetException.h"

using namespace TA_Base_Core;

namespace TA_IRS_Bus
{

    template <class PACKET_TYPE, class BUFFERED_TYPE>
    GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::GenericMessageSubscriber(DWORD notificationMessage, unsigned long locationKey)
    : 
    m_notificationMessage(notificationMessage),
    m_locationKey(locationKey)
    {
        FUNCTION_ENTRY("Constructor");
            
        FUNCTION_EXIT;
    }
    
    
    // ExceptionChecked
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::~GenericMessageSubscriber()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // Harmless to unsubscribe if not subscribed
            MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);                
        }
        catch (...)
        {
             LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    
        FUNCTION_EXIT;
    }
    
    
    // Verified to be threadsafe - discussions with CamR
    // ExceptionChecked
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    void GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::resetSubscription(bool broadcastFlag)
    {
        FUNCTION_ENTRY("resetSubscription");
     
        // Harmless to unsubscribe if not subscribed
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        
        // Location key = region key
        long regionKey = m_locationKey;
    
        unsigned long entityKey = 0;
    
        // This class is also used for the Radio GUIs to subscribe to the RadioTetraAgent!
        // Some of the messages are broadcast.
        if (broadcastFlag)
        {
            // Subscribe for broadcast updates from the Occ PA/RadioTETRA Agent
    	    MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage( 
                getMessageType(),	                //context
                this );				                //SpecialisedMessageSubscriber
        }
        else
        {
            // Subscribe for various updates from the local PA or RadioTETRA Agent
    	    MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
                getMessageType(),	                //context
                this,				                //SpecialisedMessageSubscriber
                entityKey,	                        //unsigned long entityKey,
     		    0,		    						//unsigned long subsystemKey,
    		    regionKey );    					//unsigned long regionKey);
        }
    
    
        // Subscription appears to have been successful
        onSubscriptionSuccessful();
    
        FUNCTION_EXIT;
    }
    
    // ExceptionChecked
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    void GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::receiveSpecialisedMessage(const CommsMessageCorbaDef&  message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");
    
        try
        {	            
            // This is how we distinguish between multiple subscriptions..
            TA_ASSERT(getMessageType().getTypeKey() == std::string(message.messageTypeKey), "Invalid message received");
            
            const PACKET_TYPE* convertedMessage = 0;
    
    	    if ( ((message.messageState) >>= convertedMessage) == 0 )
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to interpret message");
                return;
            }
    
            processMessage(*convertedMessage);
    	}    
        catch (...)
    	{
            //submitAuditMessage("Failed to process information received from PA");
    		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, "Exception thrown while receiving event");
    	}
    
        FUNCTION_EXIT;
    }
    
    
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    const BUFFERED_TYPE GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::getMessageData(DWORD id, HWND hWnd, bool bPeek)
    {
        TA_Base_Core::ThreadGuard sync(m_windowSetLock);
    
        if (m_windowHwnds.find(hWnd) == m_windowHwnds.end() )
        {
            TA_THROW(TransactiveException("This window no longer connected to subscriber"));
        }
    
        return m_messageQueue.fetchUpdate(id, hWnd, bPeek);
    }
    
    
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    void GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::processMessage(const PACKET_TYPE& updateData)
    {          
        onPreProcessMessage(updateData);
        
        // Do core processing
        queueDataAndDispatchNotification(getNotificationData(updateData));
    
        //////////////////////
        // Core processing complete
        //////////////////////
    
        onPostProcessMessage(updateData);
    }
    
    
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    void GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::queueDataAndDispatchNotification(
                        const BUFFERED_TYPE& updateData)
    {
        TA_Base_Core::ThreadGuard sync(m_windowSetLock);
         
        if (0 == m_windowHwnds.size())
        {
            return;
        }
    
        // Derived class must override this function if they choose to bundle notifications
        updateMessageQueue(m_messageQueue, m_windowHwnds, updateData);
    }  
         
    
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    void GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::updateMessageQueue(
                                            UpdateBuffer<BUFFERED_TYPE>& msgQueue,
                                            const std::set<HWND>& windowHwnds,
                                            const BUFFERED_TYPE& updateData) const
    {
        addItemToQueueAndNotifyWindows(msgQueue, windowHwnds, updateData);
    }
    
    
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    void GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::addItemToQueueAndNotifyWindows(
                                            UpdateBuffer<BUFFERED_TYPE>& msgQueue,
                                            const std::set<HWND>& windowHwnds,
                                            const BUFFERED_TYPE& updateData) const
    {
        // Unit Test TD #58: use 'windowHwnds' not m_windowHwnds
        unsigned long newItemKey = msgQueue.postUpdate(windowHwnds, updateData);
        
        // Use the non-locking implementation (since we're already locked in here)
        // To dispatch to update to all windows
        dispatchNotificationTo(newItemKey, windowHwnds);
    }
    
    
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    void GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::dispatchNotificationTo(
                                                       const DWORD key, 
                                                       const std::set<HWND> hWndSet) const
    {
        // Post the notification to all currently connected windows
        for (std::set<HWND>::const_iterator wnd = hWndSet.begin();
                wnd != hWndSet.end();
                wnd ++)
        {
            // Confirm window still exists (if user didn't detach on window destruction, we
            // will be left with windows that no longer exist)
            TA_ASSERT(IsWindow(*wnd), "Window no longer exists");
            TA_ASSERT(m_notificationMessage != 0, "Notification Message Invalid");
            ::PostMessage((*wnd), m_notificationMessage, key, 0);
        }
    }
    
    
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    void GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::addListenerWindow(HWND wnd, bool bcFlag)
    {
        TA_ASSERT(wnd != 0, "Must specify valid window");
        TA_ASSERT(IsWindow(wnd), "Window is invalid");
    
        if (addWindowToSet(wnd))
        {
            // First window added to set, may subscribe
            resetSubscription(bcFlag);
        }
    
        return;
    }
    
    
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    bool GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::removeWindowFromSet(HWND wnd)
    {
        LOG(SourceInfo, 
            TA_Base_Core::DebugUtil::GenericLog,  
            TA_Base_Core::DebugUtil::DebugInfo, 
            "removeWindowFromSet %X", wnd);
                
        TA_Base_Core::ThreadGuard sync(m_windowSetLock);
    
        std::set<HWND>::iterator it = m_windowHwnds.find(wnd);
    
        if (it == m_windowHwnds.end())
        {
            TA_THROW(TransactiveException("Window not currently connected"));
        }
    
        m_windowHwnds.erase(it);
    
        // There are some messages that haven't been fetched by this window
        if (m_messageQueue.getNumberOfQueuedUpdates(wnd) > 0)
        {
            // To avoid 'hanging' updates being left - since the listener window may
            // no longer be processing updates, we have to manually collect all the
            // REMAINING updates that have had notifications sent out to window 'wnd'
            // (the collection will in turn destroy them, clearing queue)
            m_messageQueue.clearQueuedUpdates(wnd);
        }
    
        bool isFinalWindowToBeRemoved = (0 == m_windowHwnds.size());
    
        return isFinalWindowToBeRemoved;
    }
    
    
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    bool GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::addWindowToSet(HWND wnd)
    {
        TA_Base_Core::ThreadGuard sync(m_windowSetLock);
    
        if (!m_windowHwnds.insert(std::set<HWND>::value_type(wnd)).second)
        {
            // hWnd already in the set
            TA_THROW(TransactiveException("Window already connected"));
        }
        
        // If this is the first window added, reopen subscription
        bool isFirstWindowAdded = (1 == m_windowHwnds.size());
        
        return isFirstWindowAdded;
    }
    
    
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    void GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::removeListenerWindow(HWND wnd)
    {
        TA_ASSERT(wnd != 0, "Must specify valid window");
       
        if (removeWindowFromSet(wnd))
        {
            // If no windows connected, we may unsubscribe
            MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        }
        return;
    }
    
    
    template <class PACKET_TYPE, class BUFFERED_TYPE>
    bool GenericMessageSubscriber<PACKET_TYPE, BUFFERED_TYPE>::isConnectedListenerWindow(HWND hWnd)
    {
        TA_Base_Core::ThreadGuard sync(m_windowSetLock);
    
        std::set<HWND>::iterator it = m_windowHwnds.find(hWnd);
    
        if (it == m_windowHwnds.end())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

} // end namespace TA_IRS_Bus
