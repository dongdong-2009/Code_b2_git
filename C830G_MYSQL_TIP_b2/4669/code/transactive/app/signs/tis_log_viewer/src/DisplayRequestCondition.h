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
#if !defined(DisplayRequestCondition_H)
#define DisplayRequestCondition_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/timers/src/TimedWaitSemaphore.h"

namespace TA_IRS_App
{
    /**
     * @author adamr
     * @version 1.0
     * @created 04-Apr-2008 1:48:11 PM
     */
    class DisplayRequestCondition
    {
    public:

        /**
         * Constructor
         */
        DisplayRequestCondition( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                 time_t timestamp,
                                 const std::string& sessionId,
                                 TA_Base_Core::TisCommandType originalCommand,
                                 unsigned long maximumRequestTime );


        /**
         * Destructor
         */
        virtual ~DisplayRequestCondition();


        /**
         * This will block until the request is complete
         * or there is a timeout waiting for the response
         */
        void waitForCompletion();


        /**
         * Checks if this command was successful or not
         * This can only be called once waitForCompletion() has returned.
         *
         * @return true if the command was a success, false otherwise
         */
        bool wasSuccessful() const;


        /**
         * Gets the error details if any.
         * This can only be called once waitForCompletion() has returned.
         *
         * @return an error string if applicable
         */
        std::string getErrorDetails() const;


        /**
         * This is an incoming response from a train.
         * It may or may not be for this display request.
         *
         * @param displayResult    The result of a TTIS command. May not be for the current object's display request.
         *
         * @return true if the display request was for this object, false otherwise
         */
        bool handleDisplayResult( const TA_Base_Core::TTISDisplayResult& displayResult );


    private:


        /**
         * The trains involved in the request that responses havent been received for.
         */
        TA_IRS_Bus::CommonTypes::TrainIdType m_trainId;


        /**
         * The time the request was submitted with
         */
        time_t m_timestamp;


        /**
         * The session ID the request was submitted with
         */
        std::string m_sessionId;


        /**
         * The command that was submitted
         */
        TA_Base_Core::TisCommandType m_originalCommand;


        /**
         * The maximum time the request can take to execute (this is the longest this object will wait for a response)
         */
        unsigned long m_waitTimeMillis;


        /**
         * The semaphore used to wait for the response
         */
        TA_Base_Core::TimedWaitSemaphore m_semaphore;

        
        /**
         * The command success/failure, populated when response is received
         */
        bool m_success;


        /**
         * The error details if any, populated when response is received
         */
        std::string m_errorDetails;

    };

}
#endif // !defined(DisplayRequestCondition_H)
