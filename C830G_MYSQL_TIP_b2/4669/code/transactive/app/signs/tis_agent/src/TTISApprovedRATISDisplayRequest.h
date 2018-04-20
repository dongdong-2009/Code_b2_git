#ifndef TTISAPPROVEDRATISDISPLAYREQUEST_H_INCLUDED
#define TTISAPPROVEDRATISDISPLAYREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/TTISApprovedRATISDisplayRequest.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class is used to represent a TTIS RATIS display 
  * request.
  *
  */

#include "app/signs/tis_agent/src/TTISManager.h"
#include "app/signs/tis_agent/src/TTISMessage.h"

// IDL compiler-produced header file
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

#include "core/timers/src/TimedSingleThreadBarrier.h"

#include <boost/shared_ptr.hpp>

#include <vector>
namespace TA_IRS_App
{

	class TTISApprovedRATISDisplayRequest : public TTISMessage
	{

	public:

        /**
         * Create the object with the required parameters to wait for and log each RATIS request result.
         * 
         * @param trainList    The lsit of trains the request is going to
         * @param ratisTag    The RATIS tag
         * @param ratisMessageType    The ratis message type
         * @param operatorName    The operator name to log
         * @param sessionId    The submitting operator
         * @param timestamp    The submission time
         */
        TTISApprovedRATISDisplayRequest( const TA_IRS_Bus::CommonTypes::TrainIdList& trainList,
                                         const std::string& ratisTag,
                                         TA_Base_Core::ERATISMessageType ratisMessageType,
                                         const std::string& destination,
                                         const std::string& operatorName,
                                         const std::string& sessionId,
                                         time_t timestamp );

		~TTISApprovedRATISDisplayRequest();

        bool freeTextMessageRequestComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                             time_t timestamp,
                                             std::string sessionId,
                                             const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters,
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


        virtual void log(bool sendSuccess) {};
    private:

        TTISApprovedRATISDisplayRequest();


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

    typedef boost::shared_ptr< TTISApprovedRATISDisplayRequest > TTISApprovedRATISDisplayRequestPtr;


    typedef std::vector< TTISApprovedRATISDisplayRequestPtr > TTISApprovedRATISDisplayRequestList;
}

#endif
