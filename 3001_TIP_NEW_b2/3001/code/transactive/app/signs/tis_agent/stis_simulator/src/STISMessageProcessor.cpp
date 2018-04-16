/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/stis_simulator/src/STISMessageProcessor.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * STISMessageProcessor is a thread which holds a queue of incoming STIS protocol packets.  The main method of the 
  * class loops, checking the queue for packets to be processed.
  */

#pragma warning (disable : 4786)

#include "ace/OS.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional>

#include "app/signs/tis_agent/stis_simulator/src/STISMessageProcessor.h"
#include "app/signs/tis_agent/stis_simulator/src/STISCommsObserver.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#define SIM_TYPE "SimType" //The simulator is a STIS or CCT simulator

namespace //anonymous
{
	using namespace TA_Base_Core;

    template<class PointerType>
    struct MatchingIdPredicate : public std::unary_function<PointerType, bool>
    {
        MatchingIdPredicate(std::string const& id) :
            m_idToMatch(id)
        {
        }

        bool operator()(PointerType* o) const
        {
            return(m_idToMatch==o->getId());
        }

        std::string m_idToMatch;
    };

	unsigned char const STX  = 0x02;	//This is the "Start Of Header" signifier.
    unsigned char const ETX         = 0x03; //This is the "End of Transmission" signifier.

    int const STX_SZ			= 1;            //The size (in bytes) of the STX field
    int const SQN_SZ			= 4;            //The size (in bytes) of the SQN field
    int const TIMESTAMP_SZ		= 14;           //The size (in bytes) of the Timestamp field
    int const MESSAGE_ID_SZ		= 3;            //The size (in bytes) of the Message ID field
    int const DATA_LENGTH_SZ	= 4;            //The size (in bytes) of the Data Length field
	int const START_TIME_SZ     = 14;			//The size (in bytes) of the Start Time field
	int const END_TIME_SZ       = 14;			//The size (in bytes) of the End Time field
	int const DESTINATION_STATION_SZ = 4;/* And 2 space add at end */	    //The size (in bytes) of the Destination station field
	int const DESTINATION_STATION_WITHSPACE_SZ = 6; 
	int const SYSTEM_ID_SZ      = 3;	        //The size (in bytes) of the System ID field
	int const NUMBER_PIDS_SZ    = 3;	        //The size (in bytes) of the Number of PIDs field
	int const PID_ADDRESS_SZ    = 3;	        //The size (in bytes) of a PID address field
	int const MESSAGE_TAG_SZ    = 12;    	    //The size (in bytes) of the message tag field
	int const PRIORITY_MASK_SZ  = 12;  	        //The size (in bytes) of the message tag field
	int const DISPLAY_MODE_SZ   = 1;    	    //The size (in bytes) of the display mode field
	int const SCROLL_SPEED_SZ   = 1;    	    //The size (in bytes) of the scroll speed field
	int const REPEAT_INTERVAL_SZ = 3;   	    //The size (in bytes) of the repeat interval field
	int const DISPLAY_TIME_SZ   = 3;	        //The size (in bytes) of the display time field
	int const JUSTIFICATION_SZ  = 1;	        //The size (in bytes) of the justification field
	int const PRIORITY_SZ       = 1;	        //The size (in bytes) of the Priority
    int const ETX_SZ            = 1;            //The size (in bytes) of the ETX field.
    int const RATIS_MESSAGE_LENGTH_SZ = 3;      //The size (in bytes) of the RATIS message length
    int const RATIS_VETTING_SZ  = 1;            //The size (in bytes) of the RATIS vetting field
    int const RATIS_CLEARING_SZ = 1;            //The size (in bytes) of the RATIS clearing field
    int const RATIS_OVERRIDE_SZ = 1;            //The size (in bytes) of the RATIS override field
    int const RATIS_PRIORITY_SZ = 1;            //The size (in bytes) of the RATIS override field

    //further constants can be added here as required.

	std::string numberVersionToStr(unsigned long version)
	{
		unsigned long l = version;
		unsigned char ch = 0;
		std::stringstream ss;	
		for(int i = 0; i < 4; ++i)
		{
			ch = (l >> (i*8));		
			ss << std::setfill('0') << std::setw(3) << (unsigned short)ch;		
		}
		return ss.str();	
	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_App
{

	STISMessageProcessor::STISMessageProcessor( TA_IRS_App::STISCommsObserver* parent )
		:	m_doThreadTerminate(false),
			m_threadIsRunning(false),
			m_parent(parent),
			m_localProcessor(false),
            m_ratisCommand(""),
			m_semaphone(0),
			m_isTimeScheduleDownloadComplete(true),
			m_isCCTsimulator(false),
			m_cctComputerStatus(0),
			m_currentTtisLibrary(0),
			m_newTtisLibrary(0)
	{
		FUNCTION_ENTRY("STISMessageProcessor");

		if(RunParams::getInstance().isSet(SIM_TYPE))
		{
			if(RunParams::getInstance().get(SIM_TYPE) == "CCT")
			{
				m_isCCTsimulator = true;
			}
		}

		start();

		FUNCTION_EXIT;
	}

	STISMessageProcessor::STISMessageProcessor()
		:   m_doThreadTerminate(false),
			m_threadIsRunning(false),
			m_parent(0),
			m_localProcessor(true),
            m_ratisCommand(""),
			m_semaphone(0),
			m_isCCTsimulator(false),
			m_cctComputerStatus(0),
			m_currentTtisLibrary(0),
			m_newTtisLibrary(0)
	{
		FUNCTION_ENTRY("STISMessageProcessor");

		if(RunParams::getInstance().isSet(SIM_TYPE))
		{
			if(RunParams::getInstance().get(SIM_TYPE) == "CCT")
			{
				m_isCCTsimulator = true;
			}
		}
		start();

		FUNCTION_EXIT;

	}

	STISMessageProcessor::~STISMessageProcessor()
	{
	

		FUNCTION_ENTRY("~STISMessageProcessor");

		m_statusFile.close();

		m_doThreadTerminate = true;
		// Post!
		m_semaphone.post();
		terminateAndWait();
		FUNCTION_EXIT;
	}


		//////////////////////////////////////////////////////////////////////////////
		//																			//
		//								Thread methods								//
		//																			//
		//////////////////////////////////////////////////////////////////////////////

	//
	//run
	//
	void STISMessageProcessor::run()
	{
		FUNCTION_ENTRY("STISMessageProcessor::run");
		m_threadIsRunning = true;

		while( false == m_doThreadTerminate )
		{
			if( false == m_incomingMessages.empty() )
			{
				//TD 15686
				//zhou yuan++
				std::vector< unsigned char > packet;
				{
					TA_Base_Core::ThreadGuard guard(m_incomingMessageLock);
					packet = m_incomingMessages.top();
					m_incomingMessages.pop();
				}
				//++zhou yuan
		
				//1. Check that we have a valid packet 
				if( true == isValidPacket(packet) )
				{
					//	1.1 Determine what type of packet we have received.
					char command[MESSAGE_ID_SZ + 1];
                
					//we want to begin copying after the SOH and PKT LEN fields, and end  after copying CMD_SZ bytes.
					std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ ), packet.begin()+( MESSAGE_ID_SZ + STX_SZ + SQN_SZ + TIMESTAMP_SZ ), command);

					command[MESSAGE_ID_SZ] = '\0';

					std::string commandString( command );
            
					if( 0 == strcmp(command, "M00") )
					{
						processConfigurationDataRequest(packet);
					}
					else if( 0 == strcmp(command, "M09") )
					{
						processTrainInformationRequest(packet);
					}
					else if( 0 == strcmp(command, "M10") )
					{
						processPredefinedDisplayRequest(packet);
					}
					else if( 0 == strcmp(command, "M11") )
					{
						processAdhocDisplayRequest(packet);
					}
					else if( 0 == strcmp(command, "M12") )             
					{
						processApprovedRATISDisplayRequest(packet);
					}
					else if( 0 == strcmp(command, "M20") )
					{
						processClearRequest(packet);          
					}
					else if( 0 == strcmp(command, "M21") )            
					{
						processPIDControlRequest(packet);
					}
					else if( 0 == strcmp(command, "M30") )            
					{
						processStationStatusRequest(packet);
					}
					else if( 0 == strcmp(command, "M31") )            
					{
						if(false == m_isCCTsimulator)
						{
						    processOCCStatusRequest(packet);
						}
						else
						{
							processCCTStatusRequest(packet);
						}
					}
					else if( 0 == strcmp(command, "M50") )            
					{
						processCurrentDisplayingMessageRequest(packet);
					}
					else if( 0 == strcmp(command, "M70") )            
					{
						processSTISUpgradePredefinedMessageLibraryRequest(packet);
					}
					else
					{
						// Send a NACK of 'undefined format'
						sendNack(packet,2);	
					}
					
				}
				else
				{
					// Send a NACK of 'undefined format'
					sendNack(packet,2);
				}				
				
			}

			//TD 15686
			//zhou yuan++
			m_semaphone.wait();
			//++zhou yuan
		}
		m_threadIsRunning = false;
		FUNCTION_EXIT;

	}


	//
	//terminate
	//
	void STISMessageProcessor::terminate()
	{
		FUNCTION_ENTRY("STISMessageProcessor::terminate");
		FUNCTION_EXIT;

	}


		//////////////////////////////////////////////////////////////////////////////
		//																			//
		//						STISMessageProcessor methods						//
		//																			//
		//////////////////////////////////////////////////////////////////////////////

	//
	//addSwitchRequest
	//
	void STISMessageProcessor::addMessageRequest( std::vector< unsigned char >& packetData )
	{
		//TD 15686
		//zhou yuan++
		TA_Base_Core::ThreadGuard guard(m_incomingMessageLock);
		m_incomingMessages.push( packetData );
		m_semaphone.post();
		//++zhou yuan
	}

	//
	//isLocal
	//
	bool STISMessageProcessor::isLocal()
	{
		return m_localProcessor;
	}


	//
	//isValidPacket
	//

	//  CCTV CMS Protocol - Packet Format
	//
	//	| SOH | PKT LEN |      COMMAND + Parameters     | PKT SUM | ETB |
	//	|  @  | Hi | Lo |<========== PKT LEN ==========>| Hi | Lo | \n  |
	//        |<=============== PKT SUM ===============>|
	//
	//
	//  SOH - Start of Header   The '@' character MUST be the first character of the
	//                          packet, or the data will be dropped.  No packet will
	//                          be returned.
	//
	//  PKT LEN - Packet Length As shown above, the values in the PKT LEN fields represent
	//                          the number of bytes required for the Command, and it's 
	//                          parameters.
	//
	//  COMMAND                 The 4 byte ASCII command code (eg RPLC - Report Local 
	//                          Configuration) plus any optional parameters.  All commands
	//                          are 4 bytes long, with as many bytes of optional parameters
	//                          as is necessary.
	//
	//  PKT SUM - Checksum      The packet checksum location and value can be determined
	//                          from the PKT LEN values, if the transmitted checksum value
	//                          matches the calculated value, the packet is correctly formed.
	//
	//  ETB End of Transmission The '\n' character MUST be the last character of the packet,
	//                          and must directly follow the PKT SUM low byte for the packet
	//                          to be deemed valid.
	//
	bool STISMessageProcessor::isValidPacket( std::vector< unsigned char>& packet )
	{
		FUNCTION_ENTRY("STISMessageProcessor::isValidPacket");
		bool packetOK = true;

		//extract the packet length value from the packet.
	/*      PacketLengthUnion packLengthU;

		//set the packet length byte values into the union, so we can access both bytes as one long.
		packLengthU.uc[0] = packet[1];
		packLengthU.uc[1] = packet[2];

		unsigned short pktLen = htons(packLengthU.us);
		//extract the checksum value from the packet.  We obtain the index of the
		//checksum high byte from the what we obtained in the packet length field
		unsigned int checksumIndex = pktLen+ SOH_SZ + PKT_LEN_SZ;// - 1; //indexes start at zero, length begins at 1
		unsigned int etbIndex = checksumIndex + PKT_SUM_SZ ;
    
		PacketLengthUnion checksumField;

		//set the checksum byte values into the union, so we can access both bytes as one long.
		checksumField.uc[0] = packet[checksumIndex];
		checksumField.uc[1] = packet[checksumIndex+1];

		unsigned short pktSum = htons(checksumField.us);

		std::vector< unsigned char > packetData;

		//copy the relevant data from the packet
		std::copy( packet.begin() + SOH_SZ, packet.begin() + checksumIndex, std::back_inserter(packetData) );
    
		std::vector< unsigned char >::iterator iter = packetData.begin();

		unsigned int checksumValue = 0;
		for(; iter != packetData.end(); ++iter)
		{
			checksumValue += (int)(*iter);
		}

		//the checksum should be the packet length value plus two (two bytes for the PKT LEN field.

		//conditions for a good packet
		//
		//1. First byte is '@'
		//2. PKT LEN >= CMD_SZ
		//3. PKT LEN is correctly calculated
		//4. Last byte (or byte after PKT SUM) is '\n'

		bool packetOK = true;

		//1.
		if( packet[0] != '@' )
		{
			packetOK = false;
		}

		//2.
		if( pktLen < CMD_SZ )
		{
			packetOK = false;
		}

		//3.
		if( pktSum != checksumValue )
		{
			packetOK = false;
		}

		//4.
		if( packet[etbIndex] != '\n' )
		{
			packetOK = false;
		}

		if(true == packetOK)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "packetOk == true");
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "packetOk == false");
		} */

		return packetOK; 

		FUNCTION_EXIT;
	}

	std::string STISMessageProcessor::sequenceNumberToString( int sequenceNumber )
	{
		std::stringstream numberStream;

			// Must be a better way to do this!!
		if( (sequenceNumber >= 1000) && (sequenceNumber <= 9999))
		{
			numberStream << sequenceNumber;
		}
		else if( ( sequenceNumber >= 100 ) && (sequenceNumber <= 999))
		{
			numberStream << "0";
			numberStream << sequenceNumber;
		}
		else if( ( sequenceNumber >= 10 ) && (sequenceNumber <= 99))
		{
			numberStream << "00";
			numberStream << sequenceNumber;
		}
		else if( sequenceNumber < 10)
		{
			numberStream << "000";
			numberStream << sequenceNumber;
		}
		
		return numberStream.str();

	}

	std::string STISMessageProcessor::getNewTimestampString()
	{
		std::stringstream timeStamp;

		TA_Base_Core::ThreadGuard guard( m_timestampLock );

		time_t currentTime;
		tm localTime;

		currentTime = time(NULL);
		//link error!
        //ACE_OS::localtime_r(&currentTime, &localTime);
		localtime_s(&localTime, &currentTime); //Equal to upon line

		timeStamp << localTime.tm_year+1900;

		if(localTime.tm_mon < 10)
		{
			timeStamp << '0';
			timeStamp << localTime.tm_mon;
		}
		else
		{
			timeStamp << localTime.tm_mon;
		}

		if(localTime.tm_wday < 10)
		{
			timeStamp << '0';
			timeStamp << localTime.tm_wday;
		}
		else
		{
			timeStamp << localTime.tm_wday;
		}


		if(localTime.tm_hour < 10 )
		{
			timeStamp << '0';
			timeStamp << localTime.tm_hour;
		}
		else
		{
			timeStamp << localTime.tm_hour;
		}


		if( localTime.tm_min < 10 )
		{
			timeStamp << '0';
			timeStamp << localTime.tm_min;
		}
		else
		{
			timeStamp << localTime.tm_min;
		}


		if( localTime.tm_sec < 10 )
		{
			timeStamp << '0';
			timeStamp << localTime.tm_sec;
		}
		else
		{
			timeStamp << localTime.tm_sec;
		}
	
		return timeStamp.str();
	}


	//////////////////////////////////////////////////////////////////////////////////////
	//                                                                                  //
	//                       Packet Processing & Sending Methods                        //
	//                                                                                  //
	//////////////////////////////////////////////////////////////////////////////////////

	//
	// processOCCStatusRequest
	//
	void STISMessageProcessor::processOCCStatusRequest(std::vector< unsigned char >& packet)
	{
		FUNCTION_ENTRY("STISMessageProcessor::processOCCStatusRequestMessage");

		//HRTB
		//  BYTES   |   FIELD NAME  |   SIZE    |   DESCRIPTION
		//-------------------------------------------------------------------------------------      
		//    0     |      SOH      |     1     |   Packet Start of Header
		//   1-2    |    PKT LEN    |     2     |   Two bytes describing the length of the data
		//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
		//   3-6    |    Command    |     4     |   Four bytes for the command code (HRTB)
		//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//   7-8    |    PKT SUM    |     2     |   The checksum bytes
		//    9     |      ETB      |     1     |   The End of Transmission Byte ('\n')
    
		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(packet.begin()+( STX_SZ ), packet.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		std::string seqmessage = "Received OCC Status Request message:  SequenceNumber = " ;

		seqmessage += sequenceNumber;
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, seqmessage.c_str() );
    	//LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "RXH acknowledgement flag: %c", packet[26]);
		// Get time schedule info
		unsigned char timeSchdule = packet[STX_SZ+SQN_SZ+TIMESTAMP_SZ+MESSAGE_ID_SZ+DATA_LENGTH_SZ+DESTINATION_STATION_WITHSPACE_SZ];
		if(timeSchdule == '1' && true == m_isTimeScheduleDownloadComplete)
		{
			m_isTimeScheduleDownloadComplete = false;
		}
		else
		{
			m_isTimeScheduleDownloadComplete = true;
		}

		sendOCCStatusReport( packet );

		FUNCTION_EXIT;
	}

	void STISMessageProcessor::processCCTStatusRequest(std::vector< unsigned char>& packet)
	{

        FUNCTION_ENTRY("STISMessageProcessor::processCCTStatusRequest");

		//HRTB
		//  BYTES   |   FIELD NAME  |   SIZE    |   DESCRIPTION
		//-------------------------------------------------------------------------------------      
		//    0     |      SOH      |     1     |   Packet Start of Header
		//   1-2    |    PKT LEN    |     2     |   Two bytes describing the length of the data
		//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
		//   3-6    |    Command    |     4     |   Four bytes for the command code (HRTB)
		//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//   7-8    |    PKT SUM    |     2     |   The checksum bytes
		//    9     |      ETB      |     1     |   The End of Transmission Byte ('\n')
    
		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(packet.begin()+( STX_SZ ), packet.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		std::string seqmessage = "Received CCT Status Request message:  SequenceNumber = " ;

		seqmessage += sequenceNumber;
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, seqmessage.c_str() );

		sendCCTStatusReport( packet );

		FUNCTION_EXIT;
	}

	//
	// processStationStatusRequest
	//
	void STISMessageProcessor::processStationStatusRequest(std::vector< unsigned char >& packet)
	{
		FUNCTION_ENTRY("STISMessageProcessor::processStationStatusRequest");

		//HRTB
		//  BYTES   |   FIELD NAME  |   SIZE    |   DESCRIPTION
		//-------------------------------------------------------------------------------------      
		//    0     |      SOH      |     1     |   Packet Start of Header
		//   1-2    |    PKT LEN    |     2     |   Two bytes describing the length of the data
		//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
		//   3-6    |    Command    |     4     |   Four bytes for the command code (HRTB)
		//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		//   7-8    |    PKT SUM    |     2     |   The checksum bytes
		//    9     |      ETB      |     1     |   The End of Transmission Byte ('\n')
    
		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1] = {0};
    
		std::copy(packet.begin()+( STX_SZ ), packet.begin()+( STX_SZ + SQN_SZ), sequenceNumber);

		std::string seqmessage = "Received CCT Status Request message:  SequenceNumber = " ;

		seqmessage += sequenceNumber;
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, seqmessage.c_str() );

		sendStationStatusReport( packet );

		FUNCTION_EXIT;
	}


	//
	// processPredefinedDisplayRequest
	//
	void STISMessageProcessor::processPredefinedDisplayRequest(std::vector< unsigned char >& packet)
	{
		FUNCTION_ENTRY("STISMessageProcessor::processPredefinedDisplayRequest");

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(packet.begin()+( STX_SZ ), packet.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		std::stringstream seqmessage;
		seqmessage << "\n--------------------------------------------";
		seqmessage << "\nReceived Predefined display Request message:";
		seqmessage << "\nSequenceNumber = " ;

		seqmessage << sequenceNumber;
		

		// timestamp - 14 chars
		// messageid - 3 chars
		// datalength - 4 chars

		// destination:
		// 3 characters for system id "CCL", 
		// 3 characters for station id eg "DBG",
		// 3 chars to indicate the number of PIDs eg "005", 3 characters for each PID ADDRESS eg "001,002"
		
		// message tag - 12 characters
		// start time - 14 characters
		// end time - 14 characters
		// priority - character

		// Destination station
		char destinationStation[DESTINATION_STATION_SZ+1]; // add one for the \n
		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ ), 
			packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ),
			destinationStation);
		destinationStation[DESTINATION_STATION_SZ] = '\0';
		seqmessage << "\nDestination Station: ";
		seqmessage << std::string(destinationStation);

		// Pid numbers
		char numPIDs[NUMBER_PIDS_SZ+1];
		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ), 
			packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ),
			numPIDs);
		numPIDs[NUMBER_PIDS_SZ] = '\0'; 
		int numberOfPids;
		numberOfPids = atoi( numPIDs );
		seqmessage << "\nNumber of PIDs: ";
		seqmessage << numberOfPids;


		// All pid address
		seqmessage << "\nPID Addresses: ";
		char pidAddress[PID_ADDRESS_SZ+1];
		int i;
		for( i=0; i < numberOfPids ; i++ )
		{
			std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (i*PID_ADDRESS_SZ)), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (i*PID_ADDRESS_SZ) + PID_ADDRESS_SZ),
			pidAddress);
			pidAddress[PID_ADDRESS_SZ] = '\0'; 
			int pidAddr = atoi( pidAddress );
			seqmessage << pidAddr << " ";
		}

		// Message tag
		char messageTag[MESSAGE_TAG_SZ+1];
		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ)), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ),
			messageTag);
		messageTag[MESSAGE_TAG_SZ] = '\0';
		seqmessage << "\nMessage Tag: ";
		seqmessage << messageTag;


		// Start time and end time
		char startTime[15];
		char endTime[15];
		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ),
			startTime);
		startTime[14] = '\0';
		seqmessage << "\nStart Time: ";
		seqmessage << startTime;

		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ),
			endTime);

		endTime[14] = '\0';
		seqmessage << "\nEnd Time: ";
		seqmessage << endTime;

		// Priority
		char priority[2];
		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + PRIORITY_SZ),
			priority);
		priority[1] = '\0';
		seqmessage << "\nPriority: ";
		seqmessage << priority;

		seqmessage << "\n--------------------------------------------\n";
		std::string message;
		message = seqmessage.str();
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, message.c_str() );

        switch( m_parent->getDisplayRequestResponseType() )
        {
        case POSITIVE_RESPONSE:
            sendDisplayReport( packet );
            break;

        case NEGATIVE_RESPONSE:
            // Send a NACK of "corrupted connection"
			sendNack(packet,1);
            std::cout << "Negative Response" << std::endl;
            break;

        case NO_RESPONSES:
            std::cout << "Play Dead" << std::endl;
            break;
        }
 	

		FUNCTION_EXIT;
	}


	//
	// processAdhocDisplayRequest
	//
	void STISMessageProcessor::processAdhocDisplayRequest(std::vector< unsigned char >& packet)
	{
		FUNCTION_ENTRY("STISMessageProcessor::processAdhocDisplayRequest");
    
		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(packet.begin()+( STX_SZ ), packet.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		std::stringstream seqmessage;
		seqmessage << "\n--------------------------------------------\n";
		seqmessage << "Received Adhoc display Request message: \n";
		seqmessage << "SequenceNumber = " ;

		seqmessage << sequenceNumber;

		char destinationStation[DESTINATION_STATION_SZ+1]; // add one for the \n
		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ ), 
			packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ),
			destinationStation);
		destinationStation[DESTINATION_STATION_SZ] = '\0';

		seqmessage << "\nDestination Station: ";
		seqmessage << std::string(destinationStation);

		char numPIDs[4];

		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ), 
			packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ),
			numPIDs);

		numPIDs[3] = '\0'; 

		int numberOfPids;

		numberOfPids = atoi( numPIDs );

		seqmessage << "\nNumber of PIDs: ";
		seqmessage << numberOfPids;

		seqmessage << "\nPID Addresses: ";
		char pidAddress[4];
		int i;
		for( i=0; i < numberOfPids ; i++ )
		{
			std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (i*PID_ADDRESS_SZ)), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (i*PID_ADDRESS_SZ) + PID_ADDRESS_SZ),
			pidAddress);

			pidAddress[3] = '\0'; 

			int pidAddr = atoi( pidAddress );

			seqmessage << pidAddr << " ";
			
		}
	
		char messageTag[13];
		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ)), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ),
			messageTag);
		messageTag[12] = '\0';

		seqmessage << "\nMessage Tag: ";
		seqmessage << messageTag;


		char startTime[15];
		char endTime[15];

		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ),
			startTime);

		startTime[14] = '\0';
		seqmessage << "\nStart Time: ";
		seqmessage << startTime;

		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ),
			endTime);

		endTime[14] = '\0';
		seqmessage << "\nEnd Time: ";
		seqmessage << endTime;

		char priority[2];
		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + PRIORITY_SZ),
			priority);

		priority[1] = '\0';
		seqmessage << "\nPriority: ";
		seqmessage << priority;

		// Display effects
		char displayMode[2];
		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + PRIORITY_SZ), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + PRIORITY_SZ + DISPLAY_MODE_SZ),
			displayMode);

		displayMode[1] = '\0';
		seqmessage << "\nDisplay Mode: ";

		switch( displayMode[0] )
		{
		case '0':
			seqmessage << "Instant On";
			break;
		case '1':
			seqmessage << "Scroll Left";
			break;
		case '2':
			seqmessage << "Scroll Right";
			break;
		case '3':
			seqmessage << "Roll Up";
			break;
		case '4':
			seqmessage << "Roll Down";
			break;
		case '5':
			seqmessage << "Wipe Left";
			break;
		case '6':
			seqmessage << "Wipe Right";
			break;
		case '7':
			seqmessage << "Left Multiple Wipe";
			break;
		case '8':
			seqmessage << "Right multiple Wipe";
			break;
		case '9':
			seqmessage << "Curtain Open";
			break;
		case 'A':
			seqmessage << "Curtain Close";
			break;
		case 'B':
			seqmessage << "Random Order";
			break;
		case 'C':
			seqmessage << "Center Spread";
			break;
		case 'D':
			seqmessage << "Two Ends Spread";
			break;
		case 'E':
			seqmessage << "Flashing";
			break;
		case 'F':
			seqmessage << "Blinking";
			break;
		case 'G':
			seqmessage << "Color Change";
			break;
		case 'H':
			seqmessage << "Dropping";
			break;
		}

		char scrollSpeed[2];
		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + PRIORITY_SZ + DISPLAY_MODE_SZ), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + PRIORITY_SZ + DISPLAY_MODE_SZ + SCROLL_SPEED_SZ),
			scrollSpeed);

		scrollSpeed[1] = '\0';
		seqmessage << "\nScroll Speed: ";
		switch( scrollSpeed[0] )
		{
		case '0':
			seqmessage << "Slowest";
			break;
		case '1':
			seqmessage << "Slow";
			break;
		case '8':
			seqmessage << "Medium";
			break;
		case 'F': // Note - this is sent as hex 'F'
			seqmessage << "Fast"; 
			break;
		}
		//Repeat Interval
		char repeatInterval[REPEAT_INTERVAL_SZ+1];
		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + PRIORITY_SZ + DISPLAY_MODE_SZ+SCROLL_SPEED_SZ), 
				packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + PRIORITY_SZ + DISPLAY_MODE_SZ + SCROLL_SPEED_SZ+REPEAT_INTERVAL_SZ),
			repeatInterval);
		repeatInterval[REPEAT_INTERVAL_SZ] = '\0';
		seqmessage << "\nRepeat Interval: " << repeatInterval;

		//display time
		char displayTime[DISPLAY_TIME_SZ+1];
		std::copy(packet.begin()+(REPEAT_INTERVAL_SZ + STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + PRIORITY_SZ + DISPLAY_MODE_SZ+SCROLL_SPEED_SZ), 
				packet.begin()+( DISPLAY_TIME_SZ + STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + PRIORITY_SZ + DISPLAY_MODE_SZ + SCROLL_SPEED_SZ+REPEAT_INTERVAL_SZ),
			displayTime);
		displayTime[DISPLAY_TIME_SZ] = '\0';
		seqmessage << "\nDisplay Time: " << displayTime;
		
		//justification
		char justification[JUSTIFICATION_SZ + 1];
		std::copy(packet.begin()+(DISPLAY_TIME_SZ + REPEAT_INTERVAL_SZ + STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + PRIORITY_SZ + DISPLAY_MODE_SZ+SCROLL_SPEED_SZ), 
				packet.begin()+(JUSTIFICATION_SZ + DISPLAY_TIME_SZ + STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_WITHSPACE_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ) + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + PRIORITY_SZ + DISPLAY_MODE_SZ + SCROLL_SPEED_SZ+REPEAT_INTERVAL_SZ),
			justification);
		justification[JUSTIFICATION_SZ] = '\0';
		seqmessage << "\nJustification: " << justification;

		std::string message;
		message = seqmessage.str();
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, message.c_str() );


        switch( m_parent->getDisplayRequestResponseType())
        {
        case POSITIVE_RESPONSE:
            sendDisplayReport( packet );
            break;

        case NEGATIVE_RESPONSE:
            // Send a NACK of "corrupted connection"
			sendNack(packet,1);
            std::cout << "Negative Response" << std::endl;
            break;

        case NO_RESPONSES:
            std::cout << "Play Dead" << std::endl;
            break;
        }
 

		FUNCTION_EXIT;
	}


	//
	// processClearRequest
	//
	void STISMessageProcessor::processClearRequest(std::vector< unsigned char >& packet)
	{
		FUNCTION_ENTRY("STISMessageProcessor::processClearRequest");

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(packet.begin()+( STX_SZ ), packet.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		std::stringstream seqmessage;
		seqmessage << "--------------------------------------------\n";
		seqmessage << "Received Clear Request message:\n";
		seqmessage << "SequenceNumber = " ;

		seqmessage << sequenceNumber << "\n";

		char systemID[4]; // add one for the \n

		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ ), 
			packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ ),
			systemID);

		systemID[3] = '\0';

        std::string sysID(systemID);
		int numberOfPids = 0;
        int endDestinationIndex = 0; 

        // if prefixed with sys, then it's from ISCS
        if (sysID.compare("sys") == 0)
        {
            // next 3 is the actual system code like NEL, RXH etc
		    char systemCode[4]; // add one for the \n

		    std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ ), 
			    packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ),
			    systemCode);

		    systemCode[3] = '\0';

		    seqmessage << "\nISCS messge to Destination System Code: ";
		    seqmessage << std::string(systemCode);

            endDestinationIndex = STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ;
        }
        else
        {
		    char destinationStation[4]; // add one for the \n

		    std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ ), 
			    packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ),
			    destinationStation);

		    destinationStation[3] = '\0';

            std::string station(destinationStation);
		    seqmessage << "Destination Station: ";
		    seqmessage << station;

            if ((station.compare("AST") == 0) || (station.compare("ATR") == 0))
            {
                // all trains or all stations don't have id list
                endDestinationIndex = STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ;
            }
            else
            {
		        char numPIDs[4];

		        std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ), 
			        packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ + NUMBER_PIDS_SZ),
			        numPIDs);

		        numPIDs[3] = '\0';

		        numberOfPids = atoi( numPIDs );

		        seqmessage << "\nNumber of PIDs: ";
		        seqmessage << numberOfPids;

		        seqmessage << "\nPID Addresses: ";
		        char pidAddress[4];
		        int i;
		        for( i=0; i < numberOfPids ; i++ )
		        {
			        std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ + NUMBER_PIDS_SZ + (i*PID_ADDRESS_SZ)), 
				        packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ + NUMBER_PIDS_SZ + (i*PID_ADDRESS_SZ) + PID_ADDRESS_SZ),
			        pidAddress);

			        pidAddress[3] = '\0'; 

			        int pidAddr = atoi( pidAddress );

			        seqmessage << pidAddr << " ";
			        
		        }

                endDestinationIndex = STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ);
            }
        }		
		char messageTag[13];
		std::copy(packet.begin()+( endDestinationIndex), 
				packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ),
			messageTag);
		messageTag[12] = '\0';

		seqmessage << "\nMessage Tag: ";
		seqmessage << messageTag;

		char priorityMask[9];
		std::copy(packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ), 
				packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + PRIORITY_MASK_SZ),
			priorityMask);
		priorityMask[8] = '\0';

		seqmessage << "\nPriority Mask: ";
		seqmessage << priorityMask;

		seqmessage << "\n--------------------------------------------\n";

		std::string message;
		message = seqmessage.str();
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, message.c_str() );

		sendClearReport( packet );

		FUNCTION_EXIT;
	}


	//
	// processSTISUpgradePredefinedMessageLibraryRequest
	//
	void STISMessageProcessor::processSTISUpgradePredefinedMessageLibraryRequest( std::vector< unsigned char >& packet )
	{
		FUNCTION_ENTRY("STISMessageProcessor::processSTISUpgradePredefinedMessageLibraryRequest");

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(packet.begin()+( STX_SZ ), packet.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		std::string seqmessage = "Received Upgrade Predefined Message Library Request message:  SequenceNumber = " ;

		seqmessage += sequenceNumber;
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, seqmessage.c_str() );

		// timestamp - 14 chars
		// messageid - 3 chars
		// datalength - 4 chars
		// library version - 3 chars
		char libraryVersion[4]; // add one for the \n

		std::copy(packet.begin()+(STX_SZ + SQN_SZ + 14 + 3 + 4), packet.begin()+(STX_SZ + SQN_SZ + 14 + 3 + 4 + 3), libraryVersion);
		libraryVersion[3] = '\0';

		upgradeSTISLibrary( atoi(libraryVersion));


		sendUpgradePredefinedMessageLibraryReport( packet );

		FUNCTION_EXIT;
	}


	///
	// processSuperimposeText
	//
	void STISMessageProcessor::processApprovedRATISDisplayRequest( std::vector< unsigned char >& packet )
	{
		FUNCTION_ENTRY("STISMessageProcessor::processApprovedRATISDisplayRequest");

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(packet.begin()+( STX_SZ ), packet.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		std::stringstream seqmessage;
		seqmessage << "\n--------------------------------------------\n";
		seqmessage << "Received RATIS Approved Display Request message: \n";
		seqmessage << "SequenceNumber = " ;

		seqmessage << sequenceNumber;

		char systemID[4]; // add one for the \n

		std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ ), 
			packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ ),
			systemID);

		systemID[3] = '\0';

        std::string sysID(systemID);
		int numberOfPids = 0;
        int endDestinationIndex = 0; 

        if (sysID.compare("sys") == 0)
        {
		    char systemCode[4]; // add one for the \n

		    std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ ), 
			    packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ),
			    systemCode);

		    systemCode[3] = '\0';

		    seqmessage << "\nISCS messge to Destination System Code: ";
		    seqmessage << std::string(systemCode);

            endDestinationIndex = STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ;
        }
        else
        {
		    char destinationStation[4]; // add one for the \n

		    std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ ), 
			    packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ),
			    destinationStation);

		    destinationStation[3] = '\0';

            std::string station(destinationStation);
		    seqmessage << "\nDestination Station: ";
		    seqmessage << station;

            if ((station.compare("AST") == 0) || (station.compare("ATR") == 0))
            {
                // all trains or all stations don't have id list
                endDestinationIndex = STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ;
            }
            else
            {
		        char numPIDs[4];

		        std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ), 
			        packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ + NUMBER_PIDS_SZ),
			        numPIDs);

		        numPIDs[3] = '\0'; 

		        numberOfPids = atoi( numPIDs );

		        seqmessage << "\nNumber of PIDs: ";
		        seqmessage << numberOfPids;

		        seqmessage << "\nPID Addresses: ";
		        char pidAddress[4];
		        int i;
		        for( i=0; i < numberOfPids ; i++ )
		        {
			        std::copy(packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ + NUMBER_PIDS_SZ + (i*PID_ADDRESS_SZ)), 
				        packet.begin()+( STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ + NUMBER_PIDS_SZ + (i*PID_ADDRESS_SZ) + PID_ADDRESS_SZ),
			        pidAddress);

			        pidAddress[3] = '\0'; 

			        int pidAddr = atoi( pidAddress );

			        seqmessage << pidAddr << " ";
			        
		        }
                endDestinationIndex = STX_SZ + SQN_SZ + TIMESTAMP_SZ + MESSAGE_ID_SZ + DATA_LENGTH_SZ + SYSTEM_ID_SZ + DESTINATION_STATION_SZ + NUMBER_PIDS_SZ + (numberOfPids*PID_ADDRESS_SZ);
            }
        }
		char messageTag[13];
		std::copy(packet.begin()+( endDestinationIndex), 
				packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ),
			messageTag);
		messageTag[12] = '\0';

		seqmessage << "\nMessage Tag: ";
		seqmessage << messageTag;


		char startTime[15];
		char endTime[15];

		std::copy(packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ), 
				packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ),
			startTime);

		startTime[14] = '\0';
		seqmessage << "\nStart Time: ";
		seqmessage << startTime;

		std::copy(packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ), 
				packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ),
			endTime);

		endTime[14] = '\0';
		seqmessage << "\nEnd Time: ";
		seqmessage << endTime;

		char priority[2];
		std::copy(packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ), 
				packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + RATIS_PRIORITY_SZ),
			priority);

		priority[1] = '\0';
		seqmessage << "\nPriority: ";
		seqmessage << priority;

		char vetting[2];
		std::copy(packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + RATIS_PRIORITY_SZ), 
				packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + RATIS_PRIORITY_SZ + RATIS_VETTING_SZ),
			vetting);

		vetting[1] = '\0';
		seqmessage << "\nVetting: ";
		seqmessage << vetting;

		char override[2];
		std::copy(packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + RATIS_PRIORITY_SZ + RATIS_VETTING_SZ), 
				packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + RATIS_PRIORITY_SZ + RATIS_VETTING_SZ + RATIS_OVERRIDE_SZ),
			override);

		override[1] = '\0';
		seqmessage << "\nOverride: ";
		seqmessage << override;

		char clearing[2];
		std::copy(packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + RATIS_PRIORITY_SZ + RATIS_VETTING_SZ + RATIS_OVERRIDE_SZ), 
				packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + RATIS_PRIORITY_SZ + RATIS_VETTING_SZ + RATIS_OVERRIDE_SZ + RATIS_CLEARING_SZ),
			clearing);

		clearing[1] = '\0';
		seqmessage << "\nClearing: ";
		seqmessage << clearing;

		char mesgLen[4];
		std::copy(packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + RATIS_PRIORITY_SZ + RATIS_VETTING_SZ + RATIS_OVERRIDE_SZ + RATIS_CLEARING_SZ), 
				packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + RATIS_PRIORITY_SZ + RATIS_VETTING_SZ + RATIS_OVERRIDE_SZ + RATIS_CLEARING_SZ + RATIS_MESSAGE_LENGTH_SZ),
			mesgLen);

        int length = atol(mesgLen);

        std::string content(packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + RATIS_PRIORITY_SZ + RATIS_VETTING_SZ + RATIS_OVERRIDE_SZ + RATIS_CLEARING_SZ + RATIS_MESSAGE_LENGTH_SZ), 
				packet.begin()+( endDestinationIndex + MESSAGE_TAG_SZ + START_TIME_SZ + END_TIME_SZ + RATIS_PRIORITY_SZ + RATIS_VETTING_SZ + RATIS_OVERRIDE_SZ + RATIS_CLEARING_SZ + RATIS_MESSAGE_LENGTH_SZ + length));

        seqmessage << "\nContent: ";
		seqmessage << content;


		std::string message;
		message = seqmessage.str();
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, message.c_str() );

        switch( m_parent->getRATISResponseType())
        {
        case POSITIVE_RESPONSE:
            sendDisplayReport( packet );
            break;

        case NEGATIVE_RESPONSE:
            // Send a NACK of "corrupted connection"
			sendNack(packet,1);
            std::cout << "Negative Response" << std::endl;
            break;

        case NO_RESPONSES:
            std::cout << "Play Dead" << std::endl;
            break;
        }
 		FUNCTION_EXIT;
	}


	//
	// processTrainInformationRequest
	//
	void STISMessageProcessor::processTrainInformationRequest( std::vector< unsigned char >& packet )
	{
		FUNCTION_ENTRY("STISMessageProcessor::processTrainInformationRequest");

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(packet.begin()+( STX_SZ ), packet.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		std::string seqmessage = "Received Train Information message:  SequenceNumber = " ;

		seqmessage += sequenceNumber;
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, seqmessage.c_str() );

		// Do not need to process these messages - the TIS Agent does not require an ACK.
    
		FUNCTION_EXIT;
	}


	//
	// processConfigurationDataRequest
	//
	void STISMessageProcessor::processConfigurationDataRequest( std::vector< unsigned char >& packet )
	{
		FUNCTION_ENTRY("STISMessageProcessor::processConfigurationDataRequest");

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(packet.begin()+( STX_SZ ), packet.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		std::string seqmessage = "Received Configuration Data Request message:  SequenceNumber = " ;

		seqmessage += sequenceNumber;
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, seqmessage.c_str() );

		sendConfigurationDataReport( packet );

		FUNCTION_EXIT;
	}


	//
	// processCurrentDisplayingMessageRequest
	//
	void STISMessageProcessor::processCurrentDisplayingMessageRequest(std::vector< unsigned char >& packet)
	{
		FUNCTION_ENTRY("STISMessageProcessor::processCurrentDisplayingMessageRequest");

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(packet.begin()+( STX_SZ ), packet.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		std::string seqmessage = "Received Current Display Message request message:  SequenceNumber = " ;

		seqmessage += sequenceNumber;
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, seqmessage.c_str() );

		sendCurrentDisplayingMessageReport( packet );


		FUNCTION_EXIT;
	}

	//
	// processCurrentDisplayingMessageRequest
	//
	void STISMessageProcessor::processPIDControlRequest(std::vector< unsigned char >& packet)
	{
		FUNCTION_ENTRY("STISMessageProcessor::processPIDControlRequest");

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(packet.begin()+( STX_SZ ), packet.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		std::string seqmessage = "Received PID Control request message:  SequenceNumber = " ;

		seqmessage += sequenceNumber;
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, seqmessage.c_str() );

        // we're going to parse the destination to figure out which packets have changed 
        bool on = packet[packet.size() - 3] == '1';
        bool off = packet[packet.size() - 2] == '1';

        int status;
        if (on && !off)
        {
            status = 1;
        }
        else if (!on && off)
        {
            status = 0;
        }
        else
        {
            // TODO: send negative acknowledgement
    		FUNCTION_EXIT;
            return;
        }

        // read past the first 3 bytes of destination as it is the system ID
        std::string destination(packet.begin() + 26, packet.end() - 3);
        std::string location = destination.substr(3, 3);
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "location = %s", location.c_str());
        if  (m_locationName.compare(location) == 0)
        {
            // we need to set pid if we're on the right station
            int numPID = atol(destination.substr(6, 3).c_str());
            for (int i = 0; i < numPID; i++)
            {
                int id = atol(destination.substr(9 + i * 3, 3).c_str());
        		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Turn PID %d to %d", id, status);
            	updatePIDStatus(id, status);
            }
        }
        
		sendPIDControlReport( packet );

		FUNCTION_EXIT;
	}


	//
	//sendPacket
	//
	void STISMessageProcessor::sendPacket( std::vector< unsigned char >& packet )
	{ 
		FUNCTION_ENTRY("SwitchRequestProcessor::sendPacket");
		
		//if this is a local processor
		if( false == m_localProcessor )
		{
			//send the packet out on the socket it arrived on.
			m_parent->getSocket()->write( (char*)&(packet[0]), packet.size() );
		}
/*		else
		{
			//sendPacket.push_back('\0');

			std::string temp( sendPacket.size() - 6, ' ' );
			std::copy( sendPacket.begin() + 3, sendPacket.begin() + (sendPacket.size() - 3), temp.begin() );
			std::cout << "@XX" << temp << "XX\n" <<  std::endl;
		} */


		FUNCTION_EXIT;
	}

	//
	//sendConfigurationDataReport
	//
	void STISMessageProcessor::sendConfigurationDataReport( std::vector< unsigned char >& command )
	{
		FUNCTION_ENTRY("STISMessageProcessor::sendConfigurationDataReport");

				std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(command.begin()+( STX_SZ ), command.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		// Add the sequence number
		messageStream << sequenceNumber;

		std::string timeStamp = getNewTimestampString();

		// Add the timestamp to the message
		messageStream << timeStamp;

		// Now the message ID
		messageStream << "A00";

		// Now the data length - this is always 9 bytes
		messageStream << "0009";

		// Now the actual data

		// Report station ??  this should be the same as supplied in the source message
		messageStream << m_locationName;

		// Now the end of packet character

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );

		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );	

		sendPacket( messageVector );


		FUNCTION_EXIT;
	}

	//
	//sendDisplayReport
	//
	void STISMessageProcessor::sendDisplayReport( std::vector< unsigned char >& command )
	{
		FUNCTION_ENTRY("STISMessageProcessor::sendDisplayReport");

		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(command.begin()+( STX_SZ ), command.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		// Add the sequence number
		messageStream << sequenceNumber;

		std::string timeStamp = getNewTimestampString();

		// Add the timestamp to the message
		messageStream << timeStamp;

		// Now the message ID
		messageStream << "A10";

		// Now the data length - this is always 6 bytes, i.e just the 'Report Station' field
		messageStream << "0006";

		// Now the actual data

		// Report station ??  this should be the same as supplied in the source message
		messageStream << m_locationName << "  "; // 2 space add

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
			
		sendPacket( messageVector );


		FUNCTION_EXIT;
	}

	//
	//sendClearReport
	//
	void STISMessageProcessor::sendClearReport( std::vector< unsigned char >& command )
	{
		FUNCTION_ENTRY("STISMessageProcessor::sendClearReport");

		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(command.begin()+( STX_SZ ), command.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		// Add the sequence number
		messageStream << sequenceNumber;

		std::string timeStamp = getNewTimestampString();

		// Add the timestamp to the message
		messageStream << timeStamp;

		// Now the message ID
		messageStream << "A20";

		// Now the data length - hardcoded in this message - it will always be 3
		// i.e just the 'Report Station' field.
		messageStream << "0003";

		// Now the actual data

		// Report station ??  this should be the same as supplied in the source message
		messageStream << m_locationName;

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
			
		sendPacket( messageVector );



		FUNCTION_EXIT;
	}

	//
	//sendStationStatusReport
	//
	void STISMessageProcessor::sendStationStatusReport( std::vector< unsigned char >& command )
	{
		FUNCTION_ENTRY("STISMessageProcessor::sendStationStatusReport");

		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		messageVector.clear();

		messageStream.clear();

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(command.begin()+( STX_SZ ), command.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		// Add the sequence number
		messageStream << sequenceNumber;

		std::string timeStamp = getNewTimestampString();

		// Add the timestamp to the message
		messageStream << timeStamp;

		// Now the message ID
		messageStream << "A30";

		// Now the data length - this is variable
		// 15 + 1 byte for each PID status
		if( m_locationName.compare("DBG") == 0 )
		{
			messageStream << "0021";
		}
		else if( m_locationName.compare("MSM") == 0 )
		{
			messageStream << "0026";
		}
		else if( m_locationName.compare("CVC") == 0 )
		{
			messageStream << "0027";
		}
		//TD 16466
		else if( m_locationName.compare("BLY") == 0 )
		{
			messageStream << "0024";
		}
		else
		{
			messageStream << "0015";
		}
		//TD 16466

		// Now the actual data

		// Report station ??  this should be the same 
		messageStream << m_locationName;

		// LED summary status - 1 is Alarm, 0 is Normal
		messageStream << m_ledDisplaySummary;

		// Plasma display status - 1 is Alarm, 0 is Normal
		messageStream << m_plasmaDisplaySummary;

		// Connection link Status - 1 is Alarm, 0 is Normal
		messageStream << m_connectionLinkSummary;

		// Clock link status - 1 is Alarm, 0 is Normal
		messageStream << m_clockLinkSummary;

		// current predefined STIS message library version (indicates the version that the STIS server is running with)
		// Can be used for a check before sending a predefined message request
		if( m_currentStisLibrary < 10 )
		{
			messageStream << "00";
			messageStream << m_currentStisLibrary;
		}
		else if( m_currentStisLibrary < 100 )
		{
			messageStream << "0";
			messageStream << m_currentStisLibrary;
		}
		else 
		{
			messageStream << m_currentStisLibrary;
		}

		// new predefined STIS message library version 
		if( m_newStisLibrary < 10 )
		{
			messageStream << "00";
			messageStream << m_newStisLibrary;
		}
		else if( m_newStisLibrary < 100 )
		{
			messageStream << "0";
			messageStream << m_newStisLibrary;
		}
		else 
		{
			messageStream << m_newStisLibrary;
		}

		std::map<int,int>::iterator iter;

		// Number of PIDs for which the status is reported (01 - 50)
		int numPIDs;
		numPIDs = m_pidStatusMap.size();

		if( numPIDs < 10 )
		{
			messageStream << "0";
			messageStream << numPIDs;
		}
		else
		{
			messageStream << numPIDs;
		}

		for( iter = m_pidStatusMap.begin() ; iter != m_pidStatusMap.end() ; iter++)
		{
			messageStream << iter->second;
		}

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
			
		sendPacket( messageVector );

		FUNCTION_EXIT;
	}
	
	void STISMessageProcessor::sendCCTStatusReport(std::vector< unsigned char>& command)
	{
		FUNCTION_ENTRY("STISMessageProcessor::sendCCTStatusReport");
		
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;
		messageVector.clear();
		messageStream.clear();
		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1] = {0};
		std::copy(command.begin()+( STX_SZ ), command.begin()+( STX_SZ + SQN_SZ), sequenceNumber);

		// Add the sequence number
		messageStream << sequenceNumber;

		std::string timeStamp = getNewTimestampString();

		// Add the timestamp to the message
		messageStream << timeStamp;

		// Now the message ID
		messageStream << "A31";

		// Now the data length - a few options here
		// 1) No new RATIS message, length is 46 bytes (assuming both filename fields are fixed at 16 bytes)
		// 2) New RATIS message, length is 46 bytes + Destination + RATIS fields

		// Now the actual data - this needs to be created separately
		// because the length of the data needs to be indicated 
		// before the data itself (i.e we need to calculate the size of it)

		std::stringstream dataStream;

		// cct server status
		dataStream << m_cctComputerStatus;

		//New ttis library filename 16 byte
		dataStream << "LibraryTTIS1.txt";

		// current predefined STIS message library version (indicates what the STIS server is running)
		// Can be used for a check before sending a predefined message request
		dataStream << numberVersionToStr(m_currentTtisLibrary);		

		// new predefined STIS message library version
		dataStream << numberVersionToStr(m_newTtisLibrary);		

		// Calculate the data length - this is variable
		std::string dataString = dataStream.str();
		int dataSize = dataString.size();
		messageStream << std::setw(4) << std::setfill('0') << dataSize;		
		
		// Add the data stream to the actual message stream after calculating its size
		messageStream << dataString;	

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );

		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
			
		sendPacket( messageVector );

		FUNCTION_EXIT;
	}

	//
	//sendOCCStatusReport
	//
	void STISMessageProcessor::sendOCCStatusReport( std::vector< unsigned char >& command )
	{
		FUNCTION_ENTRY("STISMessageProcessor::sendOCCStatusReport");
		
		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		messageVector.clear();

		messageStream.clear();

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(command.begin()+( STX_SZ ), command.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		// Add the sequence number
		messageStream << sequenceNumber;

		std::string timeStamp = getNewTimestampString();

		// Add the timestamp to the message
		messageStream << timeStamp;

		// Now the message ID
		messageStream << "A31";

		// Now the data length - a few options here
		// 1) No new RATIS message, length is 46 bytes (assuming both filename fields are fixed at 16 bytes)
		// 2) New RATIS message, length is 46 bytes + Destination + RATIS fields

		// Now the actual data - this needs to be created separately
		// because the length of the data needs to be indicated 
		// before the data itself (i.e we need to calculate the size of it)

		std::stringstream dataStream;

		/*
		// RXH link status - 1 is Alarm, 0 is Normal
		dataStream << m_rxhLinkSummary;

		// Clock link status - 1 is Alarm, 0 is Normal
		dataStream << m_clockLinkSummary;
		*/

		// Connection link status - 1 is Alarm, 0 is Normal
		dataStream << m_connectionLinkSummary;

		// TIS OCC Server Status - 1 is Alarm, 0 is Normal
		dataStream << m_tisOccServerSummary;

		// New STIS Library filename

		dataStream << "LibrarySTIS1.txt"; // It's 16 byte, Ok

		// current predefined STIS message library version (indicates what the STIS server is running)
		// Can be used for a check before sending a predefined message request
		if( m_currentStisLibrary < 10 )
		{
			dataStream << "00";
			dataStream << m_currentStisLibrary;
		}
		else if( m_currentStisLibrary < 100 )
		{
			dataStream << "0";
			dataStream << m_currentStisLibrary;
		}
		else 
		{
			dataStream << m_currentStisLibrary;
		}

		// new predefined STIS message library version 
		if( m_newStisLibrary < 10 )
		{
			dataStream << "00";
			dataStream << m_newStisLibrary;
		}
		else if( m_newStisLibrary < 100 )
		{
			dataStream << "0";
			dataStream << m_newStisLibrary;
		}
		else 
		{
			dataStream << m_newStisLibrary;
		}

		// Time Schdule download status
		if (true == m_isTimeScheduleDownloadComplete)
		{
			dataStream << "1";
		}
		else
		{
			dataStream << "0";
		}

		/*
        if (m_ratisCommand.empty())
        {
            // not a ratis message
		    dataStream << "0";  
        }
        else
        {
            dataStream << "1"; // always one cos it's a ratis message

            // parse the command into <station e.g. DBG>, [<numPID> if it is not 'AST' OR 'ATR'], <tag - up to 12 char >, <priority 1-8>, <vetting 0/1>, <override 0/1>, <clearing 0/1>
            std::stringstream commandStr(m_ratisCommand);
            int priority, vetting, override, clearing, numPID;
            std::string tag, station, starttimestr, endtimestr;
            commandStr >> station;

            // if it is all station
            if ((station.compare("AST") == 0) || (station.compare("ATR") == 0))
            {
                dataStream << "CCL"; // always for us
                dataStream << station; // send it to all stations/trains
            }
            else
            {
                // otherwise it is either station (e.g. DBG) or train (TRN), the number of pid/train, and the list
                commandStr >> numPID;

		        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
		            "For first %s PIDs at %d", station.c_str(), numPID);

                // destination station and pid
                dataStream << "CCL"; // always for us
                dataStream << station; // the station to send to
		        dataStream << std::setw(NUMBER_PIDS_SZ) << std::setfill('0') << numPID; // the number of PID

                for (int i = 1; i <= numPID; i++)
                {
    		        dataStream << std::setw(PID_ADDRESS_SZ) << std::setfill('0') << i; // pid
                }
            }

            commandStr >> tag >> priority >> vetting >> override >> clearing >> starttimestr >> endtimestr;

            std::string truncTag(tag);

            // concatenate the tag if necessary
            if (tag.size() > MESSAGE_TAG_SZ)
            {
                truncTag = tag.substr(0, MESSAGE_TAG_SZ);
            }

            // message tag - left justify this so that the end is padded rather than begining.  Put the justification back to right after this.
            dataStream << std::setw(MESSAGE_TAG_SZ) << std::left << std::setfill(' ')  << truncTag << std::right; 

            // Start Time
		    dataStream << starttimestr; // "20040112152359";

		    // End Time
		    dataStream << endtimestr;   // "20040112123000";

		    // Priority
		    dataStream << priority;

            // vetting, override, clearing
            dataStream << vetting;
            dataStream << override;
            dataStream << clearing;

            std::string messageContent;
            if (clearing == 0) // if this is not message clearing, then we add some message content into the packet
            {
                messageContent = "RATIS message. Tag: " + tag;
		        // Message length - number of characters in message content field below
		        dataStream << std::setw(RATIS_MESSAGE_LENGTH_SZ) << messageContent.size();

		        // Message content
		        dataStream << messageContent;
            }

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
		        "Destination station = %s: \ntag = %s, \npriority = %d, \nvetting = %d, \noverride = %d, \nclearing = %d, \nmessage = %s", 
                station.c_str(), truncTag.c_str(), priority, vetting, override, clearing, messageContent.c_str());

            // reset this cos we dont' want to keep sending ratis updates
            m_ratisCommand = "";
        }
		*/

		// Calculate the data length - this is variable
		std::string dataString = dataStream.str();
		int dataSize = dataString.size();

		if( dataSize >= 1000 )
		{
			
			messageStream << dataSize;
		}
		else if( dataSize >= 100 )
		{
			messageStream << "0";
			messageStream << dataSize;
		}
		else if( dataSize >= 10 )
		{
			messageStream << "00";
			messageStream << dataSize;
		}
		else 
		{
			messageStream << "000";
			messageStream << dataSize;
		}
		
		// Add the data stream to the actual message stream after calculating its size
		messageStream << dataString;	

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );

		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
			
		sendPacket( messageVector );

		FUNCTION_EXIT;
	}

	//
	//sendCurrentDisplayingMessageReport
	//
	void STISMessageProcessor::sendCurrentDisplayingMessageReport( std::vector< unsigned char >& command )
	{
		FUNCTION_ENTRY("STISMessageProcessor::sendCurrentDisplayingMessageReport");

		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Add the sequence number

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(command.begin()+( STX_SZ ), command.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		// Add the sequence number
		messageStream << sequenceNumber;

		std::string timeStamp = getNewTimestampString();

		// Add the timestamp to the message
		messageStream << timeStamp;

		// Now the message ID
		messageStream << "A50";

		// Now the actual data - this needs to be created separately
		// because the length of the data needs to be indicated 
		// before the data itself (i.e we need to calculate the size of it)

		std::stringstream dataStream;

		// Station or Train ID indicator
		// When this message is sent in response to the STIS GUI
		// this can only ever be the Station ID
		dataStream << m_locationName;

		// DESTINATION 'FIELD'

		// PID from M50 message - should this be 3 characters?
		dataStream << "56";

		// Number of messages 
		dataStream << "01";
		
		// Message Tag - if no message tag, use 12 empty spaces
		dataStream << "            ";

		// Start Time
		dataStream << "20040112152359";

		// End Time
		dataStream << "20040112123000";

		// Priority
		dataStream << "5";

		// Message length - number of characters in message content field below
		dataStream << "060";

		// Message content
		dataStream << "This is a message that is currently being displayed on a PID";

		// Calculate the data length - this is variable
		std::string dataString = dataStream.str();
		int dataSize = dataString.size();

		if( dataSize > 1000 )
		{
			
			messageStream << dataSize;
		}
		else if( dataSize > 100 )
		{
			messageStream << "0";
			messageStream << dataSize;
		}
		else if( dataSize > 10 )
		{
			messageStream << "00";
			messageStream << dataSize;
		}
		else 
		{
			messageStream << "000";
			messageStream << dataSize;
		}
		
		// Add the data stream to the actual message stream after calculating its size
		messageStream << dataString;	

		// No train data

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
				
		sendPacket( messageVector );

		FUNCTION_EXIT;
	}


	//
	//sendUpgradePredefinedMessageLibraryReport
	//
	void STISMessageProcessor::sendUpgradePredefinedMessageLibraryReport( std::vector< unsigned char >& command )
	{
		FUNCTION_ENTRY("STISMessageProcessor::sendUpgradePredefinedMessageLibraryReport");

		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(command.begin()+( STX_SZ ), command.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		// Add the sequence number
		messageStream << sequenceNumber;

		std::string timeStamp = getNewTimestampString();

		// Add the timestamp to the message
		messageStream << timeStamp;

		// Now the message ID
		messageStream << "A70";

		// Now the data length - hardcoded in this message - it will always be 3
		// Just the current Station library version
		messageStream << "0003";

		// Now the actual data

		// Report station ??  this should be the same as supplied in the source message
		messageStream << "001";

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
			
		sendPacket( messageVector );



		FUNCTION_EXIT;
	}

	//
	//sendPIDControlReport
	//
	void STISMessageProcessor::sendPIDControlReport( std::vector< unsigned char >& command )
	{
		FUNCTION_ENTRY("STISMessageProcessor::sendPIDControlReport");

#ifdef WIN32
        std::string tmp(command.begin(), command.end());
#else
        std::string tmp(command.begin(), command.end());
#endif
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"Control Packet received: %s", tmp.c_str());

		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(command.begin()+( STX_SZ ), command.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		// Add the sequence number
		messageStream << sequenceNumber;

		std::string timeStamp = getNewTimestampString();

		// Add the timestamp to the message
		messageStream << timeStamp;

		// Now the message ID
		messageStream << "A21";

		// Now the data length - hardcoded in this message - it will always be 3
		// i.e just the 'Report Station' field.
		messageStream << "0003";

		// Now the actual data

		// Report station ??  this should be the same as supplied in the source message
		messageStream << m_locationName;

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
			
		sendPacket( messageVector );


		FUNCTION_EXIT;
	}


	//
	// sendNack
	//
	void STISMessageProcessor::sendNack( std::vector< unsigned char >& command, int reason)
	{
		FUNCTION_ENTRY("STISMessageProcessor::sendNack");
  
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"Received invalid packet - sending Nack");

		std::vector<unsigned char> messageVector;
		std::stringstream messageStream;

		// Extract the sequenceNumber from the original message
		char sequenceNumber[SQN_SZ + 1];
    
		std::copy(command.begin()+( STX_SZ ), command.begin()+( STX_SZ + SQN_SZ), sequenceNumber);
		sequenceNumber[SQN_SZ] = '\0';

		// Add the sequence number
		messageStream << sequenceNumber;

		std::string timeStamp = getNewTimestampString();

		// Add the timestamp to the message
		messageStream << timeStamp;

		// Now the message ID
		messageStream << "A99";

		// Now the data length - this is always 1 bytes, i.e just the 'Reason' field
		messageStream << "0001";

		// Now the actual data

		// Reason
		messageStream << reason;

		// Now the end of packet character
		messageStream << '\n';

		std::string messageString = messageStream.str();

		messageVector.push_back( 0x02 );
		for(unsigned int i=0;i < messageString.size() ; i++)
		{
			messageVector.push_back( messageString[i] );
		}
		messageVector.push_back( 0x03 );
			
		sendPacket( messageVector );

		FUNCTION_EXIT;
	}

	void STISMessageProcessor::updatePIDStatus( int pidAddress, int status )
	{

		std::map<int,int>::iterator iter;

		int i = m_pidStatusMap.size();
		iter = m_pidStatusMap.find(pidAddress);

		iter->second = status;
	}

	void STISMessageProcessor::updateResourceStatus( const std::string& resourceName, int status )
	{

		if( resourceName.compare("rxhlink") == 0 )
		{
			m_rxhLinkSummary = status;
		}
		else if( resourceName.compare("connectionlink") == 0 )
		{
			m_connectionLinkSummary = status;
		}
		else if( resourceName.compare("clocklink") == 0 )
		{
			m_clockLinkSummary = status;
		}
		else if( resourceName.compare("occtisserver") == 0 )
		{
			m_tisOccServerSummary = status;
		}
		else if( resourceName.compare("ledsummary") == 0 )
		{
			m_ledDisplaySummary = status;
		}
		else if( resourceName.compare("plasmasummary") == 0 )
		{
			m_plasmaDisplaySummary = status;
		}
		else if (resourceName.compare("cctserver") == 0 )
		{
			m_cctComputerStatus = status;
		}
	}

	void STISMessageProcessor::displayResourceStatus()
	{
		std::cout << "\nMessage Library Information: \n";
		std::cout << "Current STIS - " << m_currentStisLibrary << "\n";
		std::cout << "Current TTIS - " << m_currentTtisLibrary << "\n";
		std::cout << "Next STIS    - " << m_newStisLibrary << "\n";
		std::cout << "Next TTIS    - " << m_newTtisLibrary << "\n";
		
		if( m_locationName.compare("OCC") == 0 )
		{
			std::cout << "\nOCC Resource Status: ( 0 - Normal, 1 - Alarm )" << "\n";
			std::cout << "rxhlink        - " << m_rxhLinkSummary << "\n";
			std::cout << "connectionlink - " << m_connectionLinkSummary << "\n";
			std::cout << "clockink       - " << m_clockLinkSummary << "\n";
			std::cout << "occtisserver   - " << m_tisOccServerSummary << "\n";
			std::cout << "cctserver      - " << m_cctComputerStatus << "\n";
		}
		else // display station resources
		{
			std::cout << "\n" << m_locationName << " Resource Status: ( 0 - Normal, 1 - Alarm )" << "\n";
			std::cout << "connectionlink - " << m_connectionLinkSummary << "\n";
			std::cout << "clocklink      - " << m_clockLinkSummary << "\n";
			std::cout << "plasmasummary  - " << m_plasmaDisplaySummary << "\n";
			std::cout << "ledsummary     - " << m_ledDisplaySummary << "\n";

			std::cout << "\nPID Status: ( 0 - off, 1 - on, 2 - failure )" << "\n";

			std::map<int,int>::iterator iter;
			for( iter = m_pidStatusMap.begin() ; iter != m_pidStatusMap.end() ; iter++)
			{
				std::cout << "PID " << iter->first << " - ";
				std::cout << iter->second << "\n";
			}
		}

	}

	void STISMessageProcessor::updateNextLibraryVersion( const std::string& libraryType, unsigned long version )
	{

		if( libraryType.compare("STIS") == 0 )
		{
			m_newStisLibrary = version;
		}
		else
		{
			m_newTtisLibrary = version;
		}

		writeLibraryVersions();
	}

	void STISMessageProcessor::updateCurrentSTISLibraryVersion( unsigned long version )
	{

		m_currentStisLibrary = version;

		writeLibraryVersions();
	}

	void STISMessageProcessor::updateCurrentTTISLibraryVersion( unsigned long version )
	{
		m_currentTtisLibrary = version;
		writeLibraryVersions();
	}

	void STISMessageProcessor::upgradeSTISLibrary( unsigned long version )
	{
		m_currentStisLibrary = version;

		writeLibraryVersions();
	}

	void STISMessageProcessor::initialise( const std::string& location )
	{

		m_locationName = location;

		// Set up the initial status for each PID
		if( m_locationName.compare("DBG") == 0 )
		{
			int i;
			for( i=0 ; i < 6 ; i++ ) // 6 PIDs at DBG
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("BBS") == 0 )
		{
			int i;
			for( i=0 ; i < 11 ; i++ ) // 11 PIDs at BBS (MSM)
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("EPN") == 0 )
		{
			int i;
			for( i=0 ; i < 12 ; i++ ) // 12 PIDs at EPN (CVC)
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("PMN") == 0 )
		{
			int i;
			for( i=0 ; i < 10 ; i++ ) // 12 PIDs at PMN (MLN)
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("NCH") == 0 )
		{
			int i;
			for( i=0 ; i < 7 ; i++ ) // 12 PIDs at NCH
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("SDM") == 0 )
		{
			int i;
			for( i=0 ; i < 8 ; i++ ) // 8 PIDs at SDM (BLV)
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("MBT") == 0 )
		{
			int i;
			for( i=0 ; i < 7 ; i++ ) // 7 PIDs at MBT (OAR)
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("DKT") == 0 )
		{
			int i;
			for( i=0 ; i < 8 ; i++ ) // 8 PIDs at DKT (TKT)
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("PYL") == 0 )
		{
			int i;
			for( i=0 ; i < 15 ; i++ ) // 15 PIDs at PYL
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("MPS") == 0 )
		{
			int i;
			for( i=0 ; i < 9 ; i++ ) // 9 PIDs at MPS
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("TSG") == 0 )
		{
			int i;
			for( i=0 ; i < 8 ; i++ ) // 8 PIDs at TSG (UPL)
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		//TD 16466
		else if( m_locationName.compare("BLY") == 0 )
		{
			int i;
			for( i=0 ; i < 9 ; i++ ) // 9 PIDs at BLY
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("SER") == 0 )
		{
			int i;
			for( i=0 ; i < 9 ; i++ ) // 9 PIDs at SER
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("LRC") == 0 )
		{
			int i;
			for( i=0 ; i < 9 ; i++ ) // 9 PIDs at LRC
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("BSH") == 0 )
		{
			int i;
			for( i=0 ; i < 10 ; i++ ) // 10 PIDs at BSH
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("MRM") == 0 )
		{
			int i;
			for( i=0 ; i < 8 ; i++ ) // 8 PIDs at MRM
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("TSN") == 0 )
		{
			int i;
			for( i=0 ; i < 7; i++ ) // 7 PIDs at TSN
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("BKB") == 0 )
		{
			int i;
			for( i=0 ; i < 0 ; i++ ) // UNKNOWN PIDs at BKB
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("BTN") == 0 )
		{
			int i;
			for( i=0 ; i < 6 ; i++ ) // 6 PIDs at BTN (ADA)
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("FRR") == 0 )
		{
			int i;
			for( i=0 ; i < 8 ; i++ ) // 8 PIDs at FRR
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("HLV") == 0 )
		{
			int i;
			for( i=0 ; i < 8 ; i++ ) // 8 PIDs at HLV (HLD)
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("BNV") == 0 )
		{
			int i;
			for( i=0 ; i < 9 ; i++ ) // 9 PIDs at BNV
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("ONH") == 0 )
		{
			int i;
			for( i=0 ; i < 8 ; i++ ) // 8 PIDs at ONH
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}

		
		else if( m_locationName.compare("KRG") == 0 )
		{
			int i;
			for( i=0 ; i < 8 ; i++ ) // 8 PIDs at KRG (NUH)
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("WCT") == 0 )
		{
			int i;
			for( i=0 ; i < 7 ; i++ ) // 7 PIDs at WCT
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("PPJ") == 0 )
		{
			int i;
			for( i=0 ; i < 6 ; i++ ) // 6 PIDs at PPJ
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		//TD 16466

		else if( m_locationName.compare("LBD") == 0 )
		{
			int i;
			for( i=0 ; i < 6 ; i++ ) // 6 PIDs at LBD
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("TLB") == 0 )
		{
			int i;
			for( i=0 ; i < 6 ; i++ ) // 6 PIDs at TLB
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}
		else if( m_locationName.compare("HBF") == 0 )
		{
			int i;
			for( i=0 ; i < 8 ; i++ ) // 6 PIDs at HBF
			{
				m_pidStatusMap.insert( std::pair<int,int>( i+1 , 1 ) ); // Assign initial status of '1' - 'ON'
			}

		}

		// Initialise all the status points to 0 - Normal
		m_rxhLinkSummary = 0;
		m_connectionLinkSummary = 0;
		m_clockLinkSummary = 0;
		m_tisOccServerSummary = 0;
		m_ledDisplaySummary = 0;
		m_plasmaDisplaySummary = 0;
		m_currentStisLibrary = 0;
		m_newStisLibrary = 0;
		m_newTtisLibrary = 0;


        // stis file
		std::stringstream fileNameStream;
		// Filename format is:
		// stisstatusXXX.txt - where XXX is the location
		fileNameStream << "stisstate";
		fileNameStream << m_locationName;
		fileNameStream << ".txt";
		m_filename = fileNameStream.str();

		//char filename[50];

		//sprintf(filename,"%s",fileName.c_str() );

		// See if the file already exists
		// Attempt to open it without creating one

		std::fstream testfile;
		//m_statusFile.open( filename, std::ios_base::in );
		testfile.open( m_filename.c_str(), std::ios_base::in );

		if( testfile.good() )  // If the file exists
		{
			testfile.close();
			// Close it as it needs to be reopened in read/write mode
		//	m_statusFile.close();

			//m_statusFile.open( filename, std::fstream::in | std::fstream::out | std::fstream::app | std::fstream::binar);

			// Read the values from the file because it already exists
			readLibraryVersions();
		}
		else
		{
			// Need to close this for some weird reason.
			//m_statusFile.close();
			testfile.close();

			// write the initial library version information to the file
			writeLibraryVersions();
		}

	}

	void STISMessageProcessor::readLibraryVersions()
	{
		m_statusFile.open( m_filename.c_str(), std::ios_base::in );

		if( m_statusFile.is_open() )
		{
			m_statusFile >> m_currentStisLibrary;
			m_statusFile >> m_currentTtisLibrary;
			m_statusFile >> m_newStisLibrary;
			m_statusFile >> m_newTtisLibrary;
		}
		else
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			"Satus file is not open - cannot write status information");
		}

		m_statusFile.flush();
		m_statusFile.close();
	}

	void STISMessageProcessor::writeLibraryVersions()
	{
		// The file did not exist, so create a new one 
		// but do not read values from the file - retain the values above
		m_statusFile.open( m_filename.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::trunc );

		if( m_statusFile.is_open() )
		{
			m_statusFile << m_currentStisLibrary << "\n";
			m_statusFile << m_currentTtisLibrary << "\n";
			m_statusFile << m_newStisLibrary << "\n";
			m_statusFile << m_newTtisLibrary << "\n";
		}
		else
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"Status file is not open - cannot write status information");
		}

		m_statusFile.flush();
		m_statusFile.close();

	}


	//
	// methods for storing what's to send in the ratis packet.  We're just going to store this
    // command temporarily and send it later on in our OCC status packet
	//

	void STISMessageProcessor::sendRATISPacketToStation( const std::string& command )
	{
        m_ratisCommand = command;
	}

	void STISMessageProcessor::sendRATISPacketToTrain( const std::string& command )
	{
        m_ratisCommand = "TRN " + command;
	}

	void STISMessageProcessor::sendRATISPacketToAllStations( const std::string& command )
	{
        m_ratisCommand = "AST " + command;
	}

	void STISMessageProcessor::sendRATISPacketToAllTrains( const std::string& command )
	{
        m_ratisCommand = "ATR " + command;
	}
}
