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

#if !defined(TrainInformationCorbaProxy_H)
#define TrainInformationCorbaProxy_H


#include "bus/trains/TrainCommonLibrary/src/GenericObserverManager.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainInformationUpdateObserver.h"
#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include <boost/shared_ptr.hpp>


namespace TA_IRS_Bus
{

    /**
     * Interface for commands and notifications used by GUIs.
     * This is a simple proxy, it wraps the named object, and converts parameters and
     * return types between CORBA and C++ datatypes using the conversion methods.
     * @author Robert van Hugten
     * @version 1.0
     * @created 22-Oct-2007 4:46:56 PM
     */

    class TrainInformationCorbaProxy
            : public TA_IRS_Bus::GenericObserverManager<TA_IRS_Bus::ITrainInformationUpdateObserver>,
              public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {

    public:


        /**
         * Constructor that will create the reference to the remote agent interface
         *
         * @param agentName    The entity name of the train agent this proxy will contact
         * @param agentKey    The train agent entity key
         * @param locationKey    The agent location key
         */
        TrainInformationCorbaProxy( const std::string& agentName,
                                    unsigned long agentKey,
                                    unsigned long locationKey );


        /**
         * Destructor
         */
        virtual ~TrainInformationCorbaProxy();


        /**
         * Given a train ID, get the current details for it.
         * The train might not be valid or at a valid location, but the current known
         * details will be given.
         *
         * @return the details of the given train
         *
         * @exception AgentCommunicationException if there was a problem contacting the
         * agent
         * @exception InvalidTrainException if the train ID given is not known
         *
         * @param trainId    The train to get the details for
         */
        TA_IRS_Bus::TrainInformationTypes::TrainDetails getTrainDetails( CommonTypes::TrainIdType trainId );


        /**
         * Get details about all known valid trains.
         *
         * This will get the current details about all known trains, that are valid -
         * meaning they are communicating either by radio, or they are present in ATS.
         *
         * @return a list of all valid trains
         *
         * @exception AgentCommunicationException if there was a problem contacting the
         * agent
         */
        TA_IRS_Bus::TrainInformationTypes::TrainDetailsList getTrainList();

        /**
         * Gets the names of all named selection lists.
         *
         * @return a sequence of names of the existing selection lists
         *
         * @exception AgentCommunicationException if there was a problem contacting the
         * agent
         */
        TA_IRS_Bus::TrainInformationTypes::TrainSelectionList getTrainSelectionList();


        /**
         * Gets the trains involved in a single named train selection list.
         *
         * @return a list of train IDs that comprise the selection list
         *
         * @exception AgentCommunicationException if there was a problem contacting the
         * agent
         * @exception InvalidSelectionListException if the given train list is not valid
         *
         * @param selectionName
         */
        CommonTypes::TrainIdList getTrainSelection( const std::string& selectionName );


        /**
         * Deletes an existing selection list. If none existed this has no effect.
         *
         * @exception AgentCommunicationException if there was a problem contacting the
         * agent
         * @exception TrainAccessDeniedException If the operator does not have the correct
         * rights or duty to perform this operation
         *
         * @param selectionName    The named selection list to delete.
         * @param sessionId    The operator deleting the selection
         */
        void deleteTrainSelection( const std::string& selectionName, const std::string& sessionId );


        /**
         * Save a selection of trains into a named selection list.
         * Creates a new list if there isnt already one by this name, otherwise overwrites
         * any previously saved list with this name.
         *
         * @exception AgentCommunicationException if there was a problem contacting the
         * agent
         * @exception TrainAccessDeniedException If the operator does not have the correct
         * rights or duty to perform this operation
         *
         * @param selectionName    The name of the selection to save
         * @param trainIDs    The list of trains included in the train selection
         * @param sessionId    The operator saving the selection list
         */
        void saveTrainSelection( const std::string& selectionName, const CommonTypes::TrainIdList& trainIDs, const std::string& sessionId );


        /**
         * This checks the current agent state to determine its operation mode. Updates
         * are also sent when this changes, this allows the GUI to enable/disable train
         * lists, and locations within train lists.
         *
         * @return operation mode information about controlled locations, radio fallback,
         * ats failure, group offline, and local duty.
         *
         * @exception AgentCommunicationException if there was a problem contacting the
         * agent
         */
        TA_IRS_Bus::TrainInformationTypes::AgentOperationMode getAgentOperationMode();


        /**
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
        struct TrainInformationCorbaObject;


        TrainInformationCorbaProxy();


        /**
         * Called to subscribe to updates from the agent
         */
        void subscribeToMessages();


        /**
         * Called to unsubscribe to updates from the agent
         */
        void unsubscribeToMessages();


        /**
         * object for access to the CORBA servant.
         */
        TrainInformationCorbaObject* m_trainInformationCorbaObject;


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
     * @created 22-Oct-2007 4:46:56 PM
     */
    typedef boost::shared_ptr< TrainInformationCorbaProxy > TrainInformationCorbaProxyPtr;

}

#endif // !defined(TrainInformationCorbaProxy_H)
