/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/TTISPredefinedDisplayRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/01/22 11:01:50 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/TTISPredefinedDisplayRequest.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestData.h"

#include "app/signs/tis_agent/src/StatusMonitor.h"

namespace TA_IRS_App
{

	TTISPredefinedDisplayRequest::TTISPredefinedDisplayRequest()     
	{

	}

	TTISPredefinedDisplayRequest::~TTISPredefinedDisplayRequest()
	{


	}

	void TTISPredefinedDisplayRequest::log(bool sendSuccess)
	{
		TA_ASSERT(m_dataIsSet, "Log() called for Predefined display request that has no data");

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
		
        //  create a new entry in the database
		TA_IRS_Core::IDisplayRequest* displayRequest = TA_IRS_Core::DisplayRequestAccessFactory::getInstance().createNewDisplayRequest();
		
		displayRequest->setMessageTag( m_messageTag );
		displayRequest->setLibrarySection( m_librarySection );
		displayRequest->setLibraryVersion( StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion() );
		displayRequest->setDestinationSystem( "TTIS" );
		displayRequest->setStartTime( m_startTime );
		displayRequest->setEndTime( m_endTime );
		displayRequest->setPriority( m_priority );
		displayRequest->setTimestamp( m_timestamp );
		displayRequest->setMessageType( TA_IRS_Core::IDisplayRequest::PREDEFINED );
		displayRequest->setSubmittedBy( m_submittedBy );
		//hongran++ TD17502
		displayRequest->setIfSendSuccess(TA_Base_Core::Pass);
		//++hongran TD17502
		
		// set the adhoc content to the predefined message text in case this library is
		// upgraded later - they will still be able to see what was sent.
		displayRequest->setAdhocMessageContent(
			StatusMonitor::getInstance()->getTrainPredefinedMessageText( m_librarySection, m_messageTag ) );
		
		std::stringstream trainNumber;
		trainNumber << m_trainNumber;
		
		displayRequest->setTtisDestinationTrainId( trainNumber.str() );
		
		displayRequest->setTtisDestinationPids( pidStream.str() );
		
		// write this display request to the database
		displayRequest->applyChanges();
		
		delete displayRequest;
		displayRequest = NULL;       
	}


	void TTISPredefinedDisplayRequest::setData( CORBA::UShort trainNumber, const TA_Base_Core::TrainPIDList& pidList, 
		TA_Base_Core::ELibrarySection librarySection, unsigned short libraryVersion,	CORBA::UShort messageID,
		CORBA::UShort priority, const char* startTime, const char* endTime, const std::string& operatorName )
	{
		m_trainNumber = trainNumber;
		m_librarySection = librarySection;
		m_libraryVersion = libraryVersion;
		m_messageTag = messageID;
		m_priority = priority;
		m_startTime = startTime;
		m_endTime = endTime;
		m_submittedBy = operatorName;
   

		unsigned int i;

		for( i = 0 ; i < pidList.length() ; i++ )
		{
			m_pidSelectionList.push_back( pidList[i] );
		}

		m_dataIsSet = true;

	}
	
}