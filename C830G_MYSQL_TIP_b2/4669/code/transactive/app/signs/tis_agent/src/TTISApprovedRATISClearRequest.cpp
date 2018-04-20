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

#include "TTISApprovedRATISClearRequest.h"

#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/TISMessageDataDefinitionsHelper.h"

#include "core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{
    TTISApprovedRATISClearRequest::TTISApprovedRATISClearRequest( const TA_IRS_Bus::CommonTypes::TrainIdList& trainList,
                                                                  const std::string& ratisTag,
                                                                  TA_Base_Core::ERATISMessageType ratisMessageType,
                                                                  int priority,
                                                                  const std::string& startTime,
                                                                  const std::string& endTime,
                                                                  const std::string& destination,
                                                                  const std::string& operatorName,
                                                                  const std::string& sessionId,
                                                                  time_t timestamp )
        : m_barrier( trainList.size() ),
          m_trainList( trainList ),
          m_operatorName( operatorName ),
          m_ratisTag( ratisTag ),
          m_messageType( ratisMessageType ),
          m_priority( priority ),
          m_startTime( startTime ),
          m_endTime( endTime ),
          m_destination( destination ),
          m_sessionId( sessionId ),
          m_timestamp( timestamp )
    {
        FUNCTION_ENTRY( "TTISApprovedRATISClearRequest" );
        FUNCTION_EXIT;
    }


    TTISApprovedRATISClearRequest::TTISApprovedRATISClearRequest()
    {
        FUNCTION_ENTRY( "TTISApprovedRATISClearRequest" );
        FUNCTION_EXIT;
    }


    TTISApprovedRATISClearRequest::~TTISApprovedRATISClearRequest()
    {
        FUNCTION_ENTRY( "~TTISApprovedRATISClearRequest" );
        FUNCTION_EXIT;
    }


    bool TTISApprovedRATISClearRequest::clearRequestComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                              time_t timestamp,
                                                              std::string sessionId,
                                                              const TA_Base_Core::TTISMessageResetParameters& displayParameters,
                                                              bool success,
                                                              std::string statusString )
    {
        FUNCTION_ENTRY( "clearRequestComplete" );

        //
        // first check if this message result is for this request
        //

        if ( ( sessionId != m_sessionId ) ||
             ( timestamp != m_timestamp ) )
        {
            FUNCTION_EXIT;
            return false;
        }
        {
            TA_THREADGUARD( m_trainListLock );

            TA_IRS_Bus::CommonTypes::TrainIdList::iterator findIter = m_trainList.find( trainId );

            if ( findIter == m_trainList.end() )
            {
                LOG_GENERIC( SourceInfo,
                             TA_Base_Core::DebugUtil::DebugError,
                             "Multiple responses received for the same clear request (Session: %s, Time: %d, Train: %d)",
                             sessionId.c_str(),
                             timestamp,
                             trainId );

                FUNCTION_EXIT;
                return false;
            }

            // remove it from the pending list if successful, only failures remain
            if ( true == success )
            {
                m_trainList.erase( findIter );
            }

            // signal that one of the waiting responses was received
            m_barrier.post();
        }

        // if the clear request failed, there is no need to do the logging
        if ( false == success )
        {
            FUNCTION_EXIT;
            return true;
        }

        //
        // log the display request to the DB
        //

        try
        {
            TA_IRS_Core::IDisplayRequest* displayRequest= TA_IRS_Core::DisplayRequestAccessFactory::getInstance().createNewDisplayRequest();
            displayRequest->setDestinationSystem( "TTIS" );
            displayRequest->setStartTime( m_startTime );
            displayRequest->setEndTime( m_endTime );
            displayRequest->setPriority( m_priority );
            displayRequest->setTimestamp( TA_IRS_App::STISManager::getInstance()->getNewTimestampString() );
            displayRequest->setMessageType( TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR );
            displayRequest->setRatisTag( m_ratisTag );
            displayRequest->setIfSendSuccess( success ? TA_Base_Core::Pass : TA_Base_Core::Fail );
            displayRequest->setSubmittedBy( m_operatorName );
            displayRequest->setRatisDestination( m_destination );
            displayRequest->setAdhocMessageContent( "" );
            displayRequest->setTtisDestinationPids( "0" );
            
            // Train number
            std::stringstream trainNumber;
            trainNumber << trainId;
            displayRequest->setTtisDestinationTrainId( trainNumber.str() );

            // write this display request to the database
            displayRequest->applyChanges();

            delete displayRequest;
            displayRequest = NULL;
        }
        catch ( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
        }

        FUNCTION_EXIT;
        return true;
    }


    TA_IRS_Bus::CommonTypes::TrainIdList TTISApprovedRATISClearRequest::waitForCompletion( unsigned long waitTimeMillis )
    {
        FUNCTION_ENTRY( "waitForCompletion" );

        m_barrier.timedWait( waitTimeMillis );

        // the remaining trains in the list have failed
        // swap them out
        TA_IRS_Bus::CommonTypes::TrainIdList failedTrains;

        TA_THREADGUARD( m_trainListLock );
        m_trainList.swap( failedTrains );

        FUNCTION_EXIT;
        return failedTrains;
    }


}
