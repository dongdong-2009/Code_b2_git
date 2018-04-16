/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(RadioMessageProcessor_H)
#define RadioMessageProcessor_H

#include "app/trains/train_agent/src/IRadioMessageProcessor.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <vector>
#include <map>

// forward declarations
namespace TA_Base_Core
{
    class ThreadPoolSingletonManager;
}

namespace TA_IRS_Bus
{
    class IResponseReceiver;
}

namespace TA_IRS_App
{
    class ITrainStatusManager;
    class ICallEndObserver;
    class IDataCallEndObserver;
}

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:00 PM
     */

    class RadioMessageProcessor : public IRadioMessageProcessor
    {

    public:

        /**
         * Constructor
         *
         * @param responseReceiver    a handler for incoming train messages
         * @param statusManager    The status manager
         */
        RadioMessageProcessor( TA_IRS_Bus::IResponseReceiver& responseReceiver,
                               ITrainStatusManager& statusManager );


        /**
         * Destructor
         */
        virtual ~RadioMessageProcessor();


        /**
         * This is a handler for incoming SDS messages.
         *
         * @param data    The incoming data
         * @param senderTsi    The senders TSI
         * @param receiverTsi    The receivers TSI
         */
        virtual void processIncomingSds( const TA_IRS_Bus::TrainMessageData& data,
                                         const std::string& senderTsi,
                                         const std::string& receiverTsi );


        /**
         * This is a handler for call end events
         *
         * @param consoleId    The console associated with the call
         * @param callId    The call ID
         */
        virtual void processCallEndEvent( unsigned long consoleId,
                                          unsigned long callId );


        /**
         * This is a handler for data call end events
         *
         * @param itsi    The TSI the transfer was to
         * @param success    Whether the transfer was successful
         * @param details    The failure reason if the transfer wasnt successful
         */
        virtual void processDataCallEndEvent( const std::string& itsi,
                                              bool success,
                                              const std::string& details );


        /**
         * Adds an observer to the list of observers.
         * If this observer already exists, no change is made.
         *
         * @param observer    The observer to add
         * @param resource    The call resource the observer is interested in
         */
        void addCallEndObserver( ICallEndObserver* observer,
                                 TA_IRS_Bus::CommonTypes::ETrainResource resource );


        /**
         * Removes an observer to the list of observers.
         * If this observer doesnt exist, no change is made.
         *
         * @param observer    The observer to remove
         * @param resource    The call resource the observer was interested in
         */
        void removeCallEndObserver( ICallEndObserver* observer,
                                    TA_IRS_Bus::CommonTypes::ETrainResource resource );


        /**
         * Adds an observer to the list of observers.
         * If this observer already exists, no change is made.
         *
         * @param observer    The observer to add
         */
        void addDataCallEndObserver( IDataCallEndObserver* observer );


        /**
         * Removes an observer to the list of observers.
         * If this observer doesnt exist, no change is made.
         *
         * @param observer    The observer to remove
         */
        void removeDataCallEndObserver( IDataCallEndObserver* observer );


    private:

        /**
         * Private default constructor
         */
        RadioMessageProcessor();


        /**
         * A list of observers for call end events
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:48:00 PM
         */
        typedef std::vector< ICallEndObserver* > CallEndObserverList;

        /**
         * A map of resource types to observers of those call resources
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:48:00 PM
         */
        typedef std::map<
            TA_IRS_Bus::CommonTypes::ETrainResource,
            CallEndObserverList
            > CallEndObserverMap;

        /**
         * A list of observers for call end events
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:48:00 PM
         */
        typedef std::vector< IDataCallEndObserver* > DataCallEndObserverList;


        /**
         * This is the handler for incoming SDS from trains.
         */
        TA_IRS_Bus::IResponseReceiver& m_responseReceiver;


        /**
         * The status manager
         */
        ITrainStatusManager& m_statusManager;


        /**
         * The thread pool used to queue work items
         */
        TA_Base_Core::ThreadPoolSingletonManager* m_threadPool;


        /**
         * The list of observers, by resource type
         */
        CallEndObserverMap m_callEndObservers;


        /**
         * A lock for the observer list
         */
        TA_Base_Core::NonReEntrantThreadLockable m_callEndObserverLock;


        /**
         * The list of observers
         */
        DataCallEndObserverList m_dataCallEndObservers;


        /**
         * A lock for the observer list
         */
        TA_Base_Core::NonReEntrantThreadLockable m_dataCallEndObserverLock;

    };

} // TA_IRS_App

#endif // !defined(RadioMessageProcessor_H)
