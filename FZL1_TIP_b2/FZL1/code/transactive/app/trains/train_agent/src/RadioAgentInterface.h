/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(RadioAgentInterface_H)
#define RadioAgentInterface_H

#include "app/trains/train_agent/src/IRadioAgentInterface.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"

#include "core/naming/src/NamedObject.h"

// forward declarations
namespace TA_IRS_App
{
    class RadioSessionData;
}

namespace TA_IRS_App
{

    /**
     * The actual implementation of the calls to the radio agent
     *
     * Looking at TrainAgent.cpp in C830 may prove useful for implementation notes.
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:47:57 PM
     */
    class RadioAgentInterface : public IRadioAgentInterface
    {

    public:

        /**
         * Constructor with minimum required information
         *
         * @param configuration    The agent configuration
         * @param statusManager    The object used to store state
         */
        RadioAgentInterface( ITrainAgentConfiguration& configuration,
                             ITrainStatusManager& statusManager );


        /**
         * Destructor
         */
        virtual ~RadioAgentInterface();


        /**
         * Sends the data to the given radio number.
         *
         * @exception TrainTransmissionFailureException if the send fails for some reason
         *
         * @param radioId    Where to send the data
         * @param data    the data
         */
        virtual void sendData( const std::string& radioId,
                               const TA_IRS_Bus::TrainMessageData& data );


        /**
         * Given a list of TSIs, create a DGNA.
         *
         * @return the group TSI
         *
         * @exception TrainRadioException if making the group fails
         *
         * @param tsiList    The list of TSIs to put in the DGNA
         * @param sessionId    The operator who will use the DGNA
         */
        virtual std::string createDynamicGroup( const TA_IRS_Bus::CallTypes::ItsiList& tsiList,
                                                const std::string& sessionId );


        /**
         * This will make an audio call to the given TSI, suitable for a PEC.
         * It is a:
         * single call
         * full duplex
         * emergency priority
         * direct (hook)
         * invisible to GUIs
         *
         * It will also add the call to the ongoing calls list in the status manager.
         *
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param trainItsi    The TSI to make the call to
         * @param sessionId    The session of the operator making the call
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makePecCall( const std::string& trainItsi,
                                                                            const std::string& sessionId );


        /**
         * This will make an audio call to the given TSI, suitable for answering an OCC Call request.
         * It is a:
         * single call
         * full duplex
         * normal priority
         * direct (hook)
         * visible to GUIs
         *
         * It will also add the call to the ongoing calls list in the status manager.
         *
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param trainItsi    The TSI to make the call to
         * @param sessionId    The session of the operator making the call
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makeOccCall( const std::string& trainItsi,
                                                                            const std::string& sessionId );


        /**
         * This will make an audio call to the given TSI, suitable for making an outgoing Driver Call.
         * It is a:
         * single call
         * full duplex
         * normal priority
         * direct (hook)
         * visible to GUIs
         *
         * It will also add the call to the ongoing calls list in the status manager.
         *
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param trainItsi    The TSI of the train to call
         * @param sessionId    The session of the operator making the call
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makeSingleDriverCall( const std::string& trainItsi,
                                                                                     const std::string& sessionId,
                                                                                     TA_IRS_Bus::CallTypes::ECallPriority callPriority );


        /**
         * This will make an audio call to the given group TSI,
         * suitable for making an outgoing Driver Call to a group (DGNA or GOA).
         * It is a:
         * group call (type specified by parameters)
         * half duplex
         * priority specified by parameters
         * direct (hook)
         * visible to GUIs
         *
         * It will also add the call to the ongoing calls list in the status manager.
         *
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param groupTsi    The TSI of the group to call
         * @param sessionId    The session of the operator making the call
         * @param callType    The type of call to make. Can not be single call, 
         *                    if a single call is given it will be overridden to a group unacknowledged call.
         * @param callPriority    The priority of the call to make
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makeGroupDriverCall( const std::string& groupTsi,
                                                                                    const std::string& sessionId,
                                                                                    TA_IRS_Bus::CallTypes::ECallType callType,
                                                                                    TA_IRS_Bus::CallTypes::ECallPriority callPriority );


        /**
         * This will make an audio call to the given TSI, suitable for making a PA Live Announcement Call.
         * It is a:
         * single call
         * half duplex
         * normal priority
         * direct (hook)
         * invisible to GUIs
         *
         * It will also add the call to the ongoing calls list in the status manager.
         *
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param trainItsi
         * @param sessionId
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makeSinglePaCall( const std::string& trainItsi,
                                                                                 const std::string& sessionId );


        /**
         * This will make an audio call to the given group TSI,
         * suitable for making a PA Live Announcement Call to a group of trains.
         * It is a:
         * group broadcast call
         * half duplex
         * normal priority
         * direct (hook)
         * invisible to GUIs
         *
         * It will also add the call to the ongoing calls list in the status manager.
         *
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param groupTsi
         * @param sessionId
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makeGroupPaCall( const std::string& groupTsi,
                                                                                const std::string& sessionId );


        /**
         * This will end a call that is in progress.
         * It will also remove the call from the ongoing calls list in the status manager.
         *
         * It will not throw any exceptions, just log an error if ending the call failed.
         *
         * @param callId
         * @param sessionId
         */
        virtual void endCall( unsigned long callId,
                              const std::string& sessionId );


        /**
         * This sets up a data call to transfer the given data to the given ITSI.
         *
         * @exception TrainRadioException if making the call fails, or there is already a transfer in progress.
         *
         * @param trainItsi    The TSI to send the data to
         * @param data    The raw bytes to send
         * @param sessionId    The operator sending the data
         */
        virtual void makeDataCall( const std::string& trainItsi,
                                   const TA_IRS_Bus::TrainMessageData& data,
                                   const std::string& sessionId );


        /**
         * This ends a data call in progress to the given ITSI.
         *
         * No exceptions will be thrown. Errors will be logged in the event of failure.
         *
         * @param trainItsi    The TSI to send the data to
         * @param sessionId    The operator sending the data
         */
        virtual void cancelDataCall( const std::string& trainItsi,
                                     const std::string& sessionId );


        /**
         * This checks the current fallback status of the radio system.
         *
         * @return true if the radio system is in fallback
         *
         * @exception AgentCommunicationException if there was an error asking the radio agent
         */
        virtual bool inFallback();


        /**
         * This will tell the radio agent (at a station) to reset its connection to the radio system.
         * This is done to leave radio fallback mode.
         *
         * @exception AgentCommunicationException if there was an error communicating with the radio agent
         */
        virtual void resetConnection();


        /**
         * This will get the local radio entity key - for receiving call stack updates
         *
         * @return the entity key to use for message subscription
         */
        virtual unsigned long getRadioAgentEntityKey();


    private:


        /**
         * Private default constructor
         */
        RadioAgentInterface();


        /**
         * Loads the radio named object for non-operator specific actions
         *
         * @param configuration    The agent configuration
         */
        void loadServerSession( ITrainAgentConfiguration& configuration );


        /**
         * loads the console radio sessions from the database
         */
        void loadConsoleSessions();


        /**
         * Gets the radio session object for the operator session
         *
         * @return the session object
         *
         * @exception TrainRadioException if the session does not exist
         *
         * @param operatorSessionId The operator doing the radio operation (look for his/her radio session)
         * @param failureType   What the operator is actually trying to do in case lookup fails
         */
        //RadioSessionData* getRadioSessionData( const std::string& operatorSessionId,
        //                                       TA_IRS_Bus::CommonTypes::ERadioErrorType failureType );


        /**
         * This will make the actual call to the radio agent to make a call.
         * It will convert the C++ into the corba types to send to the radio agent.
         * It will also add the call to the ongoing calls list in the status manager.
         *
         * @return the details of the newly made call
         *
         * @exception TrainRadioException if making the call fails
         *
         * @param resource    The resource the call is for
         * @param tsi    The number to call
         * @param sessionId    The session id of the operator making the call
         * @param callType    The type of call to make
         * @param fullDuplex    Whether the call is full or half duplex
         * @param callPriority    The call priority
         * @param hook    Direct call or not (defaults to true)
         * @param hideFromGui    Whether this call is hidden from the GUI or visible (defaults to true - hidden)
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails makeCall( TA_IRS_Bus::CommonTypes::ETrainResource resource,
                                                                         const std::string& tsi,
                                                                         const std::string& sessionId,
                                                                         TA_IRS_Bus::CallTypes::ECallType callType,
                                                                         bool fullDuplex,
                                                                         TA_IRS_Bus::CallTypes::ECallPriority callPriority,
                                                                         bool hook = true,
                                                                         bool hideFromGui = true );


        /**
         * This is the radio corba object
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:47:57 PM
         */
        typedef TA_Base_Core::NamedObject <
        TA_Base_Bus::IRadioCorbaDef,
        TA_Base_Bus::IRadioCorbaDef_ptr,
        TA_Base_Bus::IRadioCorbaDef_var > RadioAgentObject;

        /**
         * Maps console keys to the radio session data used to access the radio for that console.
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:47:57 PM
         */
        typedef std::map< unsigned long, RadioSessionData* > ConsoleKeyToRadioSessionMap;



        TA_Base_Bus::AuthenticationLibrary m_authLibrary;


        /**
         * The object used to store state
         */
        ITrainStatusManager& m_statusManager;


        /**
         * The server session (used to do non console specific things)
         */
        RadioAgentObject m_serverSession;


        /**
         * Used for subscribing to radio comms messages
         */
        unsigned long m_radioEntityKey;


        /**
         * Maps each console to its radio session information
         */
        ConsoleKeyToRadioSessionMap m_consoleKeyToRadioSessionMap;

    };

} // TA_IRS_App

#endif // !defined(RadioAgentInterface_H)
