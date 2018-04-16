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
#if !defined(TrainOperationModeCorbaProxy_H)
#define TrainOperationModeCorbaProxy_H


#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include <string>
#include <boost/shared_ptr.hpp>


namespace TA_IRS_Bus
{

    /**
     * Interface for commands used during Degraded Mode.
     * This is a simple proxy, it wraps the named object, and converts parameters and
     * return types between CORBA and C++ datatypes using the conversion methods.
     * @author Robert van Hugten
     * @version 1.0
     * @created 22-Oct-2007 4:46:57 PM
     */

    class TrainOperationModeCorbaProxy
    {

    public:


        /**
         * Constructor that will create the reference to the remote agent interface
         *
         * @param agentName    The entity name of the train agent this proxy will contact
         */
        TrainOperationModeCorbaProxy( const std::string& agentName );


        /**
         * Destructor
         */
        virtual ~TrainOperationModeCorbaProxy();


        /**
         * Checks whether the agent is in local control due to radio fallback or local
         * duty. This is only applicable at station locations.
         *
         * This is called by the ATS agent to determine if it should be exchanging ATS
         * data locally, or using OCC ATS to do the data exchange.
         *
         * @return true if ATS should be in local mode
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         */
        bool radioInFallback();


        /**
         * Called by the radio agent to notify the local train agent that it has entered
         * or left radio fallback mode. If not already in local duty mode, entering
         * fallback will trigger local duty mode.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         *
         * @param fallback    Whether the radio system is in fallback or not (true is fallback, false is normal)
         */
        void inFallback( bool fallback );


        /**
         * Called by the ATS agent to notify the local train agent that it has entered or
         * left double OCC ATS failure mode. If not already in local duty mode, entering
         * fallback will trigger local duty mode.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         *
         * @param doubleAtsFailure
         */
        void inDoubleAtsFailure( bool doubleAtsFailure );


        /**
         * This will manually force train communications over to this train agent.
         * It is only to be used in failure scenarios, and incorrect use could result
         * in the situation automatically normalising (or attempting to normalise)
         * 
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidTrainException if the train ID given is not known or not valid for this operation
         * @exception TransmissionFailureException if there was a failure sending the command to the train
         * @exception TransactionTimeoutException if there was a timeout waiting for the train to respond to the command
         * @exception TransactionCancelledException if the transaction was externally cancelled before completion
         * @exception MpuChangeoverException if during the transaction, an MPU changeover was received
         * @exception BadCommandException if instead of a valid response, the train responded with a bad command message
         * 
         * @param trainId    The train ID to manually take
         * @param sessionId    The operator taking control
         */
        void manuallyTakeTrainControl( CommonTypes::TrainIdType trainId, const std::string& sessionId );


    private:

        // to avoid including any CORBA in the proxy class header files
        // we are now using the 'pimpl' idiom
        // ive just pimpled this class out
        struct OperationModeCorbaObject;


        TrainOperationModeCorbaProxy();


        /**
         * object for access to the CORBA servant.
         */
        OperationModeCorbaObject* m_operationModeCorbaObject;

    };


    /**
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:57 PM
     */
    typedef boost::shared_ptr< TrainOperationModeCorbaProxy > TrainOperationModeCorbaProxyPtr;

}

#endif // !defined(TrainOperationModeCorbaProxy_H)
