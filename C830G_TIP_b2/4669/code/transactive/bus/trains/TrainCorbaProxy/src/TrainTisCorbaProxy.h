/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(TrainTisCorbaProxy_H)
#define TrainTisCorbaProxy_H


#include "bus/trains/TrainCommonLibrary/src/GenericObserverManager.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainTisUpdateObserver.h"
#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include <boost/shared_ptr.hpp>


namespace TA_IRS_Bus
{
    /**
     * Interface for commands used by Trainborne TIS operations.
     * This is a simple proxy, it wraps the named object, and converts parameters and
     * return types between CORBA and C++ datatypes using the conversion methods.
     * @author Robert van Hugten
     * @version 1.0
     * @created 22-Oct-2007 4:47:02 PM
     */

    class TrainTisCorbaProxy
              : public TA_IRS_Bus::GenericObserverManager<ITrainTisUpdateObserver>,
                public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {

    public:


        /**
         * Constructor that will create the reference to the remote agent interface
         *
         * @param agentKey    The train agent entity key
         * @param agentName    The entity name of the train agent this proxy will contact
         * @param locationKey    The agent location key
         */
        TrainTisCorbaProxy( const std::string& agentName,
                            unsigned long agentKey,
                            unsigned long locationKey );

        /**
         * destructor
         */
        virtual ~TrainTisCorbaProxy();


        /**
         * Gets the library versions for the given train.
         *
         * @return the library version for the train specified
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         *
         * @param trainId    The train to get the library versions for
         */
        TtisTypes::TisLibraryVersionUpdate getLibraryVersions( CommonTypes::TrainIdType trainId );


        /**
         * Gets the library versions for all known trains
         *
         * @return the library versions for all kown trains
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         */
        TtisTypes::TisLibraryVersionList getLibraryVersions();


        /**
         * Displays a free text message to the given PIDs on the given trains.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The list of trains to display the message on
         * @param displayParameters    The parameters required to display the message
         * @param timestamp    The time of the request submission. This will be set along
         * with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        void displayFreeTextMessage( const CommonTypes::TrainIdList& trains,
                                     const TtisTypes::TtisFreeTextDisplayParameters& displayParameters,
                                     time_t timestamp,
                                     const std::string& sessionId );


        /**
         * Displays a pre defined text message to the given PIDs on the given trains.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The list of trains to display the message on
         * @param displayParameters    The predefined display parameters
         * @param timestamp    The time of the request submission. This will be set along
         * with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        void displayStoredMessage( const CommonTypes::TrainIdList& trains,
                                   const TtisTypes::TtisPredefinedDisplayParameters& displayParameters,
                                   time_t timestamp,
                                   const std::string& sessionId );


        /**
         * Sends a reset/clear request for messages of a certain priority on the given
         * PIDs on the given trains.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The list of trains to clear the message on
         * @param displayParameters    The parameters for the reset message
         * @param timestamp    The time of the request submission. This will be set along
         * with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        void resetMessage( const CommonTypes::TrainIdList& trains,
                           const TtisTypes::TtisResetParameters& displayParameters,
                           time_t timestamp,
                           const std::string& sessionId );


        /**
         * This will start a predefined message library download to the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message. It will indicate the download
         * start action, and whether it was successful or failed, and give a failure reason.
         * If the download started, once the download completes another one of these
         * messages will be sent indicating the status.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to download the given library to
         * @param libraryData    The message library data to download to the train
         * @param libraryVersion    The library version of the library Data
         * @param sessionId    The operator initiating the download
         */
        void libraryDownload( const CommonTypes::TrainIdList& trains,
                              const TtisTypes::LibraryDataType& libraryData,
                              unsigned short libraryVersion,
                              const std::string& sessionId );


        /**
         * This will upgrade the predefined message library on the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message. It will indicate
         * whether the upgrade was successful or failed, and give a failure reason.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to send a library upgrade command to
         * @param sessionId    The operator initiating the upgrade
         */
        void libraryUpgrade( const CommonTypes::TrainIdList& trains,
                             const std::string& sessionId );


        /**
         * This will start a time schedule download to the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message. It will indicate
         * the download start action, and whether it was successful or failed, and give a failure reason.
         * If the download started, once the download completes another one of these
         * messages will be sent indicating the status.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to download the given schedule to
         * @param scheduleData    The time schedule data to download to the train
         * @param scheduleVersion    The version of the time schedule data
         * @param sessionId    The operator initiating the download
         */
        void scheduleDownload( const CommonTypes::TrainIdList& trains,
                               const TtisTypes::LibraryDataType& scheduleData,
                               unsigned short scheduleVersion,
                               const std::string& sessionId );


        /**
         * This will upgrade the predefined message library on the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message. It will indicate
         * whether the upgrade was successful or failed, and give a failure reason.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to send a schedule upgrade command to
         * @param sessionId    the operator initiating the upgrade
         */
        void scheduleUpgrade( const CommonTypes::TrainIdList& trains,
                              const std::string& sessionId );


        /**
         * Gets the maximum wait time for a TTIS operation.
         * This is the total time to wait for the unsolicited response from a train.
         *
         * @return the wait time in seconds
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         */
        unsigned long getMaximumWaitTime();


        /**
         *
         * @param payload
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload );


    protected:

        /**
         * This is a virtual method, which by default does nothing.
         * It is called when the first observer is added, and can be overridden to perform
         * some specific operation (eg subscribe to messages)
         */
        virtual void firstObserverAdded();


        /**
         * This is a virtual method, which by default does nothing.
         * It is called when last first observer is removed, and can be overridden to
         * perform some specific operation (eg unsubscribe to messages)
         */
        virtual void lastObserverRemoved();


    private:


        // to avoid including any CORBA in the proxy class header files
        // we are now using the 'pimpl' idiom
        // ive just pimpled this class out
        struct TisCorbaObject;


        TrainTisCorbaProxy();


        /**
         * Called to subscribe to updates from the agent
         */
        void subscribeToMessages();


        /**
         * Called to unsubscribe to updates from the agent
         */
        void unsubscribeToMessages();


        /**
         * object for access to the CORBA servant
         */
        TisCorbaObject* m_tisCorbaObject;


        /**
         * flag to indicate message subscription
         */
        bool m_subscribedToMessages;


        /**
         * Lock to protect the subscription process
         */
        TA_Base_Core::NonReEntrantThreadLockable m_subscriptionLock;


        /**
         * This is the entity name of the train agent.
         */
        std::string m_trainAgentEntityName;


        /**
         * This is the entity key of the train agent, used to subscribe to comms messages
         * and state update messages.
         */
        unsigned long m_trainAgentEntityKey;


        /**
         * This is the location key of the train agent entity.
         */
        unsigned long m_trainAgentLocationKey;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:47:02 PM
     */
    typedef boost::shared_ptr< TrainTisCorbaProxy > TrainTisCorbaProxyPtr;

}

#endif // !defined(TrainTisCorbaProxy_H)
