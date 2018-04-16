/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/STISAdHocDisplayRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "app/signs/tis_agent/src/STISAdHocDisplayRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "TISMessageDataDefinitionsHelper.h"

#include <iomanip>

namespace TA_IRS_App
{

    STISAdHocDisplayRequest::STISAdHocDisplayRequest()
            : STISMessage()
    {
        m_messageIDString = "M11";
    }


    STISAdHocDisplayRequest::~STISAdHocDisplayRequest()
    {
    }


    std::vector<unsigned char> STISAdHocDisplayRequest::createProtocolMessage()
    {
        std::vector<unsigned char> messageVector;
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
        dataStream << getMessageDestinationString( m_destination );

        // MESSAGE (MSGFREE) 'FIELDS'

        // Message Tag - this is actually the timestamp for M11 messages
        // format is YYMMDDHHMMSS as opposed to YYYYMMDDHHMMSS used everywhere else in the protocol
        for ( int i=2 ; i < m_timestamp.size() ; i++ )
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

        // Display effects & lcd font & led font setting
        dataStream << getDisplayAttributesAsString();

        // message length 001-255
        dataStream << std::setw( 3 ) << std::setfill( '0' ) << m_message.size();

        // The actual message
        dataStream << m_message;

        // Calculate the data length - this is variable
        std::string dataString = dataStream.str();
        int dataSize = dataString.size();

        // add the data size to the stream
        messageStream << std::setw( 4 ) << std::setfill( '0' ) << dataSize;

        // Add the data stream to the actual message stream after calculating it
        messageStream << dataString;

        // No train data

        std::string messageString = messageStream.str();
        unsigned long j;

        messageVector.push_back( 0x02 );
        for ( j=0;j < messageString.size() ; j++ )
        {
            messageVector.push_back( messageString[j] );
        }
        messageVector.push_back( 0x03 );

        return messageVector;
    }

    void STISAdHocDisplayRequest::setData( const TA_Base_Core::STISDestination& destination, const std::string& messageContent, const std::string& startTime,
                                           const std::string& endTime, unsigned short priority, const TA_Base_Core::DisplayAttributes& displayAttributes,
                                           const TA_Base_Core::LCDAttributesSTIS& lcdAttributes,
                                           const TA_Base_Core::LEDAttributes& ledAttributes,
                                           const std::string& operatorName  )
    {
        m_message = messageContent;
        m_destination = destination;
        m_startTime = startTime;
        m_endTime = endTime;
        m_priority = priority;
        m_displayAttributes = displayAttributes;
        m_submittedBy = operatorName;
		m_LCDAttributes = lcdAttributes;
		m_LEDAttributes = ledAttributes;

        // repeat interval should be 0 for a continuous message
        if ( m_priority < 4 )
        {
            m_displayAttributes.repeatInterval = 0;
        }

        m_dataIsSet = true;
    }


    std::string STISAdHocDisplayRequest::getDisplayAttributesAsString()
    {
        std::stringstream attributeStream;

		// Display mode
		attributeStream << TISMessageDataDefinitionsHelper::convertDisplayModeToChar(m_displayAttributes.displayMode);
		// Scroll speed
		attributeStream << TISMessageDataDefinitionsHelper::convertScrollSpeedToChar(m_displayAttributes.scrollSpeed);
        // Repeat interval, 3 bytes
        attributeStream << std::setw( 3 ) << std::setfill( '0' ) << m_displayAttributes.repeatInterval;
        // DisplayTime, 3 bytes
        attributeStream << std::setw( 3 ) << std::setfill( '0' ) << m_displayAttributes.displayTime;
        // Justification, 1 byte
        attributeStream << m_displayAttributes.justification;
        // Lcd font size and color
		attributeStream << TISMessageDataDefinitionsHelper::convertLcdFontSizeToChar(m_LCDAttributes.fontSize);
        attributeStream << m_LCDAttributes.fontColour;
        // Led font size and color, protocol from 1, but enum start from 0
		attributeStream << TISMessageDataDefinitionsHelper::convertLedFontSizeToChar(m_LEDAttributes.fontSize);
		attributeStream << TISMessageDataDefinitionsHelper::convertLedFontColourToChar(m_LEDAttributes.fontColour);

        return attributeStream.str();
    }
}