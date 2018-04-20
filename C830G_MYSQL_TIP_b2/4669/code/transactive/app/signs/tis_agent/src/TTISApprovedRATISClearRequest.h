/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(TTISApprovedRATISClearRequest_H)
#define TTISApprovedRATISClearRequest_H

#include "app/signs/tis_agent/src/TTISMessage.h"

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/timers/src/TimedSingleThreadBarrier.h"

#include <boost/shared_ptr.hpp>

#include <vector>


namespace TA_IRS_App
{
    /**
     * @version 1.0
     * @created 01-Apr-2008 2:40:18 PM
     */
    class TTISApprovedRATISClearRequest : public TTISMessage
    {
    public:

        /**
         * Create the object with the required parameters to wait for and log each RATIS request result.
         *
         * @param trainList    The lsit of trains the request is going to
         * @param ratisTag    The RATIS tag
         * @param ratisMessageType    The ratis message type
         * @param priority    The original priority
         * @param operatorName    The operator name to log
         * @param sessionId    The submitting operator
         * @param timestamp    The submission time
         */
        TTISApprovedRATISClearRequest( const TA_IRS_Bus::CommonTypes::TrainIdList& trainList,
                                       const std::string& ratisTag,
                                       TA_Base_Core::ERATISMessageType ratisMessageType,
                                       int priority,
                                       const std::string& startTime,
                                       const std::string& endTime,
                                       const std::string& destination,
                                       const std::string& operatorName,
                                       const std::string& sessionId,
                                       time_t timestamp );


        ~TTISApprovedRATISClearRequest();


        /**
         * This notifies that a clear command sent to a train has completed, and its status.
         * It will check if that message completion was for a train as part of this request, if so the result will be handled here and does not need to be handled by the normal handler.
         *
         * @return true if this was part of this RATIS request
         *
         * @param trainId    The train this request is for
         * @param timestamp    The time the request was initiated (this must be sent back to the original client GUI)
         * @param sessionId    The originating session ID
         * @param displayParameters    The original message parameters
         * @param success    whether the command succeeded or not
         * @param statusString    The error status (for user display)
         */
        virtual bool clearRequestComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           time_t timestamp,
                                           std::string sessionId,
                                           const TA_Base_Core::TTISMessageResetParameters& displayParameters,
                                           bool success,
                                           std::string statusString );


        /**
         * This will wait for the (already submitted) request to complete. It will block until all results have returned or until the wait times out.
         *
         * @return the list of trains that failed to complete (either timed out or got a failure response)
         *
         * @param waitTimeMillis    The maximum time to wait for the request to complete
         */
        TA_IRS_Bus::CommonTypes::TrainIdList waitForCompletion( unsigned long waitTimeMillis );


        virtual void log( bool sendSuccess )
        {
        };//limin, added to made it none abstruct

    private:

        TTISApprovedRATISClearRequest();


        /**
         * This is used to wait for all the responses or time out trying
         */
        TA_Base_Core::TimedSingleThreadBarrier m_barrier;


        /**
         * Lock for m_trainList
         */
        TA_Base_Core::ReEntrantThreadLockable m_trainListLock;


        /**
         * The list of trains this request is for
         * During execution this will have successful trains removed, so on completion will hold a list of failed trains.
         */
        TA_IRS_Bus::CommonTypes::TrainIdList m_trainList;


        /**
         * The operator who submitted (for logging)
         */
        std::string m_operatorName;


        std::string m_ratisTag;


        TA_Base_Core::ERATISMessageType m_messageType;


        /**
         * The original ratis message priority
         */
        int m_priority;

        std::string m_startTime;
        std::string m_endTime;
        std::string m_destination;


        /**
         * The session ID the request was submitted with (used to match display request results)
         */
        std::string m_sessionId;


        /**
         * The time the request was sent to the train agent (used to match display request results)
         */
        time_t m_timestamp;

    };


    typedef boost::shared_ptr< TTISApprovedRATISClearRequest > TTISApprovedRATISClearRequestPtr;


    typedef std::vector< TTISApprovedRATISClearRequestPtr > TTISApprovedRATISClearRequestList;

}
#endif // !defined(TTISApprovedRATISClearRequest_H)
