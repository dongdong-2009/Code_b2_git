/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/TTISApprovedRATISDisplayRequest.cpp $
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

#ifdef _MSC_VER
#pragma warning( disable : 4503 )
#endif // _MSC_VER


#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/TISMessageDataDefinitionsHelper.h"
#include "app/signs/tis_agent/src/TTISApprovedRATISDisplayRequest.h"

#include "core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestData.h"

#include <iomanip>

namespace TA_IRS_App
{

    TTISApprovedRATISDisplayRequest::TTISApprovedRATISDisplayRequest( const TA_IRS_Bus::CommonTypes::TrainIdList& trainList,
                                                                      const std::string& ratisTag,
                                                                      TA_Base_Core::ERATISMessageType ratisMessageType,
                                                                      const std::string& destination,
                                                                      const std::string& operatorName,
                                                                      const std::string& sessionId,
                                                                      time_t timestamp )
        : m_barrier( trainList.size() ),
          m_trainList( trainList ),
          m_operatorName( operatorName ),
          m_ratisTag( ratisTag ),
          m_messageType( ratisMessageType ),
          m_destination( destination ),
          m_sessionId( sessionId ),
          m_timestamp( timestamp )
    {
        FUNCTION_ENTRY( "TTISApprovedRATISDisplayRequest" );
        FUNCTION_EXIT;
    }
	TTISApprovedRATISDisplayRequest::TTISApprovedRATISDisplayRequest()
	{
        FUNCTION_ENTRY( "TTISApprovedRATISDisplayRequest" );
        FUNCTION_EXIT;
	}


	TTISApprovedRATISDisplayRequest::~TTISApprovedRATISDisplayRequest()
	{
        FUNCTION_ENTRY( "~TTISApprovedRATISDisplayRequest" );
        FUNCTION_EXIT;
	}


    bool TTISApprovedRATISDisplayRequest::freeTextMessageRequestComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                                          time_t timestamp,
                                                                          std::string sessionId,
                                                                          const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters,
                                                                          bool success,
                                                                          std::string statusString )
	{
        FUNCTION_ENTRY( "freeTextMessageRequestComplete" );

        //
        // first check if this message result is for this request
        //

        if ( ( sessionId != m_sessionId ) || ( timestamp != m_timestamp ) )
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
                             "Multiple responses received for the same Free Text display request (Session: %s, Time: %d, Train: %d)",
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

        // if the display request failed, there is no need to do the logging
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
		TA_IRS_Core::IDisplayRequest* displayRequest = TA_IRS_Core::DisplayRequestAccessFactory::getInstance().createNewDisplayRequest();

            displayRequest->setAdhocMessageContent( displayParameters.message.in() );
            displayRequest->setDestinationSystem( "TTIS" );
            displayRequest->setStartTime( displayParameters.startTime.in() );
            displayRequest->setEndTime( displayParameters.endTime.in() );
            displayRequest->setPriority( displayParameters.priority );
            displayRequest->setTimestamp( STISManager::getInstance()->getNewTimestampString() );
            displayRequest->setIfSendSuccess( success ? TA_Base_Core::Pass : TA_Base_Core::Fail );

        // Convert m_messageType (TA_IRS_Core::ERATISMessageType) into EMessageType
        switch (m_messageType)
        {
            case TA_Base_Core::RATIS_IN_NEW:
                displayRequest->setMessageType(TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW);
                break;
            case TA_Base_Core::RATIS_OUT_NEW:
                displayRequest->setMessageType(TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW);
                break;

            case TA_Base_Core::RATIS_IN_CLEAR:
                displayRequest->setMessageType(TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR);
                break;
            case TA_Base_Core::RATIS_OUT_CLEAR:
                displayRequest->setMessageType(TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR);
                break;
            case TA_Base_Core::RATIS_IN_UPDATE:
                displayRequest->setMessageType(TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE);
                break;
            case TA_Base_Core::RATIS_OUT_UPDATE:
                 displayRequest->setMessageType(TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE);
                break;
           default:
                break;
        }

		// Display Attributes
		TA_Base_Core::DisplayAttributes displayAttributes;
            displayAttributes.displayMode = displayParameters.displayMode;
            displayAttributes.justification = displayParameters.justification;
            displayAttributes.repeatInterval = displayParameters.repeatInterval;
		displayRequest->setDisplayAttributes( displayAttributes );

            displayRequest->setSubmittedBy( m_operatorName );

		// LED Attributes
		TA_Base_Core::LEDAttributes ledAttributes;
            switch ( displayParameters.fontSize )
		{
                case TA_Base_Core::TTIS_FONT_SIZE_SMALL:
                    ledAttributes.fontSize = TA_Base_Core::FONT_SIZE_SMALL;
                    break;

                case TA_Base_Core::TTIS_FONT_SIZE_LARGE:
                default:
                    ledAttributes.fontSize = TA_Base_Core::FONT_SIZE_LARGE;
                    break;
		}

            switch ( displayParameters.intensity )
            {
                case TA_Base_Core::NORMAL:
                    ledAttributes.intensity = TA_Base_Core::INTENSITY_LOW;
                    break;

                case TA_Base_Core::HIGH:
                default:
                    ledAttributes.intensity = TA_Base_Core::INTENSITY_HIGH;
                    break;
		}

		displayRequest->setLEDAttributes( ledAttributes );

            displayRequest->setTtisDestinationPids( "0" );
            displayRequest->setRatisTag( m_ratisTag );
            displayRequest->setRatisDestination( m_destination );
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


    TA_IRS_Bus::CommonTypes::TrainIdList TTISApprovedRATISDisplayRequest::waitForCompletion( unsigned long waitTimeMillis )
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