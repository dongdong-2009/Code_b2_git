/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/TTISAdHocDisplayRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/TTISAdHocDisplayRequest.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestData.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>
namespace TA_IRS_App
{

	TTISAdHocDisplayRequest::TTISAdHocDisplayRequest()    
	{
        FUNCTION_ENTRY( "TTISAdHocDisplayRequest" );
        FUNCTION_EXIT;
	}


	TTISAdHocDisplayRequest::~TTISAdHocDisplayRequest()
	{
        FUNCTION_ENTRY( "~TTISAdHocDisplayRequest" );
        FUNCTION_EXIT;
	}


	void TTISAdHocDisplayRequest::log(bool sendSuccess)
	{
        FUNCTION_ENTRY( "log" );
		TA_ASSERT(m_dataIsSet, "Log() called for Adhoc display request that has no data");

        // create the PID list
		std::stringstream pidStream;
        std::vector<TA_Base_Core::EPIDSelection >::iterator iter;
		for( iter = m_pidSelectionList.begin() ; iter != m_pidSelectionList.end() ; iter++ )
		{
			// Add a space before the next level name
			// this ensures that we don't add one at the end
			if( iter != m_pidSelectionList.begin() )
			{
				pidStream << " ";
			}

			pidStream << (int)(*iter);
		}
		
		
		
		TA_IRS_Core::IDisplayRequest* displayRequest = TA_IRS_Core::DisplayRequestAccessFactory::getInstance().createNewDisplayRequest();
		
		displayRequest->setAdhocMessageContent( m_messageText );
		displayRequest->setDestinationSystem( "TTIS" );
		displayRequest->setStartTime( m_startTime );
		displayRequest->setEndTime( m_endTime );
		displayRequest->setPriority( m_priority );
		displayRequest->setTimestamp( m_timestamp );
		displayRequest->setMessageType( TA_IRS_Core::IDisplayRequest::ADHOC );
		//hongran++ TD17502
		displayRequest->setIfSendSuccess(sendSuccess ? TA_Base_Core::Pass : TA_Base_Core::Fail);
		//++hongran TD17502
		
		// Display Attributes
		TA_Base_Core::DisplayAttributes displayAttributes;
		displayAttributes.displayMode = m_displayMode;
		displayAttributes.justification = m_justification;
		displayAttributes.repeatInterval = m_repeatInterval;
		displayRequest->setDisplayAttributes( displayAttributes );
		
		displayRequest->setSubmittedBy( m_submittedBy );
		
		// LED Attributes
		TA_Base_Core::LEDAttributes ledAttributes;
        if ( m_fontSize == TA_Base_Core::TTIS_FONT_SIZE_SMALL )
        {
            ledAttributes.fontSize = TA_Base_Core::FONT_SIZE_SMALL;
        }
        else
        {
            ledAttributes.fontSize = TA_Base_Core::FONT_SIZE_LARGE;
        }
		
		// Map the ETTISLEDIntensity to an ELEDIntensity
		// The reverse will have to be performed when reading from the database
		if( m_intensity == TA_Base_Core::NORMAL )
		{
			ledAttributes.intensity = TA_Base_Core::INTENSITY_LOW;
		}
		else
		{
            ledAttributes.intensity = TA_Base_Core::INTENSITY_HIGH;
		}
		
		displayRequest->setLEDAttributes( ledAttributes );
		
		// Train number
		std::stringstream trainNumber;
		trainNumber << m_trainNumber;
		displayRequest->setTtisDestinationTrainId( trainNumber.str() );
		
		displayRequest->setTtisDestinationPids( pidStream.str() );
		
		// write this display request to the database
		displayRequest->applyChanges();
	
		
		delete displayRequest;
		displayRequest = NULL;
     
        FUNCTION_EXIT;
	}


    void TTISAdHocDisplayRequest::setData( CORBA::UShort trainNumber,
                                           const TA_Base_Core::TrainPIDList& pidList,
                                           const char* messageContent,
                                           TA_Base_Core::ETTISLEDFontSize fontSize,
                                           TA_Base_Core::EJustification justification,
                                           TA_Base_Core::ETTISLEDIntensity intensity,
                                           TA_Base_Core::EDisplayMode displayMode,
                                           CORBA::UShort priority,
                                           const char* startTime,
                                           const char* endTime,
                                           CORBA::UShort repeatInterval,
                                           const std::string& operatorName )
	{
        FUNCTION_ENTRY( "setData" );
		m_trainNumber = trainNumber;

		unsigned int i;
		for( i = 0 ; i < pidList.length() ; i++ )
		{
			m_pidSelectionList.push_back( pidList[i] );
		}

		m_messageText = messageContent;
		m_fontSize = fontSize;
		m_justification = justification;
		m_intensity = intensity;
		m_displayMode = displayMode;
		//2008-08-28 convert enum to number
        m_priority = priority + 1;
		m_startTime = startTime;
		m_endTime = endTime;
		m_repeatInterval = repeatInterval;
		m_submittedBy = operatorName;  

		m_dataIsSet = true;

        FUNCTION_EXIT;
	}
	
}