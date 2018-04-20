/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/messagesubscriber/src/GenericMessageSubscriber.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * GenericMessageSubscriber
  * Multiple windows can be attached to this subscriber at once, each 
  * received their own buffered data (which must be collected)
  * The windows receive a message (defined in constructor) where wParam
  *  is the ID of the message (used as internal key - for use when
  *  calling getMessageData)
  *
  */

#if !defined(GenericMessageSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
#define GenericMessageSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_

#include <set>
#include <map>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ValueNotSetException.h"
#include "bus/pa/common/src/UpdateBuffer.h"
#include "core/message/types/PAAgentAudit_MessageTypes.h"

namespace TA_Base_Core
{
    class AuditMessageSender;
}

namespace TA_IRS_Bus
{

    template <class PACKET_TYPE, class BUFFERED_TYPE = PACKET_TYPE>
    class GenericMessageSubscriber 
    : 
    public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {
        friend class GenericMessageSubscriberTestCases;

    public:

        /**
         * GenericMessageSubscriber (constructor)
         * 
         * @param notificationMessage the identifier of the message to send to
         *      all attached windows when an update is received (some sort of WM_APP message)
         *
         * @param locationKey the location key used for subscription purposes
         *
         */
	    GenericMessageSubscriber(DWORD notificationMessage, unsigned long locationKey);

        /**
         * ~GenericMessageSubscriber (destructor)
         *
         * @pre all windows are detached before destruction
         *
         * Client should ensure there are no remaining attached windows
         * before the object gets destroyed (internal safety check)
         *
         */
	    virtual ~GenericMessageSubscriber();


        /**
         * getMessageType
         *
         * @return the message type for this subscriber (defines what messages
         *          we actually listen out for
         *
         */
        virtual const TA_Base_Core::MessageType& getMessageType() const = 0;


        /**
         * receiveSpecialisedMessage
         * 
         * Required derived function to receive messages
         * (not for external use of client code - it is public to match base class)
         *
         */
        virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef&  message);
    

        /**
         * getMessageData
         *
         * Returns the message update that is currently stored in buffer
         *
         * @param id the identifier defining the message data we want to retrieve
         *          corresponds to key the data is mapped to (and wParam of posted message)
         *
         * @param hWnd the window that received the notification update messages, this must
         *              correspond to one of the windows that this subscriber sent an update
         *              message to
         *
         * @param bPeek set to true if want to peek at the packet, but not purge map
         *          of the message returned
         *
         * @return the message data
         *      
         * @exception ValueNotSetException if the specified message data doesn't exist
         *
         * @exception TransactiveException if the specified window is no longer attached
         *           user should use function isConnectedListenerWindow to verify still connected
         *
         */
        const BUFFERED_TYPE getMessageData(DWORD id, HWND hWnd, bool bPeek = false);    

        /**
         * connectToWindow
         *
         * Connects this subscriber to the specified window - user must remove itself
         *  as the window is being shut down (with removeListenerWindow)
         *
         * @param hWnd the window to receive notification update messages     
         * @param broadcastFlag set this to true if you wish to subscribe to
         *                      broadcast messages rather than just local.
         *                      Defaults to false for backward compatibility.
         * 
         * @exception TransactiveException if this window is already connected
         *
         */
        void    addListenerWindow(HWND wnd, bool broadcastFlag=false);


        /**
         * removeListenerWindow
         *
         * Disconnects this subscriber from the specified window - should call from same thread
         *          that requests updates - that is, the GUI thread (not 100% threadsafe)
         *
         * @param hWnd the window to disconnect 
         * 
         * @exception TransactiveException if this window is not currently connected
         *
         */
        void    removeListenerWindow(HWND wnd);
    

        /**
         * isConnectedListenerWindow
         *
         * @return true if the specified window is attached and listening for updates from this subscriber
         *
         */
        bool    isConnectedListenerWindow(HWND hWnd);


        /**
         * Required for simulated message processing
         *
         */
        void DEBUG_processMessage(const PACKET_TYPE& updateData) { processMessage(updateData); }

    protected:

        /**
         * getNotificationData
         *
         * @return the set of data that should be buffered for subsequent
         *      retrieval by attached windows
         *
         * @param packetData the original packet of data
         *
         */
        virtual BUFFERED_TYPE getNotificationData(const PACKET_TYPE& packetData) = 0;

        /**
         * onPreProcessMessage
         *
         * Called whenever a data packet has been received, just before notification of receipt is sent out
         *
         * @param updateData the message that has been received
         *
         * @exception TransactiveException if there is an error (see what() for more details)
         *              NOTE: This aborts processing of packet - it will be discarded if this exception thrown
         *
         */
        virtual void onPreProcessMessage(const PACKET_TYPE& updateData) = 0;
    

        /**
         * onPostProcessMessage
         *
         * Called after notification of new data has been sent out
         *
         * @param updateData the message that has been received
         *
         * @exception TransactiveException if there is an error (see what() for more details)
         *
         */
        virtual void onPostProcessMessage(const PACKET_TYPE& updateData) = 0;
    
        /**
         * onSubscriptionSuccessful
         *
         * Calls after a new subscription has successfully been initialised
         *
         */
        virtual void onSubscriptionSuccessful() = 0;

        /**
         * queueDataAndDispatchNotification
         * 
         * Queues the data packet (for collection by windows), unless the 
         *  derived class overrides updateMessageQueue, the default behaviour is
         *  to add the new data to the queue buffer and send out notifications.
         *  However may be customised (overriden) to not send out notification
         * Derived class may call directly to use the buffering / notification system
         *
         * @pre the notification message type passed in constructor is
         *          valid
         *
         * @exception TransactiveException if there is an error (see what() for more details)
         *
         */
        void queueDataAndDispatchNotification(const BUFFERED_TYPE& updateData);
    
        /**
         * updateMessageQueue
         *
         * Function provided to give control to derived classes on how
         *  they want to update the message queue.  Should remain const
         *  because called when lock exists of windowSet (meaning other
         *  threads cannot access window or msg queue - and to avoid deadlock
         *  we cannot call non const functions)
         * NOTE: This function is responsible for sending out notifications to windows
         *  of new data added to the message queue
         *          
         * @param msgQueue the message queue to update (and buffer the data in)
         *
         * @param windowHwnds the set of windows this data needs to be associated with in queue
         *
         * @param updateData the data to place into the queue
         *
         */
        virtual void updateMessageQueue(UpdateBuffer<BUFFERED_TYPE>& msgQueue,
                                            const std::set<HWND>& windowHwnds,
                                            const BUFFERED_TYPE& updateData) const;

        /**
         * addItemToQueueAndNotifyWindows
         *
         * Adds the input data to the message queue, and dispatches a notification
         *  message to all windows defined by windowHwnds
         *
         * @param msgQueue the message queue to update (and buffer the data in)
         *
         * @param windowHwnds the set of windows this data needs to be associated with in queue
         *
         * @param updateData the data to place into the queue
         * 
         */
        void addItemToQueueAndNotifyWindows(UpdateBuffer<BUFFERED_TYPE>& msgQueue,
                                            const std::set<HWND>& windowHwnds,
                                            const BUFFERED_TYPE& updateData) const;

    private:

        /**
         * Resets subscription for this subscriber (disconnects and/or reconnects)
         *
         * @param broadcastFlag set this to true if you wish to subscribe to
         *                      broadcast messages rather than just local.
         *                      Defaults to false for backward compatibility.
         */
        void resetSubscription(bool broadcastFlag=false);

        // Copy constructor not used so made private
	    GenericMessageSubscriber( const GenericMessageSubscriber&);
        // Assignment not used so it is made private
	    GenericMessageSubscriber& operator=(const GenericMessageSubscriber&);

        /**
         * processMessage
         * 
         * Entry point for processing of the updates
         *      Calls onPreProcessMessage for any custom handling before notification sent out
         *           - If throws TransactiveException, or returns false, the main processing 
         *              is aborted and packet is completely ignored 
         *           - Calls onPostProcessMessage for any custom handling 
         *              after notification sent out
         * 
         * @param updateData the message that has been received
         *
         * @exception TransactiveException if there is an error (see what() for more details)
         *
         */
        void processMessage(const PACKET_TYPE& updateData);

        /**
         * addWindowToSet
         *
         * Adds a new window to the internal window set
         *
         * @param wnd the window to add
         *
         * @return true if this is the first window added to the set
         *
         */
        bool addWindowToSet(HWND wnd);

        /**
         * removeWindowFromSet
         *
         * Removes a window from the internal window set
         *
         * @param wnd the window to remove
         *
         * @return true if this is the final window to be removed from the set (ie set now empty)
         *
         */
        bool removeWindowFromSet(HWND wnd);

        /**
         * dispatchNotificationTo
         *
         * Dispatches notification to the specified window set
         *  using the supplied key as parameter
         *
         * @param key to send as wParam value
         *
         * @param hWndSet the set of windows to post message to
         *
         */
        void dispatchNotificationTo(const DWORD key, 
                                    const std::set<HWND> hWndSet) const;

        // The set of windows we're posting messages to
        std::set<HWND>  m_windowHwnds;
    
        // Lock on window set data
        TA_Base_Core::NonReEntrantThreadLockable     m_windowSetLock;
	        
        // Windows notification message sent out when data received
        DWORD   m_notificationMessage;

        // Buffer of all data (for all windows)
        UpdateBuffer<BUFFERED_TYPE>  m_messageQueue;
    
        unsigned long m_locationKey;
    };

} // end namespace TA_IRS_Bus

// Template class required inclusion of inline definition
#include "bus/pa/messagesubscriber/src/GenericMessageSubscriber.inl"

#endif // !defined(GenericMessageSubscriber_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
