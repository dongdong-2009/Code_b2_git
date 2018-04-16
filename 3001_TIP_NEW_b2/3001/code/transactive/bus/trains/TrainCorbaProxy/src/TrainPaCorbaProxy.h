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

#if !defined(TrainPaCorbaProxy_H)
#define TrainPaCorbaProxy_H


#include "bus/trains/TrainCommonLibrary/src/GenericObserverManager.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainPaUpdateObserver.h"
#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

#include <boost/shared_ptr.hpp>


namespace TA_IRS_Bus
{
    /**
     * Interface for commands used by Trainborne PA operations.
     * This is a simple proxy, it wraps the named object, and converts parameters and
     * return types between CORBA and C++ datatypes using the conversion methods.
     * @author Robert van Hugten
     * @version 1.0
     * @created 22-Oct-2007 4:46:58 PM
     */

    class TrainPaCorbaProxy
                : public TA_IRS_Bus::GenericObserverManager< ITrainPaUpdateObserver >,
                  public TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >
    {

    public:


        /**
         * Constructor that will create the reference to the remote agent interface
         *
         * @param agentKey    The train agent entity key
         * @param agentName    The entity name of the train agent this proxy will contact
         * @param locationKey    The agent location key
         */
        TrainPaCorbaProxy( const std::string& agentName,
                           unsigned long agentKey,
                           unsigned long locationKey );


        /**
         * Destructor
         */
        ~TrainPaCorbaProxy();


        /**
         * Gets the maximum wait time for a PA operation.
         * This is the total time to wait for the unsolicited response from a train.
         *
         * @return the wait time in seconds
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         */
        unsigned long getCommandTimeout();


        /**
         * Gets the PA library version on this train.
         *
         * @return the PA library version for the given train
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         *
         * @param trainId    The train ID to query
         */
        //unsigned short getTrainPaLibraryVersion( CommonTypes::TrainIdType trainId );
        unsigned long getTrainPaLibraryVersion( CommonTypes::TrainIdType trainId );


        /**
         *
         * @param payload
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload );


        /**
         * Gets the PA library version for all known trains
         *
         * @return the PA library version for all trains
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         */
        PaTypes::TrainLibraryInfoList getTrainPaLibraryVersionList();


        /**
         * This will broadcast a cyclic PA pre-recorded announcement on each given train.
         *
         * It will send the command to all trains in parallel. Once the commands have been
         * spawned, this method will return, the result of each command on each train will
         * be returned asynchronously using a PA Update Event.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trainIdList    The trains included in the announcement
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param messageNumber    This is the message number to boradcast. (Table offset)
         * @param libraryVersion    This is the library version used to choose the message number.
         *                          If different from the version on the train the boradcast will not be done.
         * @param announcementPeriod    This is the time parameters to broadcast the message
         * @param override    Whether to set the override flag, or leave this as a normal request
         * @param sessionId    The operator submitting the broadcast
         */
        void cyclicAnnouncement( const CommonTypes::TrainIdList& trainIdList,
                                 PaTypes::AnnouncementId announcementId,
                                 //unsigned char messageNumber,
                                 unsigned short messageNumber,
                                 //unsigned short libraryVersion,
                                 unsigned long libraryVersion,
                                 const PaTypes::MessageSchedule& announcementPeriod,
                                 bool override,
                                 const std::string& sessionId );


        /**
         * This will broadcast a PA pre-recorded announcement on each given train.
         *
         * It will send the command to all trains in parallel. Once the commands have been
         * spawned, this method will return, the result of each command on each train will
         * be returned asynchronously using a PA Update Event.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trainIdList    The trains included in the announcement
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param messageNumber    This is the message number to boradcast. (Table offset)
         * @param libraryVersion    This is the library version used to choose the message
         * number. If different from the version on the train the boradcast will not be
         * done.
         * @param override    Whether to set the override flag, or leave this as a normal
         * request
         * @param sessionId    The operator submitting the broadcast
         */
        void preRecordedAnnouncement( const CommonTypes::TrainIdList& trainIdList,
                                      PaTypes::AnnouncementId announcementId,
                                      //unsigned char messageNumber,
                                      unsigned short messageNumber,
                                      //unsigned short libraryVersion,
                                      unsigned long libraryVersion,
                                      bool override,
                                      const std::string& sessionId,
									  bool isEmergency);


        /**
         * For a given live announcement in progress, when an audio call hasnt been made,
         * this will make the audio call for the live announcement.
         *
         * Depending on the number of trains involved, and whether a static talkgroup was
         * chosen, this will either simply call an individual TSI, a static talkgroup, or
         * make a DGNA then call the DGNA.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception TrainRadioException if a radio operation failed
         *
         * @param announcementId    The ID of the announcement to begin
         * @param sessionId    The operator making the broadcast
         */
        void beginLiveAnnouncement( PaTypes::AnnouncementId announcementId,
                                    const std::string& sessionId );


        /**
         * This will start the live announcement set up process on each train.
         * This will spawn off the commands to each train, and return to the caller once
         * this is done. The status of the command will be sent back using a PA Update
         * Event.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         * @exception GroupInUseException if the given group TSI is in use
         *
         * @param trainIdList    The trains included in the announcement
         * @param announcementId    The ID of the announcement for tracking purposes
         * @param override    Whether to set the override flag, or leave this as a normal request
         * @param staticGroup    The optional group TSI to use to call the trains for thelive announcement.
         *                       Empty string for a single train announcement, or for a DGNA.
         *                       If this is set for a single train, it will be ignored.
         * @param sessionId    The operator submitting the broadcast
         */
        void setupLiveAnnouncement( const CommonTypes::TrainIdList& trainIdList,
                                    PaTypes::AnnouncementId announcementId,
                                    bool override,
                                    const std::string& staticGroup,
                                    const std::string& sessionId );


        /**
         * For a given live announcement in progress, send a PA continue to all trains requiring it.
         *
         * Called when the "Continue" button is pressed by an operator for a PA Live announcement
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param announcementId    The ID of the live announcement to continue
         * @param sessionId    The operator making the broadcast
         */
        void continueLiveAnnouncement( PaTypes::AnnouncementId announcementId,
                                       const std::string& sessionId );


        /**
         * This ends a live announcment in progress. It will end any audio in progress for
         * the announcement, and reset the announcement on each train.
         *
         * This will return once the command is spawned on each train, and status updates
         * will be sent via PA Update Event.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param announcementId    The ID of the live announcement to end
         * @param sessionId    The operator making the broadcast
         */
        void endLiveAnnouncement( PaTypes::AnnouncementId announcementId,
                                  const std::string& sessionId );


        /**
         * This ends any ongoing DVA or cyclic DVA boradcast on the train.
         * This is not used for live announcements.
         * The result will be sent back by a PA Update Event.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidParameterException if one of the options given for the operation is erroneous
         *
         *
         * @param trainIdList    The trains to end this announcement on
         * @param announcementId    The ID of the announcement to end
         * @param sessionId    The operator ending the broadcast
         */
        void endBroadcast( const CommonTypes::TrainIdList& trainIdList,
                           PaTypes::AnnouncementId announcementId,
                           const std::string& sessionId );


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
        struct PaCorbaObject;

        
        TrainPaCorbaProxy();


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
        PaCorbaObject* m_paCorbaObject;


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
     * @created 22-Oct-2007 4:46:58 PM
     */
    typedef boost::shared_ptr< TrainPaCorbaProxy > TrainPaCorbaProxyPtr;

}

#endif // !defined(TrainPaCorbaProxy_H)
