/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/signalling/SignallingProtocolLibrary/src/Telegram.cpp $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2010/11/25 09:26:41 $
  * Last modified by:  $Author: raghu.babu $
  *
  * Base class for AckTelgram, ConnectConfirmationTelegram, 
  * ThirdRailTelegram and VariableFieldTelegram
  */

// for "ntohs" function
#if !defined( WIN32 )
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

#include <time.h>
#include <sstream>

#include "core/utilities/src/DebugUtil.h"

#include "bus/signalling/SignallingProtocolLibrary/src/Telegram.h"
#include "bus/signalling/SignallingProtocolLibrary/src/TelegramException.h"
#include "bus/signalling/SignallingProtocolLibrary/src/HelperLogs.h"

//Private Variables
const ta_uint16 Telegram::RESERVED_1_WORD_POSITION          = 4;
const ta_uint16 Telegram::RESERVED_2_WORD_POSITION          = 6;
const ta_uint16 Telegram::INTERFACE_NUMBER_BYTE_POSITION    = 8;
const ta_uint16 Telegram::MESSAGE_TYPE_BYTE_POSITION        = 9;
const ta_uint16 Telegram::ERROR_CODE_BYTE_POSITION          = 10;
const ta_uint16 Telegram::RESERVED_3_BYTE_POSITION          = 11;
const ta_uint16 Telegram::DWORD_SIZE                        = 2;

//Protected Variables
const ta_uint16 Telegram::MAX_PACKET_LENGTH                 = 6144;

//Public Variables
const ta_uint16 Telegram::HEADER_LENGTH                     = 12;
const ta_uint16 Telegram::CONN_HEADER_LENGTH                = 8;


// Header
const ta_uint8	Telegram::HEADER_SYSTEM_ID					= 0;	
const ta_uint16 Telegram::HEADER_TOTAL_LEN					= 1;
const ta_uint8	Telegram::HEADER_MULTI_FLAG					= 3;
const ta_uint16	Telegram::SIG_HEADER_LEN					= 4;


// Packet Data Message (Common for all SIG Data)
const ta_uint16 Telegram::LENGTH_WORD_POSITION              = 0;
const ta_uint16 Telegram::LENGTH_MSG_POSITION               = 4;	// this is use on out going messages
const ta_uint16 Telegram::IDENTIFICATION_WORD_POSITION      = 2;

// HELPER CONSTANTS
const ta_uint8 Telegram::MSG_LENGTH_SIZE      = 2;

//SIG
const ta_uint16 Telegram::SIG_MAX_PACKET_LENTH = 1028; // Max. Packet per frame
const ta_uint8 Telegram::SIG_HEADER_SIZE      = 3;    // SystemID(1 Byte) + TotalLength(2 Bytes)  
const ta_uint8 Telegram::SIG_MULTIFLAG_POS    = 3;    // MultiFlag Position
const ta_uint8 Telegram::SIG_MESSAGE_ID_POS   = 6;    // Messag Identification Position
const ta_uint8 Telegram::SIG_MSG_LEN_POS      = 4;

// CONSTANT for MULTIFLAG
#define MAX_ALLOWED_DATA_LEN_ON_FRAME	1025
#define SIG_SYSTEM_ID				0xFF

#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

//Public Methods///////////////////////////////////////////////////////////////
Telegram::Telegram(std::vector<unsigned char>& incomingData, bool bEnableExceptionMechanism)
//:m_rawData(incomingData),
:m_bHasError(false)
{
	//FUNCTION_ENTRY("Telegram Constructor(std::vector<unsigned char>& incomingData)");  

	if ( true == bEnableExceptionMechanism )
	{
		//Make sure that the m_rawData is not less than the Header size
		//Commented : The exception case was moved to IncompletePacket case condition
// 		if ( m_rawData.size() < SIG_HEADER_LEN )
// 		{
// 			TA_THROW(TelegramException
// 				(TelegramException::InvalidHeaderSize, "Invalid Header Size.", 
// 				"Invalid Header Size"));
// 		}

		unsigned int ActualDataSize     = incomingData.size();
		if (ActualDataSize<SIG_HEADER_LEN)
		{
			TA_THROW(TelegramException
				(TelegramException::IncompletePacket, "Incomplete Packet Length.", 
				"Incomplete Packet Length")); 
		}
		else 
		{
			//just copy header data for performance reason
			m_rawData.insert(m_rawData.begin(), incomingData.begin(), incomingData.begin() + SIG_HEADER_LEN );
		}

		unsigned int StatedPacketLength = getLength();		
		bool bCutData = false;
		unsigned char multiflag = getMultiFlag();

		//Check if the SystemID is valid.
		//If Invalid, throw an InvalidSystemID Exception
		unsigned char SystemID = getByte(HEADER_SYSTEM_ID);
		if ( SIG_SYSTEM_ID != SystemID )
		{
			TA_THROW(TelegramException
				(TelegramException::InvalidSystemID, "Invalid System ID.", 
				"Invalid System ID"));    
		}

		//Handling of IncompletePacket Exception case condition
		//This will throw an exception if the data is not enough
		if( (ActualDataSize < (StatedPacketLength + SIG_HEADER_SIZE)) )
		{
			TA_THROW(TelegramException
				(TelegramException::IncompletePacket, "Incomplete Packet Length.", 
				"Incomplete Packet Length"));    
		}

		//Needs to be cleared to reassign a new value based on the Stated Packet Length
		m_rawData.clear();
		m_rawData.reserve(StatedPacketLength + SIG_HEADER_SIZE);


		//Make sure that the cache contains exact data based on the Stated Packet Length
		//Insert the first telegram into m_rawdata based on the number of Length
		m_rawData.insert(m_rawData.begin(), incomingData.begin(), incomingData.begin() + StatedPacketLength + SIG_HEADER_SIZE );

		//Remove the telegram data from the incoming vector
		incomingData.erase(incomingData.begin(), incomingData.begin() + StatedPacketLength + SIG_HEADER_SIZE );
	}
	else
	{
		// no checking for msg specific items
		// msg specific will be handled on on the
		// telegram specific class
		//incomingData.erase(incomingData.begin(),incomingData.end());
		m_rawData.insert(m_rawData.begin(), incomingData.begin(), incomingData.begin() + incomingData.size() );
		incomingData.clear();
	}
	

	setTimeStamp();

	//FUNCTION_EXIT;
}

Telegram::~Telegram()
{
    //FUNCTION_ENTRY("~Telegram");
    //FUNCTION_EXIT;
}

ta_uint16 Telegram::getLength() const
{    
    //FUNCTION_ENTRY("getLength");
    //FUNCTION_EXIT;
    return getWord(HEADER_TOTAL_LEN);
}

ta_uint16 Telegram::getMsgLength() const
{    
	//FUNCTION_ENTRY("getLength");
	//FUNCTION_EXIT;
	return getWord(LENGTH_WORD_POSITION);
}

std::vector<unsigned char> Telegram::getPacketData()
{
    //FUNCTION_ENTRY("getPacketData");
    //FUNCTION_EXIT;
    return m_rawData;
}

Telegram::ETelegramIdentification Telegram::getIdentification() const
{
    //FUNCTION_ENTRY("getIdentification");

    ETelegramIdentification ret = UNKNOWN_TELEGRAM;

    ta_uint16 iIdentification = getWord(IDENTIFICATION_WORD_POSITION); 

    switch(iIdentification) 
    {

	case MESSAGE_POLLING:
		ret = MESSAGE_POLLING;
		break;

	case ATS_ISCS_TRAIN_LOCATION:
		ret = ATS_ISCS_TRAIN_LOCATION;
		break;

	case ATS_ISCS_BLOCK_INFO:
		ret = ATS_ISCS_BLOCK_INFO;
		break;

	case ATS_ISCS_DYNAMIC_TRAIN_DATA:
		ret = ATS_ISCS_DYNAMIC_TRAIN_DATA;
		break;

	case ATS_ISCS_SIGNAL_STATUS:
		ret = ATS_ISCS_SIGNAL_STATUS;
		break;

	case ATS_ISCS_TRAIN_PIS_INFO:
		ret = ATS_ISCS_TRAIN_PIS_INFO;
		break;

	case ISCS_PLANNED_TRAIN_DATA:
		ret = ISCS_PLANNED_TRAIN_DATA;
		break;

	case ISCS_PRACTICAL_TRAIN_DATA:
		ret = ISCS_PRACTICAL_TRAIN_DATA;
		break;
        
	default:
		{
            ostringstream msg;
            msg << "The ETelegramIdentification " << iIdentification 
                << " is incorrect."
                <<  ends;
	       
			setHasError(true);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"getIdentification : ERROR , ID : %d",iIdentification);

			TA_THROW(TelegramException
                (TelegramException::InvalidValue, msg.str(), 
                "Telegram Identification"));

        }
    }

    //FUNCTION_EXIT;
    return ret;
}


Telegram::ETelegramIdentification Telegram::getIdentification(std::vector <unsigned char> & vctrData) const
{
	//FUNCTION_ENTRY("getIdentification");

	ETelegramIdentification ret = UNKNOWN_TELEGRAM;

	ta_uint16 iIdentification = getWord(IDENTIFICATION_WORD_POSITION,vctrData); 

	switch(iIdentification) 
	{

	case MESSAGE_POLLING:
		ret = MESSAGE_POLLING;
		break;

	case ATS_ISCS_TRAIN_LOCATION:
		ret = ATS_ISCS_TRAIN_LOCATION;
		break;

	case ATS_ISCS_BLOCK_INFO:
		ret = ATS_ISCS_BLOCK_INFO;
		break;

	case ATS_ISCS_DYNAMIC_TRAIN_DATA:
		ret = ATS_ISCS_DYNAMIC_TRAIN_DATA;
		break;

	case ATS_ISCS_SIGNAL_STATUS:
		ret = ATS_ISCS_SIGNAL_STATUS;
		break;

	case ATS_ISCS_TRAIN_PIS_INFO:
		ret = ATS_ISCS_TRAIN_PIS_INFO;
		break;

	case ISCS_PLANNED_TRAIN_DATA:
		ret = ISCS_PLANNED_TRAIN_DATA;
		break;

	case ISCS_PRACTICAL_TRAIN_DATA:
		ret = ISCS_PRACTICAL_TRAIN_DATA;
		break;

	default:
		{
			ostringstream msg;
			msg << "The ETelegramIdentification " << iIdentification 
				<< " is incorrect."
				<<  ends;
			
			setHasError(true);
			
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"getIdentification : ERROR , ID : %d",iIdentification);

			TA_THROW(TelegramException
				(TelegramException::InvalidValue, msg.str(), 
				"Telegram Identification"));                    
		}
	}

	//FUNCTION_EXIT;
	return ret;
}


Telegram::EInterfaceType Telegram::getInterface() const
{
    //FUNCTION_ENTRY("getInterface");
    
    unsigned char cInterfaceNum = getByte(INTERFACE_NUMBER_BYTE_POSITION);

    EInterfaceType ret = static_cast<EInterfaceType>(cInterfaceNum);

    if( (ret != PtiInterface) &&
        (ret != ThirdRailInterface) &&
        (ret != RadioInterface) &&
        (ret != CongestionInterface) &&
        (ret != PidsInterface))
    {
        ostringstream msg;
        msg << "The EInterfaceType " << cInterfaceNum 
            << " is incorrect."
            <<  ends;
		
		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Interface Type"));                        
    }
    
    //FUNCTION_EXIT;
    return ret;
}

Telegram::EMessageType Telegram::getMessageType() const
{
    //FUNCTION_ENTRY("getMessageType");
    
    unsigned char cMessageType = getByte(MESSAGE_TYPE_BYTE_POSITION); 

    EMessageType ret = static_cast<EMessageType>(cMessageType);

    if( (ret != SpontaneousMessage) &&
        (ret != AckMessage))
    {
        ostringstream msg;
        msg << "The EMessageType " << cMessageType 
            << " is incorrect."
            <<  ends;
		
		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Message Type"));                        
    }

    //FUNCTION_EXIT;
    return ret;
}

Telegram::EErrorCode Telegram::getErrorCode() const
{
    //FUNCTION_ENTRY("getErrorCode");
    
    unsigned char cErrorCode = getByte(ERROR_CODE_BYTE_POSITION); 

    EErrorCode ret = static_cast<EErrorCode>(cErrorCode);

    if( (ret != NoError) &&
        (ret != UnknownInterface) &&
        (ret != InterfaceNotReachable) &&
        (ret != OtherError))
    {
        ostringstream msg;
        msg << "The EErrorCode " << cErrorCode 
            << " is incorrect."
            <<  ends;
	    TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Error Code"));                    
    }

    //FUNCTION_EXIT;
    return ret;
}

void Telegram::setErrorCode(EErrorCode errorCode)
{
    //FUNCTION_ENTRY("setErrorCode");

    unsigned char value = static_cast<unsigned char>(errorCode);
        
    setByte(ERROR_CODE_BYTE_POSITION, value);

    //FUNCTION_EXIT;
}

time_t Telegram::getTimeStamp() const
{
    //FUNCTION_ENTRY("getTimeStamp");
    //FUNCTION_EXIT;
    return m_timeStamp;
}

std::vector<unsigned char>& Telegram::getHeader()
{
    //FUNCTION_ENTRY("getHeader");

    ta_uint16 msgLength = m_rawData.size();
    
    if(msgLength < HEADER_LENGTH)
    {
        ostringstream msg;
        msg << "Message Length "
            << msgLength
            << " is not equal to the data packet length " 
            << HEADER_LENGTH
            << ends;

		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Message Length"));    
    }
   
    m_header.assign( (m_rawData.begin() + LENGTH_WORD_POSITION), 
                     (m_rawData.begin() + LENGTH_WORD_POSITION + HEADER_LENGTH));

    //FUNCTION_EXIT;
	return m_header;
}

std::vector<unsigned char>& Telegram::getConnHeader()
{
    //FUNCTION_ENTRY("getConnHeader");
	
    ta_uint16 msgLength = m_rawData.size();
    
    if(msgLength != CONN_HEADER_LENGTH)
    {
        ostringstream msg;
        msg << "Message Length "
            << msgLength
            << " is not equal to the connection packet length " 
            << CONN_HEADER_LENGTH
            << ends;

		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Message Length"));    
    }
    
    m_header.assign(    (m_rawData.begin() + LENGTH_WORD_POSITION), 
                        (m_rawData.begin() + LENGTH_WORD_POSITION + CONN_HEADER_LENGTH));
    
    //FUNCTION_EXIT;
	return m_header;
}

//Protected Methods////////////////////////////////////////////////////////////
Telegram::Telegram(ETelegramIdentification identification, EInterfaceType interfaceType, EMessageType messageType, EErrorCode errorCode)
{
    //FUNCTION_ENTRY("Telegram Constructor(ETelegramIdentification, EInterfaceType, EMessageType, EErrorCode");

    //Populate the m_rawData for the header
    m_rawData.resize(HEADER_LENGTH);
    setLength(m_rawData.size());

    setTimeStamp();

    setTelegramIdentification(identification);
    setInterface(interfaceType);
    setMessageType(messageType);
    setErrorCode(errorCode);

    //FUNCTION_EXIT;
}

// msgLength does not include length size
// telegramSize do include length size
Telegram::Telegram(ta_uint16 msgLength, ETelegramIdentification identification, ta_uint16 telegramSize)
{
	//FUNCTION_ENTRY("Telegram Constructor(ta_uint16 msgLength, ETelegramIdentification identification");
	
	// resize the handler base on the mssge size 
	// plus the header size
	m_rawData.resize(telegramSize + SIG_HEADER_LEN);
	
	// write the header for the message
	// siince the msgLenth already ommit the legth
	 // we should use the total lenght of the message packaet data
	// which is the telegram size
	setSigHeader(telegramSize);
	
	// setting length on this item 
	// assumes that the length has been subtracted
	setMsgLength(msgLength);

	setTelegramIdentification(identification);

	//FUNCTION_EXIT;
}

Telegram::Telegram(ta_uint16 msgLength, ETelegramIdentification identification)
{
	//FUNCTION_ENTRY("Telegram");
	
	// resize the handler base on the mssge size 
	// plus the header size
	m_rawData.resize(msgLength + SIG_HEADER_LEN);
	
	// write the header for the message
	setSigHeader(msgLength);

	// setting of the message length
	// for specific telegram e.g. ScadaInfo
	// subtract 2 since the message length includes the 
	// size of the lenth for the message
	setMsgLength(msgLength - MSG_LENGTH_SIZE);	

	setTelegramIdentification(identification);
	//FUNCTION_EXIT;
}

Telegram::Telegram(const Telegram& rhs)
{
    //FUNCTION_ENTRY("Telegram Copy Constructor");

    m_rawData = rhs.m_rawData;    
    m_timeStamp = rhs.m_timeStamp;

    //FUNCTION_EXIT;
}

void Telegram::setMessageType(EMessageType messageType)
{
    //FUNCTION_ENTRY("setMessageType");

    unsigned char value = static_cast<EMessageType>(messageType);
        
    setByte(MESSAGE_TYPE_BYTE_POSITION, value);

    //FUNCTION_EXIT;
}

void Telegram::setTelegramIdentification(ETelegramIdentification identification)
{
    //FUNCTION_ENTRY("setTelegramIdentification");

    setWord(IDENTIFICATION_WORD_POSITION + SIG_HEADER_LEN, identification);

    //FUNCTION_EXIT;
}

void Telegram::setMessageByte(ta_uint16 position, unsigned char value)
{
    //FUNCTION_ENTRY("setMessageByte");

    setByte(position + HEADER_LENGTH, value);

    //FUNCTION_EXIT;
}

void Telegram::setMessageBytes(ta_uint16 position, const std::vector<unsigned char>& values)
{
    //FUNCTION_ENTRY("setMessageBytes");

    ta_uint16 lastPos = values.size() - 1 + position;

    if(lastPos > (MAX_PACKET_LENGTH - HEADER_LENGTH - 1))
    {
        ostringstream msg;
        msg << "Position " 
            << position
            << " exceeds Maximum Packet Data length " 
            << MAX_PACKET_LENGTH
            << ends;
        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Position"));        
    }

    setBytes(position + HEADER_LENGTH, values);

    //FUNCTION_EXIT;
}

void Telegram::setMessageWord(ta_uint16 position, ta_uint16 value)
{
    //FUNCTION_ENTRY("setMessageWord");

    //The position must be on a word boundary, e.g. an even number
    if((position & 1) == 0)
    {
        setWord(position + HEADER_LENGTH, value);        
    }
    else
    {
        ostringstream msg;
        msg << " The position " 
            << position 
            << " is not of word boundary" 
            << ends;
        TA_THROW(TelegramException(TelegramException::InvalidValue,msg.str(), 
            "Position"));                    
    }

    //FUNCTION_EXIT;
}

void Telegram::setMessageWords(ta_uint16 position, const std::vector<ta_uint16>& values)
{
    //FUNCTION_ENTRY("setMessageWords");

    ta_uint16 lastPos = (values.size() * 2) - 1 + position;

    if(lastPos > (MAX_PACKET_LENGTH - HEADER_LENGTH - 1))
    {
        ostringstream msg;
        msg << "Position " 
            << position
            << " exceeds above the maximum allowable packet size."
            << ends;
        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Position"));        
    }

    //The position must be on a word boundary, e.g. an even number
    if((position & 1) == 0)
    {
        setWords(position + HEADER_LENGTH, values);        
    }
    else
    {
        ostringstream msg;
        msg << " The position " 
            << position 
            << " is not of word boundary" 
            << ends;

		setHasError(true);

        TA_THROW(TelegramException(TelegramException::InvalidValue,msg.str(), 
            "Position"));                    
    }

    //FUNCTION_EXIT;
}

unsigned char Telegram::getMessageByte(ta_uint16 position) const
{
    //FUNCTION_ENTRY("getMessageByte");
    
    unsigned char ret;

    if(position > (MAX_PACKET_LENGTH - HEADER_LENGTH - 1))
    {
        ostringstream msg;
        msg << " The position " 
            << position 
            << " is greater than max. length (6144 - 12 - 1 = 6131)" 
            << ends;

		setHasError(true);

        TA_THROW(TelegramException(TelegramException::InvalidValue,msg.str(),
            "Position"));                    
    }
    
    ret = getByte(position + HEADER_LENGTH);
    
    //FUNCTION_EXIT;
    return ret;
}

std::vector<unsigned char> Telegram::getMessageBytes(ta_uint16 position, ta_uint16 length) const
{
    //FUNCTION_ENTRY("getMessageBytes");
    
    vector<unsigned char> ret;

    if(position > (MAX_PACKET_LENGTH - HEADER_LENGTH - 1))
    {
        ostringstream msg;
        msg << " The position " 
            << position 
            << " is greater than max. length (6144 - 12 - 1 = 6131)" 
            << ends;

		setHasError(true);

        TA_THROW(TelegramException(TelegramException::InvalidValue,msg.str(),
            "Position"));                        
    }

    if(length <= 1)
    {
        ostringstream msg;
        msg << " The length " 
            << length
            << " is less than or equal to 1" 
            << ends;

		setHasError(true);

        TA_THROW(TelegramException(TelegramException::InvalidValue,msg.str(),
            "Length"));                        
    }

    if(length > (MAX_PACKET_LENGTH - HEADER_LENGTH - 1))
    {
        ostringstream msg;
        msg << " The length " 
            << position 
            << " is greater than max. length (6144 - 12 - 1 = 6131)" 
            << ends;

		setHasError(true);

        TA_THROW(TelegramException(TelegramException::InvalidValue,msg.str(),
            "Length"));                        
    }

    //ret = getBytes(position + HEADER_LENGTH, length);
	//Lucky
	ret = getBytes(position, length);
            
    //FUNCTION_EXIT;
    return ret;
}

ta_uint16 Telegram::getMessageWord(ta_uint16 position) const
{
    //FUNCTION_ENTRY("getMessageWord");
    
    ta_uint16 ret;
    
    if(position > (MAX_PACKET_LENGTH - HEADER_LENGTH - 1))
    {
        ostringstream msg;
        msg << " The position " 
            << position 
            << " is greater than max. length (6144 - 12 - 1 = 6131)" 
            << ends;

		setHasError(true);

        TA_THROW(TelegramException(TelegramException::InvalidValue,msg.str(),
            "Position"));                            
    }

    //The position must be on a word boundary, e.g. an even number
    if((position & 1) == 0)
    {
        ret = getWord(position + HEADER_LENGTH);           
    }    
    else
    {
        ostringstream msg;
        msg << " The position " 
            << position 
            << " is not of word boundary" 
            << ends;

		setHasError(true);

        TA_THROW(TelegramException(TelegramException::InvalidValue,msg.str(),
            "Position"));                    
    }

    //FUNCTION_EXIT;
    return ret;
}

std::vector<ta_uint16> Telegram::getMessageWords(ta_uint16 position, ta_uint16 length) const
{
    //FUNCTION_ENTRY("getMessageWords");
    
    vector<ta_uint16> ret;

    if(position > (MAX_PACKET_LENGTH - HEADER_LENGTH - 1))
    {
        ostringstream msg;
        msg << " The position " 
            << position 
            << " is greater than max. length (6144 - 12 - 1 = 6131)" 
            << ends;

		setHasError(true);

        TA_THROW(TelegramException(TelegramException::InvalidValue,msg.str(),
            "Position"));                        
    }
    
    if(length > (MAX_PACKET_LENGTH - HEADER_LENGTH - 1))
    {
        ostringstream msg;
        msg << " The length " 
            << position 
            << " is greater than max. length (6144 - 12 - 1 = 6131)" 
            << ends;

		setHasError(true);

        TA_THROW(TelegramException(TelegramException::InvalidValue,msg.str(),
            "Length"));                        
    }
    
    //The position must be on a word boundary, e.g. an even number
    if((position & 1) == 0)
    {
        ret = getWords(position + HEADER_LENGTH, length);        
    }
    else
    {
        ostringstream msg;
        msg << " The position " 
            << position 
            << " is not of word boundary" 
            << ends;

		setHasError(true);

        TA_THROW(TelegramException(TelegramException::InvalidValue,msg.str(),
            "Position"));                    
    }
        
    //FUNCTION_EXIT;
    return ret;
}


//Private Methods//////////////////////////////////////////////////////////////
void Telegram::setLength(ta_uint16 length)
{
    //FUNCTION_ENTRY("setLength");

    setWord(LENGTH_WORD_POSITION, length);

    //FUNCTION_EXIT;
}

void Telegram::setInterface(EInterfaceType interfaceType)
{
    //FUNCTION_ENTRY("setInterface");

    unsigned char value = static_cast<unsigned char>(interfaceType);

    setByte(INTERFACE_NUMBER_BYTE_POSITION, value);

    //FUNCTION_EXIT;
}

void Telegram::setByte(ta_uint16 position, unsigned char value)
{
    //FUNCTION_ENTRY("setByte");

    if(position > MAX_PACKET_LENGTH-1)
    {
        ostringstream msg;
        msg << "Position " 
            << position
            << " exceeds Maximum Packet Data length " 
            << MAX_PACKET_LENGTH
            << ends;

		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Position"));    
    }
    
    // grow if cannot fit
    if(position > m_rawData.size()-1) 
    {
        // add 1 to cater for 0th element
        m_rawData.resize(position + 1); 
        setLength(m_rawData.size());
    }
    
    m_rawData[position] = value;

    //FUNCTION_EXIT;
}

void Telegram::setBytes(ta_uint16 position, const std::vector<unsigned char>& values)
{
    //FUNCTION_ENTRY("setBytes");

    //position must be within 0 to MAX_PACKET_LENGTH
    if(position > MAX_PACKET_LENGTH - 1)
    {
        ostringstream msg;
        msg << "Position " 
            << position
            << " exceeds above Maximum Packet Data length "
            << ends;

		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Position"));    
    }
    
    ta_uint16 dataSize = values.size();

    //check that the values size DID NOT exceed the MAX_PACKET_LENGTH
    if (dataSize > MAX_PACKET_LENGTH)
    {
        ostringstream msg;
        msg << "Data Length (" 
            << dataSize
            << ") exceeds above Maximum Packet Data length "
            << ends;

		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Position"));    
    }

    ta_uint16 lastPos = values.size() + position - 1;

    //Check that after added the vector size plus position DID NOT exceed MAX_PACKET_LENGTH
    if (lastPos > MAX_PACKET_LENGTH - 1)
    {
        ostringstream msg;
        msg << "After added " 
            << values.size()
            << ", total size of telegram exceeds above Maximum Packet Data length "
            << ends;

		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Position"));    
    }

    //grow if the m_rawData cannot fit
    if(position >= m_rawData.size())
    {
        // add 1 to cater for 0th element
        m_rawData.resize(position + values.size()); 
        setLength(m_rawData.size());    
    }    

    //Remove the allocated content of m_rawData 
    //from the position to the end of the values vector    
    int j=0;
    for(int i=position; i<m_rawData.size(); ++i, ++j)
    {
        m_rawData[i] = values[j];
    }

    //FUNCTION_EXIT;
}

void Telegram::setWord(ta_uint16 position, ta_uint16 value)
{
    //FUNCTION_ENTRY("setWord");

    unsigned short size = m_rawData.size();
    
    if(position > MAX_PACKET_LENGTH - 1)
    {
        ostringstream msg;
        msg << "Position " 
            << position
            << " exceeds above Maximum Packet Data length "
            << ends;
		
		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Position"));    
    }

//     //The position must be on a word boundary, e.g. an even number
//     if((position & 1) != 0)
//     {
//         ostringstream msg;
//         msg << "Position " 
//             << position
//             << " must be on a word boundary."
//             << ends;
//         TA_THROW(TelegramException
//             (TelegramException::InvalidValue, msg.str(), 
//             "Position"));    
//     }

    // grow if cannot fit
    if(position >= m_rawData.size()) 
    {
        // add 1 to cater for 0th element
        // add 1 as it is a word
        m_rawData.resize(position + 2); 
        setLength(m_rawData.size());
    }

    m_rawData[position + 1]     = value&255; //Gets the lower byte of value
    m_rawData[position] = value>>8; //Gets the upper byte of value

    //FUNCTION_EXIT;
}

void Telegram::setWords(ta_uint16 position, const std::vector<ta_uint16>& values)
{
    //FUNCTION_ENTRY("setWords");

    //The position must be on a word boundary, e.g. an even number
    if((position & 1) != 0)
    {
        ostringstream msg;
        msg << "Position " 
            << position
            << " must be on a word boundary."
            << ends;

		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Position"));    
    }

    //position must be within 0 to MAX_PACKET_LENGTH
    if(position > MAX_PACKET_LENGTH - 1)
    {
        ostringstream msg;
        msg << "Position " 
            << position
            << " exceeds above Maximum Packet Data length "
            << ends;

		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Position"));    
    }
 
    ta_uint16 dataSize = values.size() * 2; //is word size.. so need to multiple by 2

    //check that the values size DID NOT exceed the MAX_PACKET_LENGTH
    if (dataSize > MAX_PACKET_LENGTH)
    {
        ostringstream msg;
        msg << "Data Length (" 
            << dataSize
            << ") exceeds above Maximum Packet Data length "
            << ends;

		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Position"));    
    }

    ta_uint16 lastPos = dataSize + position - 1;

    //Check that after added the vector size plus position DID NOT exceed MAX_PACKET_LENGTH
    if (lastPos > MAX_PACKET_LENGTH - 1)
    {
        ostringstream msg;
        msg << "After added " 
            << values.size()
            << ", total size of telegram exceeds above Maximum Packet Data length "
            << ends;

		setHasError(true);

        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Position"));    
    }

/*
    ta_uint16 numWords = values.size() * 2; //is word size.. so need to multiple by 2
    if(position > MAX_PACKET_LENGTH - numWords)
    {
        ostringstream msg;
        msg << "Position " 
            << position
            << " exceeds above Maximum Packet Data length "
            << ends;
        TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Position"));    
    }
*/    
    // grow if cannot fit
    if(position >= m_rawData.size()) 
    {
        // add 1 to cater for 0th element        
        m_rawData.resize(position + dataSize); 
        setLength(m_rawData.size());
    }
    
    vector<ta_uint16>::const_iterator sourceIter = values.begin();
    for(; sourceIter != values.end(); ++sourceIter)
    {
        ta_uint16 value = *sourceIter;
        
		//zhou yuan ++
        m_rawData[position +1]	= value&255; //Gets the lower byte of value
        m_rawData[position ]	= value>>8;  //Gets the upper byte of value
		//++zhou yuan
        position+=2;
    }    

    //FUNCTION_EXIT;
}

unsigned char Telegram::getByte(ta_uint16 position) const
{
    //FUNCTION_ENTRY("getByte");

    ta_uint16 msgLength = m_rawData.size();

    if(position > msgLength)
    {
        ostringstream msg;
        msg << "Position exceeds above message length " 
            << msgLength
            << ends;

		setHasError(true);

	    TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Message Length"));    
    }

    unsigned char ret;

    ret = m_rawData[position];
    
	//FUNCTION_EXIT;
    return ret;
}

std::vector<unsigned char> Telegram::getBytes(ta_uint16 position, ta_uint16 length) const
{
    //FUNCTION_ENTRY("getBytes");

    ta_uint16 msgLength = m_rawData.size();

    if( (position+length) > msgLength)
    {
        ostringstream msg;
        msg << "bytes requested "
            << (position+length)
            << "exceeds the packet length " 
            << m_rawData.size()
            << ends;

		setHasError(true);

	    TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Message Length"));    
    }
 
    vector<unsigned char> ret;

    ret.assign( (m_rawData.begin() + position), 
                (m_rawData.begin() + position + length));
    
    //FUNCTION_EXIT;
    return ret;
}

ta_uint16 Telegram::getWord(ta_uint16 position) const
{
    //FUNCTION_ENTRY("getWord");
    
    ta_uint16 msgLength = m_rawData.size();

	//zhou yuan++
	//check whether we have 2 byte to convert it to word
    if( position >= msgLength)
    {
        ostringstream msg;
        msg << "Position exceeds above message length " 
            << msgLength
            << ends;

	    TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Message Length"));    
    }
	
    ta_uint16 ret;

    unsigned char upperByte = *(m_rawData.begin() + position);
    unsigned char lowerByte = *(m_rawData.begin() + position + 1);

    ret = (upperByte << 8) + lowerByte;
    //++zhou yuan

    //FUNCTION_EXIT;
    return ret;
}

ta_uint32 Telegram::getDWord(ta_uint16 position) const
{
	//FUNCTION_ENTRY("getDWord");

	std::vector<ta_uint16> _dWord = getWords(position,DWORD_SIZE);
	ta_uint32 _ret = 0;
	ta_uint8 count = 0;
	std::vector<ta_uint16>::const_iterator iter = _dWord.begin();
	for ( iter; iter != _dWord.end(); ++iter)
	{
		if ( 0 == count )
		{
			_ret = *iter << 16;
			count++;
		}
		else
			_ret += *iter;
	}
	//FUNCTION_EXIT;
	return _ret;
}

std::vector<ta_uint16> Telegram::getWords(ta_uint16 position, ta_uint16 length) const
{
    //FUNCTION_ENTRY("getWords");

    /*ta_uint16 msgLength = m_rawData.size() - (length * 2);*/
	ta_uint16 msgLength = m_rawData.size() - 2;

    if(position > msgLength)
    {
        ostringstream msg;
        msg << "Position exceeds above message length " 
            << msgLength
            << ends;

		setHasError(true);

	    TA_THROW(TelegramException
            (TelegramException::InvalidValue, msg.str(), 
            "Message Length"));    
    }
 

    vector<ta_uint16> ret;

    vector<unsigned char>::const_iterator sourceIter = (m_rawData.begin() + position);

    ta_uint16 iValue;
    ta_uint16 count = 0;

    while(  (count < length) && 
            (sourceIter != m_rawData.end()))             
    {
        unsigned char upperByte = *(sourceIter);
        unsigned char lowerByte = *(sourceIter + 1);
        
		iValue = (upperByte << 8) + lowerByte;
		        
        ret.insert((ret.begin() + count), iValue);

        //Move the counter
        ++count;
        sourceIter+=2;
    }
    
    //FUNCTION_EXIT;
    return ret;
}

ta_uint32 Telegram::getWord32(ta_uint16 position) const
{
	//FUNCTION_ENTRY("getWord32");

	ta_uint16 msgLength = m_rawData.size() - 2;

	if(position > msgLength)
	{
		ostringstream msg;
		msg << "Position exceeds above message length " 
			<< msgLength
			<< ends;
		TA_THROW(TelegramException
			(TelegramException::InvalidValue, msg.str(), 
			"Message Length"));    
	}


	vector<ta_uint16> ret;

	vector<unsigned char>::const_iterator sourceIter = (m_rawData.begin() + position);

	ta_uint32 _ret = 0;
	ta_uint32 _temp = 0;

	unsigned char high1 = *(sourceIter);
	unsigned char low1 = *(sourceIter + 1);
	unsigned char high0 = *(sourceIter + 2);
	unsigned char low0 = *(sourceIter + 3);
	_ret = (high1 << 8) + low1;
	_ret = _ret << 16;
	_temp = (high0 << 8) + low0;
	_ret += _temp;

	//FUNCTION_EXIT;
	return _ret;

}

void Telegram::setTimeStamp()
{
    //FUNCTION_ENTRY("setTimeStamp");

    time(&m_timeStamp);

    //FUNCTION_EXIT;    
}

bool Telegram::isValidTelegram()
{
	//FUNCTION_ENTRY("isValidTelegram");

	unsigned int StatedPacketLength = getLength();
	unsigned int StatedMsgLength = getMsgLength();

	unsigned int ActualDataSize     = m_rawData.size();

	bool _ret = false;
	
	
	// validate first the header if we do have the correct header
	if ( StatedPacketLength == (ActualDataSize - HEADER_MULTI_FLAG))
	{		
		
		//Check if the MsgID is a valid Identification	
		if ( UNKNOWN_TELEGRAM != getIdentification())
		{
			_ret = true;
		}
		else
		{
			setHasError(true);
		}
	}
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"isValidTelegram : %s",_ret?"VALID Telegram":"INVALID Telegram");
	//FUNCTION_EXIT;

	return _ret;
}

ta_uint16 Telegram::getSigHeaderLength ()
{
	//FUNCTION_ENTRY("getSigHeaderLength");	
	//FUNCTION_EXIT;

	return SIG_HEADER_LEN;
}

void Telegram::setSigHeader (ta_uint16 msgLength)
{
	//FUNCTION_ENTRY("setSigHeader");	
	
	setSigSystemId ();
	setSigTotalLen (msgLength);
	setSigMultiFlag (msgLength);
	
	//FUNCTION_EXIT;
}

void Telegram::setSigSystemId ()
{
	//FUNCTION_ENTRY("setSigSystemId");	
	
	setByte(HEADER_SYSTEM_ID,0xFF);
	
	//FUNCTION_EXIT;
}

void Telegram::setSigTotalLen (ta_uint16 msgLength)
{
	//FUNCTION_ENTRY("setSigSystemId");	

	setWord(HEADER_TOTAL_LEN,msgLength + 1 );	// msg length + 1 byte MF

	//FUNCTION_EXIT;
}

void Telegram::setSigMultiFlag (ta_uint16 msgLength)
{
	//FUNCTION_ENTRY("setSigSystemId");	

	ta_uint8 multiFlag = 0;
	
	if (msgLength == MAX_ALLOWED_DATA_LEN_ON_FRAME)
	{
		multiFlag = 1;
	}

	setByte(HEADER_MULTI_FLAG,multiFlag);	// msg length + 1 byte MF

	//FUNCTION_EXIT;
}


MessageContainer Telegram::getAllMessages ()
{
	//FUNCTION_ENTRY("getAllMessages");	
	//FUNCTION_EXIT;
	
	return m_messageContainer;
}

void Telegram::collectMessages ()
{
	//FUNCTION_ENTRY("collectMessages");	

	MessageData messageData;
	
	// trim the message packet alone

	if (m_rawData.size() < (HEADER_MULTI_FLAG + 1))
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Cannot Start Collecting message, size of raw data is invalid");
		//FUNCTION_EXIT;
		return;
	}

	messageData.insert(messageData.begin(),m_rawData.begin() + HEADER_MULTI_FLAG + 1 ,m_rawData.end());
	
 	while (0 != messageData.size())
	{
		
		MessageData messageEntry;

		long lLength = getWord(LENGTH_WORD_POSITION,messageData);
		
		if (validateMessage(messageData))
		{
			messageEntry.insert(messageEntry.begin(),
								messageData.begin(),
								messageData.begin() + lLength + MSG_LENGTH_SIZE);

			// total length + MSG_LENGTH_SIZE (the size of the msg len)
			messageData.erase(messageData.begin(),messageData.begin() + lLength + MSG_LENGTH_SIZE);

			m_messageContainer.push_back(messageEntry);

			messageEntry.clear();	
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Error encourter when validating message");

			ostringstream msg;
			msg << "Error Encounter On Telegram" 
				<< ends;
			
			TA_THROW(TelegramException
					(TelegramException::InvalidValue, msg.str(), 
					"Error Encounter On Telegram"));    
			break;
		}
	}

	//FUNCTION_EXIT;
}

bool Telegram::validateMessage (const MessageData & messageData)
{
	bool bReturn = true;
	try
	{
		std::vector <unsigned char> vctrData = messageData;
		getIdentification(vctrData);
		
		long lLength = getWord(LENGTH_WORD_POSITION,vctrData);
		
		if (lLength > messageData.size())
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Specified length is greater than the actual length");
			bReturn = false;
		}
	}
	catch (...)
	{
		bReturn = false;
	}

	return bReturn;
}

ta_uint16 Telegram::getWord(ta_uint16 position, MessageData & msgData) const
{

	//FUNCTION_ENTRY("getWord");

	ta_uint16 msgLength = msgData.size();

	//zhou yuan++
	//check whether we have 2 byte to convert it to word
	if( position >= msgLength)
	{
		ostringstream msg;
		msg << "Position exceeds above message length, MsgLen=" 
			<< msgLength<< " pos=" <<position
			<< ends;

		setHasError(true);

		TA_THROW(TelegramException
			(TelegramException::InvalidValue, msg.str(), 
			"Message Length"));    
	}

	ta_uint16 ret;

	unsigned char upperByte = *(msgData.begin() + position);
	unsigned char lowerByte = *(msgData.begin() + position + 1);

	ret = (upperByte << 8) + lowerByte;
	//++zhou yuan

	//FUNCTION_EXIT;
	return ret;
}

void Telegram::setMsgLength (ta_uint16 length)
{
	//FUNCTION_ENTRY("setLength");

	setWord(LENGTH_MSG_POSITION, length);

	//FUNCTION_EXIT;
}

bool Telegram::hasError ()
{
	//FUNCTION_ENTRY("hasError");
	//FUNCTION_EXIT;
	return m_bHasError;
}

void Telegram::setHasError (bool bValue) const
{
	//FUNCTION_ENTRY("setHasError");

	(const_cast<Telegram *> (this))->m_bHasError = bValue;

	//FUNCTION_EXIT;
}
unsigned char Telegram::getMultiFlag() const
{
	//FUNCTION_ENTRY("getMultiFlag");

	unsigned char multiFlag = getByte(HEADER_MULTI_FLAG);

	//FUNCTION_EXIT;
	return multiFlag;

}

std::string Telegram::getDataLog()
{
	//FUNCTION_ENTRY("getDataLog");

	std::vector<unsigned char>::iterator iter = m_rawData.begin();
	std::stringstream dataLog;
	for( iter ; iter != m_rawData.end() ; ++iter)
	{
		dataLog << "0x" << std::hex << static_cast<unsigned int>(*iter) << " ";
	}

	//FUNCTION_EXIT;
	return dataLog.str().c_str();

}

void Telegram::appendData( Telegram& telData )
{
	//FUNCTION_ENTRY("appendData");

	ta_uint16 oldTotalLength = 0;

	//Check if there is a old data from cache
	if ( ! m_rawData.empty())
	{
		oldTotalLength = getLength();
	}

	//Get current Total length from telData
	ta_uint16 currentTotalLength = telData.getLength();
	ta_uint8 multiFlag   = telData.getMultiFlag();
	std::vector<unsigned char> messageContent = telData.getMessageContent();

	ta_uint16 TotalLength = ( oldTotalLength - 1 ) + ( currentTotalLength - 1); //Multiflag will be added in setSigTotalLen

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Previous TotalLength : %d, Incoming Data TotalLength :%d",
								oldTotalLength, currentTotalLength );
	
	//insert data
	m_rawData.insert( m_rawData.end(),messageContent.begin(),messageContent.end() );

	setSigTotalLen(TotalLength);
	setSigMultiFlag(multiFlag);
	
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Total Length :%d, Message Content Length :%d, MessagID :0x%X, MessageData Size :%d",
		getWord(HEADER_TOTAL_LEN), getWord(SIG_MSG_LEN_POS), getWord(SIG_MESSAGE_ID_POS), getMessageContent().size());

	//FUNCTION_EXIT;

}

std::vector<unsigned char> Telegram::getMessageContent()
{
	//FUNCTION_ENTRY("getMessageContent");

	std::vector<unsigned char> messageContent;

	messageContent.insert(messageContent.begin(),m_rawData.begin() + HEADER_MULTI_FLAG + 1 ,m_rawData.end());

	//FUNCTION_EXIT;
	return messageContent;
}

Telegram::ETelegramIdentification Telegram::getSIGIdentification() const
{
	//FUNCTION_ENTRY("getSIGIdentification");

	ETelegramIdentification ret = UNKNOWN_TELEGRAM;

	ta_uint16 iIdentification = getWord(SIG_MESSAGE_ID_POS); 

	switch(iIdentification) 
	{

	case MESSAGE_POLLING:
		ret = MESSAGE_POLLING;
		break;

	case ATS_ISCS_TRAIN_LOCATION:
		ret = ATS_ISCS_TRAIN_LOCATION;
		break;

	case ATS_ISCS_BLOCK_INFO:
		ret = ATS_ISCS_BLOCK_INFO;
		break;

	case ATS_ISCS_DYNAMIC_TRAIN_DATA:
		ret = ATS_ISCS_DYNAMIC_TRAIN_DATA;
		break;

	case ATS_ISCS_SIGNAL_STATUS:
		ret = ATS_ISCS_SIGNAL_STATUS;
		break;

	case ATS_ISCS_TRAIN_PIS_INFO:
		ret = ATS_ISCS_TRAIN_PIS_INFO;
		break;

	case ISCS_PLANNED_TRAIN_DATA:
		ret = ISCS_PLANNED_TRAIN_DATA;
		break;

	case ISCS_PRACTICAL_TRAIN_DATA:
		ret = ISCS_PRACTICAL_TRAIN_DATA;
		break;

	default:
		{
			ostringstream msg;
			msg << "The ETelegramIdentification " << iIdentification 
				<< " is incorrect."
				<<  ends;

			setHasError(true);

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"getIdentification : ERROR , ID : %d",iIdentification);

			TA_THROW(TelegramException
				(TelegramException::InvalidValue, msg.str(), 
				"Telegram Identification"));                    
		}
	}

	//FUNCTION_EXIT;
	return ret;
}
