/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3002_TIP/3002/transactive/app/signs/pids_agent/src/PIDSAdHocDisplayRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include "app/signs/pids_agent/src/PIDSAdHocDisplayRequest.h"
#include "app/signs/pids_agent/src/PIDSManager.h"
#include "app/signs/pids_agent/src/RawTable.h"
//#include "core/data_access_interface/Pids_agent/src/DisplayRequestAccessFactory.h"
//#include "core/data_access_interface/Pids_agent/src/DisplayRequestData.h"

#include <iomanip>

namespace TA_IRS_App
{
	PIDSAdHocDisplayRequest::PIDSAdHocDisplayRequest()
		: PIDSMessage()
	{
	}


	PIDSAdHocDisplayRequest::~PIDSAdHocDisplayRequest()
	{
	}


	TA_Base_Bus::DataBlock<WORD> PIDSAdHocDisplayRequest::createProtocolMessage()
	{
		//int nLength = MESSAGE_HEADER_LENGTH + int((m_messageContent.length())/2+0.5);
		//RawTable rawTable(MODBUS_START_ADDRESS,nLength+MODBUS_START_ADDRESS-1);			//variable length
		RawTable rawTable(MODBUS_START_ADDRESS,MODBUS_END_ADDRESS);			//fixed length 1020
		rawTable.putBitToWord(0,STATION_SELECT_BIT_IN_WORD,m_stationSelected);	//Station selected
		rawTable.putBitToWord(0,TRAIN_SELECT_BIT_IN_WORD,m_trainSelected);	//Train selected
		rawTable.putBitToWord(0,STATION_MESSAGE_PRIORITY_BIT_IN_WORD,m_msgType);	//Message Priority
		rawTable.putBitToWord(0,STATION_DISPLAY_LOCATION_BIT_IN_WORD,m_showType);	//Display Location
		rawTable.putBitToWord(0,TRAIN_MESSAGE_PRIORITY_BIT_IN_WORD,m_msgType);	//Message Priority
		rawTable.putBitToWord(0,TRAIN_DISPLAY_LOCATION_BIT_IN_WORD,m_showType);	//Display Location
		rawTable.putBitToWord(0,STATION_CLEAR_MESSAGE_BIT_IN_WORD,0);			//Clear station Adhoc Message
		rawTable.putBitToWord(0,TRAIN_CLEAR_MESSAGE_BIT_IN_WORD,0);			//Clear train Adhoc Message
		if (m_trainNo > 0)
		{
			rawTable.putByte(0,m_trainNo);			//train No
		}
		else if(m_trainNo == 0)
		{
			rawTable.putBitToWord(0,ALL_TRAIN_MESSAGE_BIT_IN_WORD,1);			//Train Message
		}
		
		for (int i=0; i<m_destinationList.length();i++)		//Location
		{
			int byteIndex = (1 + (m_destinationList[i].station-1)/4)*2;
			int bitNum = ((m_destinationList[i].station-1)%4)*4;
			rawTable.putBitToWord(byteIndex,bitNum,m_destinationList[i].concourse);
			rawTable.putBitToWord(byteIndex,bitNum+1,m_destinationList[i].upTrack);
			rawTable.putBitToWord(byteIndex,bitNum+2,m_destinationList[i].downTrack);
			rawTable.putBitToWord(byteIndex,bitNum+3,m_destinationList[i].gateway);
		}

		for (int i=0; i < m_messageContent.length(); i += 2)
		{
		    // LSB is forward
			unsigned short value = m_messageContent[i+1]*256 + m_messageContent[i];
			rawTable.putWord(20+i, value);
		}
		return rawTable.getDataBlock();
/*		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number
		messageStream << getSequenceNumberAsString();

		// Add the timestamp to the message
		messageStream << m_timestamp;

		// Now the message ID
		messageStream << m_messageIDString; // M11

		// Now the actual data - this needs to be created separately
		// because the length of the data needs to be indicated 
		// before the data itself (i.e we need to calculate the size of it)

		std::stringstream dataStream;

		// Message destination (MSGDEST)
        dataStream << getMessageDestinationString(m_destination);

		// MESSAGE (MSGFREE) 'FIELDS'
		
		// Message Tag - this is actually the timestamp for M11 messages
		// format is YYMMDDHHMMSS as opposed to YYYYMMDDHHMMSS used everywhere else in the protocol
		for( int i=2 ; i < m_timestamp.size() ; i++ )
		{
			dataStream << m_timestamp[i];
		}

		// Start Time
		dataStream << m_startTime;

		// End Time
		dataStream << m_endTime;

		// Priority
		dataStream << m_priority;

		// DISPLAYEFFECT fields
		
		// Display Mode
		dataStream << getDisplayAttributesAsString();

		// PLASMAFONT fields
		dataStream << getPlasmaFontAttributesAsString();
		
		// LEDFONT fields
		dataStream << getLedFontAttributesAsString();

		// message length
		dataStream << std::setw(3) << std::setfill('0') << m_message.size();

		// The actual message
		dataStream << m_message;
		
		// Calculate the data length - this is variable
		std::string dataString = dataStream.str();
		int dataSize = dataString.size();

        // add the data size to the stream
		messageStream << std::setw(4) << std::setfill('0') << dataSize;
		
		// Add the data stream to the actual message stream after calculating it
		messageStream << dataString;	

		// No train data

		std::string messageString = messageStream.str();
		unsigned int j;

		messageVector.push_back( 0x02 );
		for(j=0;j < messageString.size() ; j++)
		{
			messageVector.push_back( messageString[j] );
		}
		messageVector.push_back( 0x03 );
				
		return messageVector;
*/
	}


	void PIDSAdHocDisplayRequest::log(bool sendSuccess)
	{
/*		TA_ASSERT(m_dataIsSet, "Log() called for Adhoc display request that has no data");

        TA_IRS_Core::IDisplayRequest* displayRequest = TA_IRS_Core::DisplayRequestAccessFactory::getInstance().createNewDisplayRequest();

		if( displayRequest )
		{
			displayRequest->setAdhocMessageContent( m_message );
			displayRequest->setDestinationSystem( "PIDS" );
			displayRequest->setStartTime( m_startTime );
			displayRequest->setEndTime( m_endTime );
			displayRequest->setPriority( m_priority );
			displayRequest->setTimestamp( m_timestamp );
            displayRequest->setMessageType( TA_IRS_Core::IDisplayRequest::ADHOC );
			displayRequest->setSubmittedBy( m_submittedBy );
			//hongran++ TD17502
			displayRequest->setIfSendSuccess(sendSuccess ? TA_Base_Core::Pass : TA_Base_Core::Fail);
			//++hongran TD17502

			displayRequest->setSPidsDestinationStation( m_destination.station.in() );
			
			std::stringstream levelStream;

			unsigned int i;
			for( i = 0 ; i < m_destination.levels.length() ; i++ )
			{
				// Add a space before the next level name
				// this ensures that we don't add one at the end
				if( i > 0 )
				{
					levelStream << " ";
				}

				levelStream << m_destination.levels[i].in();
			}

			displayRequest->setSPidsDestinationLevels( levelStream.str() );

			std::stringstream pidStream;
			for( i = 0 ; i < m_destination.pids.length() ; i++ )
			{
				// Add a space before the next level name
				// this ensures that we don't add one at the end
				if( i > 0 )
				{
					pidStream << " ";
				}

				pidStream << m_destination.pids[i].in();
			}

			std::string wtf = pidStream.str();

			displayRequest->setSPidsDestinationPids( pidStream.str() );

			displayRequest->setDisplayAttributes( m_displayAttributes );

			displayRequest->setPlasmaAttributes( m_plasmaAttributes );

			displayRequest->setLEDAttributes( m_LEDAttributes );

			// write this display request to the database
			displayRequest->applyChanges();

			delete displayRequest;
			displayRequest = NULL;
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Could not create new display request Database object");
		}
*/
	}


	void PIDSAdHocDisplayRequest::setData(TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList destinationList, 
		TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage messageContent, 
		::CORBA::Boolean msgType, 
		::CORBA::Boolean showType, 
		::CORBA::Long trainNo, 
		const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& startTime, 
		const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& endTime, 
		::CORBA::Long interval, 
		::CORBA::Boolean isCyclic)
	{
		m_messageContent = messageContent;
		m_destinationList = destinationList;
		m_msgType = msgType;
		m_showType = showType;
		m_trainNo = trainNo;
		m_startTime = startTime;
		m_endTime = endTime;
		m_interval = interval;
		m_isCyclic = isCyclic;

		if (m_destinationList.length() == 0)
		{
			m_stationSelected = false;
		} 
		else
		{
			m_stationSelected = true;
		}
		if (m_trainNo >=0 )
		{
			m_trainSelected = true;
		} 
		else
		{
			m_trainSelected = false;
		}

		m_dataIsSet = true;
	}

/*
	std::string PIDSAdHocDisplayRequest::getDisplayAttributesAsString()
	{
		std::stringstream attributeStream;

		attributeStream << (int)m_displayAttributes.displayMode;
	
		// Scroll Speed
		switch( m_displayAttributes.scrollSpeed )
		{
            case TA_Base_Core::SCROLL_SLOW:
				attributeStream << "1";
				break;

			case TA_Base_Core::SCROLL_MEDIUM:
				attributeStream << "8";
				break;

			case TA_Base_Core::SCROLL_FAST:
				attributeStream << "F";
				break;
		}

		// repeat interval
	    attributeStream << std::setw(3) << std::setfill('0') << m_displayAttributes.repeatInterval;

		// displayTime
		attributeStream << std::setw(3) << std::setfill('0') << m_displayAttributes.displayTime;

		// Justification
		attributeStream << m_displayAttributes.justification;

		return attributeStream.str();
	}


	std::string PIDSAdHocDisplayRequest::getPlasmaFontAttributesAsString()
	{
		std::stringstream plasmaFontStream;

        plasmaFontStream << std::hex << std::uppercase // display each attrib as a hex and uppercase if it is A-F
                        << m_plasmaAttributes.fontType
		                << m_plasmaAttributes.fontSize
                        << m_plasmaAttributes.fontColour
		                << m_plasmaAttributes.backgroundColour;

		return plasmaFontStream.str();
	}


	std::string PIDSAdHocDisplayRequest::getLedFontAttributesAsString()
	{
		std::stringstream ledFontStream;
		
		// This is the Font Type field and is always 0
		ledFontStream << "0";

		ledFontStream << m_LEDAttributes.fontSize;

		switch ( m_LEDAttributes.intensity )
		{
		case TA_Base_Core::INTENSITY_LOW:
			ledFontStream << "1";
			break;

		case TA_Base_Core::INTENSITY_MEDIUM:
			ledFontStream << "4";
			break;

		case TA_Base_Core::INTENSITY_HIGH:
			ledFontStream << "8";
			break;

		}

		ledFontStream << m_LEDAttributes.fontColour;


        // background font colour is not selectable.
        // It was removed from the protocol.

		return ledFontStream.str();
	}
*/

	
}