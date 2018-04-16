/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_simulator/protocols/COE/src/COECommsObserver.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * CommsObserver manages COE server communications for particual host:port.
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <iostream>
#include <iomanip>

#include "app/cctv/video_switch_simulator/protocols/COE/src/COECommsObserver.h"
#include "app/cctv/video_switch_simulator/protocols/COE/src/COEEquipmentState.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    const unsigned char PHB_ERROR = 0x02;
    const unsigned char BADCMD_ERROR = 0x04;

    //////////////////////////////////////////////////////////////////////////////
    //          Construction/Destruction
    //////////////////////////////////////////////////////////////////////////////

    COECommsObserver::COECommsObserver(const std::string& simulatorName, const std::string& location, bool isActive, TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > * socket)
    :           m_simulatorName( simulatorName ),
                m_location( location ),
                m_isActive( isActive ),
                m_clientSocket(socket),
                m_stillConnected(true),
                m_messageBuffer(),
                m_currentPacketLength(0)
    {
        setUpCommandToRangeAndProcessorMappings();
        socket->addObserver(* this);
        socket->start();

    }


    COECommsObserver::~COECommsObserver()
    {
        FUNCTION_ENTRY("~COECommsObserver");
        if( 0 != m_clientSocket )
        {
            //terminate the thread the socket is holding.
            m_clientSocket->terminate();
            delete m_clientSocket;
            m_clientSocket = 0;
        }

        FUNCTION_EXIT;
    }


    //////////////////////////////////////////////////////////////////////////////
    //                                                                          //
    //                          ITcpObserver methods                            //
    //                                                                          //
    //////////////////////////////////////////////////////////////////////////////

    //
    //processReceivedData
    //
    void COECommsObserver::processReceivedData( std::vector<unsigned char> & pData, TA_Base_Core::ISocket * pSocket )
    {
        FUNCTION_ENTRY("processReceivedData");

        std::stringstream debugMessage;
        debugMessage << "Observer received message: ";
        std::vector<unsigned char>::iterator itr = pData.begin();
        for ( ; itr != pData.end(); ++itr )
        {
            debugMessage << "0x" << std::hex << (short)*itr << std::dec << " ";
			//std::cout << "0x" << std::hex << (short)*itr << std::dec << " ";
        }
		//std::cout << std::endl;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, debugMessage.str().c_str() );
        //std::cout << debugMessage.str() << std::endl;


        while ( !pData.empty() )
        {
            m_messageBuffer.push_back(*(pData.begin()));
            pData.erase(pData.begin());

            if ( 0 == m_currentPacketLength )
            {
                // If we have packet heading length and command bytes, validate
                // them.  
                if ( m_messageBuffer.size() > COMMAND_BYTE )
                {
                    validateRequestPacketHeader();
                }
            }

            //
            // IMPORTANT - don't replace with "else" here as we want to pass through on the cycle
            // that reaches minimum packet length.
            //

            if ( 0 != m_currentPacketLength )
            {
                // A valid packet length/command combo has been received.
                if ( m_messageBuffer.size() == static_cast< unsigned int >( m_currentPacketLength + NUM_LENGTH_BYTES ) )
                {
                    processCurrentPacket();
                    // Clear the buffer
                    m_messageBuffer.clear();
                    m_currentPacketLength = 0;
                }
            }
        }

        FUNCTION_EXIT;
    }


    //
    //writeFailed
    //
    void COECommsObserver::writeFailed()
    {
        //should probably throw some sort of exception.
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Write failed!" );
    }


    //
    //connectionLost
    //
    bool COECommsObserver::connectionLost()
    {
        m_stillConnected = false;

        return false; // Tell observer to terminate, don't attempt re-connect
    }


    //
    //connectionEstablished
    //
    void COECommsObserver::connectionEstablished()
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
             "Connection to client established.");
        m_stillConnected = true;
    }


    //////////////////////////////////////////////////////////////////////////////
    //                                                                          //
    //                          Class specific Functions                        //
    //                                                                          //
    //////////////////////////////////////////////////////////////////////////////


    //
    // setSwitchStatus
    //
    void COECommsObserver::setSwitchStatus( bool isActive )
    {
        m_isActive = isActive;
    }


    //
    // registerCommandWithRangeAndProcessorFunction
    //
    void COECommsObserver::registerCommandWithRangeAndProcessorFunction( unsigned char command, 
        unsigned short minPacketLength, unsigned short maxPacketLength, MessageProcessorFunction processorFunction )
    {
        TA_ASSERT( ( m_packetCommandToRangeAndProcessor.find( command ) == m_packetCommandToRangeAndProcessor.end() ), "Command already registered!" );

        PacketLengthRange range( minPacketLength, maxPacketLength );
        PacketRangeAndProcessor rangeAndProcessor( range, processorFunction );
        m_packetCommandToRangeAndProcessor.insert( PacketCommandToRangeAndProcessor::value_type( command, rangeAndProcessor ) );
    }


    //
    // sendMessage
    //
    void COECommsObserver::sendPacket( const std::vector< unsigned char >& message )
    {
        TA_ASSERT( ( message.size() >= COMMAND_BYTE ), "Invalid packet length!" );
        
		if(message [ COMMAND_BYTE ] != 58)
		{
			std::vector< unsigned char >::const_iterator it;

			//std::cout << "*** sending packet ***" << std::endl;
			for(it=message.begin(); it!=message.end(); it++)
			{
			//	std::cout << "0x" << std::hex << static_cast< unsigned short >(*it) << std::dec << " ";
			}
			//std::cout << std::endl << "*********************" << std::endl;
		}
		
		unsigned short length = getUnsignedShortFromMessageData( message[ MSB_LENGTH_BYTE ], message[ LSB_LENGTH_BYTE ] );

        TA_ASSERT( ( message.size() == static_cast< unsigned int >( length + NUM_LENGTH_BYTES ) ), 
            "Packet length does not correspond to header length bytes!" );
        
        // The following is safe as vector stores elements in contiguous memory and sizeOf unsigned char
        // equal to sizeOf char (1 byte).
        m_clientSocket->write( reinterpret_cast< const char* >( &message[ 0 ] ), message.size() );
    }


    //
    // sendMonitorState
    //
    void COECommsObserver::sendMonitorState( unsigned char monitorID, const COEEquipmentState::MonitorState& state )
    {

		if(monitorID == 11)
		{
			int hello=0;
		}
        		std::vector< unsigned char > packet;
        if(state.assignmentType == COEEquipmentState::ALARM)
		{
			int length = 4 + state.alarmStack.size()*2;
			packet.assign( length + 2  , 0x00 );
			unsigned char test;
			test=length % 0x0100;
			packet[ LSB_LENGTH_BYTE ] = length % 0x0100;
			test = length / 0x0100;
			packet[ MSB_LENGTH_BYTE ] = length / 0x0100;
			
        }
		else if(state.assignmentType == COEEquipmentState::PARKED)
		{
			packet.assign( 6, 0x00 );
			packet[ LSB_LENGTH_BYTE ] = 0x04;
        }
		else
		{
			packet.assign( 8, 0x00 );
			packet[ LSB_LENGTH_BYTE ] = 0x06;
        }
        packet[ COMMAND_BYTE ] = 0x38;
        
		packet[ COMMAND_BYTE + 1 ] = monitorID;
		packet[ COMMAND_BYTE + 2 ] = static_cast< unsigned char >( state.assignmentType );
        
		if (state.assignmentType == COEEquipmentState::ALARM)
		{
			TriggeringEventList::const_iterator itr;
			int i=3;
			for(itr = state.alarmStack.begin(); itr != state.alarmStack.end(); itr++, i+=2)
			{
				packet[ COMMAND_BYTE + i ] = (*itr).eventId;
				packet[ COMMAND_BYTE + i + 1 ] = (*itr).stationId;
			}
		}
		else if (state.assignmentType == COEEquipmentState::PARKED)
		{
			//do nothing we are done
		}
		else if ( state.assignmentType != COEEquipmentState::TRAIN )
        {
            packet[ COMMAND_BYTE + 3 ] = state.assignedDeviceID;
			packet[ COMMAND_BYTE + 4 ] = state.assignedDeviceLocationKey;
        }
		else
        {
            packet[ COMMAND_BYTE + 3 ] = 0x00;
            packet[ COMMAND_BYTE + 4 ] = state.assignedDeviceLocationKey;
        }

        sendPacket( packet );
    }


    //
    // sendSequenceState
    //
    void COECommsObserver::sendSequenceState( unsigned char sequenceID, const COEEquipmentState::SequenceState& state )
    {
        TA_ASSERT( ( ( state.orderedCameras.size() % 2 ) == 0 ), "Not even number of Station ID to Camera ID count!" );

        unsigned short length( 0 );
        if ( state.orderedCameras.size() != 0 )
        {
            length = static_cast< unsigned short >( 4 + state.orderedCameras.size() );
        }
        else
        {
            length = 3;
        }

        std::vector< unsigned char > packet( ( NUM_LENGTH_BYTES + length ), 0x00 );

        TA_ASSERT( ( length < 255 ), "Too many cameras configured in sequence to packetise according to COE protocol!" );

        packet[ LSB_LENGTH_BYTE ] = length % 0x0100;
        packet[ MSB_LENGTH_BYTE ] = length / 0x0100;
        packet[ COMMAND_BYTE ] = 0x36;
        packet[ COMMAND_BYTE + 1 ] = sequenceID;

        if ( length > 3 )
        {
            packet[ COMMAND_BYTE + 2 ] = state.dwellTime % 0x0100;
        }

        int packetIndex = COMMAND_BYTE + 3;
        std::vector< unsigned char >::const_iterator it = state.orderedCameras.begin();
        while ( it != state.orderedCameras.end() )
        {
            packet[ ( packetIndex + 1 ) ] = ( *it );
            it++;
            packet[ packetIndex ] = ( *it );
            it++;
            packetIndex+=2;
        }

        sendPacket( packet );
    }


    //
    // sendQuadState
    //
    void COECommsObserver::sendQuadState( unsigned char quadID, const COEEquipmentState::QuadState& state )
    {
        TA_ASSERT( ( state.size() == 8 ), "Need 4 Station:Camera pairs to capture Quad State!" );

        std::vector< unsigned char > packet( ( 3 + NUM_LENGTH_BYTES ), 0x00 );
        packet[ LSB_LENGTH_BYTE ] = 0x0B;
        packet[ COMMAND_BYTE ] = 0x35;
        packet[ COMMAND_BYTE + 1 ] = quadID;

        packet.insert( packet.end(), state.begin(), state.end() );

        sendPacket( packet );
    }


    //
    // sendRecordingUnitState
    //
    void COECommsObserver::sendRecordingUnitState( unsigned char recordingUnitID, COEEquipmentState::ERecordingUnitState state )
    {
        std::vector< unsigned char > packet( 6, 0x00 );
        packet[ LSB_LENGTH_BYTE ] = 0x04;
        packet[ COMMAND_BYTE ] = 0x37;
        packet[ COMMAND_BYTE + 1 ] = recordingUnitID;
        packet[ COMMAND_BYTE + 2 ] = static_cast< unsigned char >( state );

        sendPacket( packet );
    }


    //
	// sendTriggeringEvents
	//
	void COECommsObserver::sendTriggeringEvents( TriggeringEventList& events )
	{
        unsigned short length( 0 );
        length = static_cast< unsigned short >( events.size()*2 + 4 );
        
        std::vector< unsigned char > packet( ( 2 + length ), 0x00 );

        packet[ LSB_LENGTH_BYTE ] = length % 0x0100;
        packet[ MSB_LENGTH_BYTE ] = length / 0x0100;
        packet[ COMMAND_BYTE ] = 0x3B;
        packet[ COMMAND_BYTE + 1 ] = events.size() % 0x0100;
		packet[ COMMAND_BYTE + 2 ] = events.size() / 0x0100;

		int i=3;
		TriggeringEventList::iterator itr;

		for(itr=events.begin(); itr!=events.end(); itr++, i+=2)
        {
			packet[ COMMAND_BYTE + i ] = (*itr).stationId;
			packet[ COMMAND_BYTE + i + 1 ] = (*itr).eventId;
		}

        sendPacket( packet );
	}
			
			
	//
    // setUpCommandToRangeAndProcessorMappings
    //
    void COECommsObserver::setUpCommandToRangeAndProcessorMappings()
    {
        registerCommandWithRangeAndProcessorFunction( 0x06, 3, 3, processClearMon);
        registerCommandWithRangeAndProcessorFunction( 0x12, 4, 4, processSetRecUnit);
        registerCommandWithRangeAndProcessorFunction( 0x17, 3, 258, processProgSeq);
        registerCommandWithRangeAndProcessorFunction( 0x18, 11, 11, processSetQuad);
        registerCommandWithRangeAndProcessorFunction( 0x21, 5, 5, processSelectCamAndMon );
        registerCommandWithRangeAndProcessorFunction( 0x22, 4, 4, processSelectSeqAndMon );
        registerCommandWithRangeAndProcessorFunction( 0x23, 4, 4, processControlSeq );
        registerCommandWithRangeAndProcessorFunction( 0x24, 4, 4, processSelectQuadAndMon );
        registerCommandWithRangeAndProcessorFunction( 0x25, 4, 4, processSelectBVSStageAndMon );
        registerCommandWithRangeAndProcessorFunction( 0x26, 4, 4, processAckTrainAlarm );
		registerCommandWithRangeAndProcessorFunction( 0x27, 4, 4, processClearTrainAlarm );
		registerCommandWithRangeAndProcessorFunction( 0x35, 3, 3, processRetQuadStat );
        registerCommandWithRangeAndProcessorFunction( 0x36, 3, 3, processRetSeqStat );
        registerCommandWithRangeAndProcessorFunction( 0x37, 3, 3, processRetRecUnitStat );
        registerCommandWithRangeAndProcessorFunction( 0x38, 3, 3, processRetMonStat );
        registerCommandWithRangeAndProcessorFunction( 0x3a, 2, 2, processRetSummaryStat );
		registerCommandWithRangeAndProcessorFunction( 0x3b, 2, 2, processRetTriggeringEvents );
		registerCommandWithRangeAndProcessorFunction( 0x3c, 5, 5, processAckTriggeringEvent );
		registerCommandWithRangeAndProcessorFunction( 0x3d, 4, 4, processClearTriggeringEvent );
		

    }


    //
    // processSelectCamAndMon
    //
    void COECommsObserver::processSelectCamAndMon( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) == ( 5 + NUM_LENGTH_BYTES ) ), "Incorrect message size for Cam to Monitor command!" );

        COECommsObserver::printValidPacketReceived( messageBegin, messageEnd, commsObserver );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x21 ), "Command is not for Cam to Monitor!" );

        messageBegin++; // increment to camera location key.
        COEEquipmentState::MonitorState newState;
		newState.assignmentType = COEEquipmentState::CAMERA;
        newState.assignedDeviceLocationKey = ( *messageBegin );
        messageBegin++; // increment to camera ID
        newState.assignedDeviceID = ( *messageBegin );
        messageBegin++; // increment to Monitor ID
		newState.monitorId = ( *messageBegin );
        
        std::cout << commsObserver.getLocation() << " received request to switch Camera ";
        std::cout << static_cast< unsigned short >( newState.assignedDeviceLocationKey );
        std::cout << ":" << static_cast< unsigned short >( newState.assignedDeviceID ) << " ";
        std::cout << "to Monitor " << static_cast< unsigned short >( newState.monitorId ) << ".";
        std::cout << std::endl;

        std::vector< unsigned char > response( 4, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong)
        response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2
        response[ COMMAND_BYTE ] = 0x21; // Cam to Mon command

        commsObserver.sendPacket( response );

        COEEquipmentState::getInstance().setMonitorState( commsObserver.getLocation(), ( *messageBegin ), newState, true );
    }


    //
    // processSelectSeqAndMon
    //
    void COECommsObserver::processSelectSeqAndMon( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) == ( 4 + NUM_LENGTH_BYTES ) ), "Incorrect message size for Seq to Monitor command!" );

        COECommsObserver::printValidPacketReceived( messageBegin, messageEnd, commsObserver );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x22 ), "Command is not for Seq to Monitor!" );

        messageBegin++; // increment to sequence ID.
        COEEquipmentState::MonitorState newState;
		newState.assignmentType = COEEquipmentState::SEQUENCE;
        newState.assignedDeviceLocationKey = 0x00;
        newState.assignedDeviceID = ( *messageBegin );
        messageBegin++; // increment to Monitor ID
		newState.monitorId = ( *messageBegin );
        
        std::cout << commsObserver.getLocation() << " received request to switch Sequence ";
        std::cout << static_cast< unsigned short >( newState.assignedDeviceID );
        std::cout << " to Monitor " << static_cast< unsigned short >( newState.monitorId ) << ".";
        std::cout << std::endl;

        std::vector< unsigned char > response( 4, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong).
        response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2.
        response[ COMMAND_BYTE ] = 0x22; // Seq to Mon command.

        commsObserver.sendPacket( response );

        COEEquipmentState::getInstance().setMonitorState( commsObserver.getLocation(), ( *messageBegin ), newState, true );
    }


    //
    // processSelectQuadAndMon
    //
    void COECommsObserver::processSelectQuadAndMon( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) == ( 4 + NUM_LENGTH_BYTES ) ), "Incorrect message size for Quad to Monitor command!" );

        COECommsObserver::printValidPacketReceived( messageBegin, messageEnd, commsObserver );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x24 ), "Command is not for Quad to Monitor!" );

        messageBegin++; // increment to sequence ID.
        COEEquipmentState::MonitorState newState;
        newState.assignmentType = COEEquipmentState::QUAD;
        newState.assignedDeviceLocationKey = 0x00;
        newState.assignedDeviceID = ( *messageBegin );
        messageBegin++; // increment to Monitor ID
		newState.monitorId = ( *messageBegin );
        
        std::cout << commsObserver.getLocation() << " received request to switch Quad ";
        std::cout << static_cast< unsigned short >( newState.assignedDeviceID );
        std::cout << " to Monitor " << static_cast< unsigned short >( newState.monitorId ) << ".";
        std::cout << std::endl;

        std::vector< unsigned char > response( 4, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong).
        response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2.
        response[ COMMAND_BYTE ] = 0x24; // Quad to Mon command.

        commsObserver.sendPacket( response );

        COEEquipmentState::getInstance().setMonitorState( commsObserver.getLocation(), ( *messageBegin ), newState, true );
    }


    //
    // processSelectBVSStageAndMon
    //
    void COECommsObserver::processSelectBVSStageAndMon( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) == ( 4 + NUM_LENGTH_BYTES ) ), "Incorrect message size for BVSStage to Monitor command!" );

        COECommsObserver::printValidPacketReceived( messageBegin, messageEnd, commsObserver );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x25 ), "Command is not for BVSStage to Monitor!" );

        messageBegin++; // increment to BVS Stage ID.
        COEEquipmentState::MonitorState newState;
        newState.assignmentType = COEEquipmentState::TRAIN;
        newState.assignedDeviceLocationKey = ( *messageBegin );
        newState.assignedDeviceID = 0x00;
        messageBegin++; // increment to Monitor ID
		newState.monitorId = ( *messageBegin );
        
        std::cout << commsObserver.getLocation() << " received request to switch Active Train in BVS Stage ";
        std::cout << static_cast< unsigned short >( newState.assignedDeviceLocationKey );
        std::cout << " to Monitor " << static_cast< unsigned short >( newState.monitorId ) << ".";
        std::cout << std::endl;

        std::vector< unsigned char > response( 4, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong).
        response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2.
        response[ COMMAND_BYTE ] = 0x25; // BVSStage to Mon command.

        commsObserver.sendPacket( response );

        COEEquipmentState::getInstance().setMonitorState( commsObserver.getLocation(), ( *messageBegin ), newState, true );
    }


    //
    // processAckTrainAlarm
    //
    void COECommsObserver::processAckTrainAlarm( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) == ( 4 + NUM_LENGTH_BYTES ) ), "Incorrect message size for Ack train alarm command!" );

        COECommsObserver::printValidPacketReceived( messageBegin, messageEnd, commsObserver );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x26 ), "Command is not for Ack train alarm!" );

        messageBegin++; // increment to BVS Stage ID.
		unsigned char stageId = (*messageBegin);

		messageBegin++;
		unsigned char monitorId = (*messageBegin);

        COEEquipmentState::getInstance().openTriggeringEvent(commsObserver.getLocation(), stageId, 0, monitorId);
	}
    
	//
    // processClearTrainAlarm
    //
    void COECommsObserver::processClearTrainAlarm( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) == ( 4 + NUM_LENGTH_BYTES ) ), "Incorrect message size for Clear train alarm command!" );

        COECommsObserver::printValidPacketReceived( messageBegin, messageEnd, commsObserver );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x27 ), "Command is not for Clear train alarm!" );

        messageBegin++; // increment to BVS Stage ID.
		unsigned char stageId = (*messageBegin);

		messageBegin++;
		unsigned char monitorId = (*messageBegin);

        COEEquipmentState::getInstance().closeTriggeringEvent(commsObserver.getLocation(), stageId, 0, monitorId);
	}
    
	//
    // processClearMon
    //
    void COECommsObserver::processClearMon( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) == ( 3 + NUM_LENGTH_BYTES ) ), "Incorrect message size for Cam to Monitor command!" );

        COECommsObserver::printValidPacketReceived( messageBegin, messageEnd, commsObserver );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x06 ), "Command is not for Clear Monitor!" );
        
        if((COEEquipmentState::getInstance().getMonitorState( commsObserver.getLocation(), ( *messageBegin ))).assignmentType != COEEquipmentState::ALARM)
		{

			messageBegin++;
			TriggeringEventList list;
			COEEquipmentState::MonitorState newState;
			newState.monitorId = ( *messageBegin );
			newState.assignmentType = COEEquipmentState::PARKED;
			newState.assignedDeviceID = 0x00;
			newState.assignedDeviceLocationKey = 0x00;
			newState.alarmStack = list;

			std::cout << commsObserver.getLocation() << " received request to clear Monitor ";
			std::cout << static_cast< unsigned short >( *messageBegin ) << "." << std::endl;

			std::vector< unsigned char > response( 4, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong)
			response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2
			response[ COMMAND_BYTE ] = 0x06; // Clear Mon command

			commsObserver.sendPacket( response );

			COEEquipmentState::getInstance().setMonitorState( commsObserver.getLocation(), ( *messageBegin ), newState, true );
		}
		else
		{
			//not valid for alarm state monitors
			std::vector< unsigned char > response( 4, 0x00 );
			response[ ERROR_BYTE ] = PHB_ERROR; // command prohibited at the moment
			response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2
			response[ COMMAND_BYTE ] = 0x06; // Clear Mon command

			commsObserver.sendPacket( response );
		}

    }


    //
    // processSetRecUnit
    //
    void COECommsObserver::processSetRecUnit( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) == ( 4 + NUM_LENGTH_BYTES ) ), "Incorrect message size for set RecUnit command!" );

        COECommsObserver::printValidPacketReceived( messageBegin, messageEnd, commsObserver );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x12 ), "Command is not for set RecUnit!" );

        messageBegin++; // increment to recUnit ID.
        unsigned char recUnitID = ( *messageBegin );

        messageBegin++; // increment to recUnitState to set;
        if ( ( ( *messageBegin ) == 0x01 ) || ( ( *messageBegin ) == 0x02 ) )
        {
            unsigned short stateToBeSetByte = static_cast< unsigned short >(  ( *messageBegin ) + 0x01 ); // have to add 0x01 to align with state enum.
            COEEquipmentState::ERecordingUnitState newState = static_cast< COEEquipmentState::ERecordingUnitState >( stateToBeSetByte );

            std::cout << commsObserver.getLocation() << " received request to switch recording unit ";
            std::cout << static_cast< unsigned short >( recUnitID ) << " to ";
            switch ( newState )
            {
            case COEEquipmentState::REAL_TIME_RECORDING:
                {
                    std::cout << "real-time recording mode.";
                }
                break;
            case COEEquipmentState::TIME_LAPSE_RECORDING:
                {
                    std::cout << "time-lapse recording mode.";
                }
                break;
            default:
                break;
            }
            std::cout << std::endl;

            std::vector< unsigned char > response( 4, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong).
            response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2.
            response[ COMMAND_BYTE ] = 0x12; // set rec unit command.

            commsObserver.sendPacket( response );

            COEEquipmentState::getInstance().setRecordingUnitState( commsObserver.getLocation(), recUnitID, newState );
        }
        else
        {
            std::cout << commsObserver.getLocation() << " received invalid recording unit setting request." << std::endl;
            // Invalid field in request.
            std::vector< unsigned char > response( 4, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong).
            response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2.
            response[ COMMAND_BYTE ] = 0x12; // set rec unit command.
            response[ ERROR_BYTE ] = BADCMD_ERROR;

            commsObserver.sendPacket( response );
        }
    }


    //
    // processSetQuad
    //
    void COECommsObserver::processSetQuad( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) == ( 11 + NUM_LENGTH_BYTES ) ), "Incorrect message size for set Quad command!" );

        COECommsObserver::printValidPacketReceived( messageBegin, messageEnd, commsObserver );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x18 ), "Command is not for set Quad!" );

        messageBegin++; // increment to Quad ID.
        unsigned char quadID = ( *messageBegin );

        messageBegin++; // increment to beginning of Quad state fields.
        COEEquipmentState::QuadState newState( messageBegin, messageEnd ); // Slots straight into the sim's QuadState Structure.

        std::cout << commsObserver.getLocation() << " received request to set Quad ";
        std::cout << static_cast< unsigned short >( quadID ) << " to:";
        std::cout << std::endl;
        int segNumber = 1;
        while ( messageBegin != messageEnd )
        {
            std::cout << "Quad Segment " << segNumber << " Camera" << " - " <<  static_cast< unsigned short >( *messageBegin );
            messageBegin++;
            std::cout << ":" << static_cast< unsigned short >( *messageBegin ) << std::endl;
            messageBegin++;
            segNumber++;
        }

        std::vector< unsigned char > response( 4, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong).
        response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2.
        response[ COMMAND_BYTE ] = 0x18; // set Quad command.

        commsObserver.sendPacket( response );

        COEEquipmentState::getInstance().setQuadState( commsObserver.getLocation(), quadID, newState );
    }


    //
    // processProgSeq
    //
    void COECommsObserver::processProgSeq( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) >= ( 3 + NUM_LENGTH_BYTES ) ), "Incorrect message size for program Sequence command!" );

        COECommsObserver::printValidPacketReceived( messageBegin, messageEnd, commsObserver );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x17 ), "Command is not for program Sequence!" );

        messageBegin++; // increment to sequence ID.
        unsigned char sequenceID = ( *messageBegin );

        messageBegin++; // increment to dwell time;

        unsigned short dwellTime( 0 );
        if ( messageBegin != messageEnd )
        {
            dwellTime = static_cast< unsigned short >( *messageBegin );

            messageBegin++; // increment to ordered camera list.
        }

        if ( ( ( messageEnd - messageBegin ) % 2 ) == 0 )
        {   
            COEEquipmentState::SequenceState newState;
            newState.dwellTime = dwellTime;
            newState.orderedCameras.insert( newState.orderedCameras.end(), messageBegin, messageEnd );

            std::cout << commsObserver.getLocation() << " received request to program Sequence ";
            std::cout << static_cast< unsigned short >( sequenceID ) << " with:" << std::endl;
            unsigned short camNumber = 1;
            while ( messageBegin != messageEnd )
            {
                std::cout << "Sequence Camera " << camNumber << " - " <<  static_cast< unsigned short >( *messageBegin );
                messageBegin++;
                std::cout << ":" << static_cast< unsigned short >( *messageBegin ) << std::endl;
                messageBegin++;
                camNumber++;
            }

            std::vector< unsigned char > response( 4, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong).
            response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2.
            response[ COMMAND_BYTE ] = 0x17; // prog sequence.

            commsObserver.sendPacket( response );

            COEEquipmentState::getInstance().setSequenceState( commsObserver.getLocation(), sequenceID, newState );
        }
        else
        {
            std::cout << commsObserver.getLocation() << " received invalid program Sequence request ";
            std::cout << "(uneven number of Station to Camera bytes)." << std::endl;
            std::vector< unsigned char > response( 4, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong).
            response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2.
            response[ COMMAND_BYTE ] = 0x17; // prog sequence.
            response[ ERROR_BYTE ] = BADCMD_ERROR;

            commsObserver.sendPacket( response );
        }
    }


    //
    // processProgSeq
    //
    void COECommsObserver::processControlSeq( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) == ( 4 + NUM_LENGTH_BYTES ) ), "Incorrect message size for program Sequence command!" );

        COECommsObserver::printValidPacketReceived( messageBegin, messageEnd, commsObserver );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x23 ), "Command is not for controlling Sequence!" );

        messageBegin++; // increment to control command.
        unsigned char controlCommand = ( *messageBegin );

        messageBegin++; // increment to Monitor ID.
        unsigned char monitorID = ( *messageBegin );        

        if ( ( controlCommand >= 0x01 ) && ( controlCommand <= 0x04 ) )
        {
            std::cout << commsObserver.getLocation() << " received request to set playback of Sequence on Monitor ";
            std::cout << static_cast< unsigned short >( monitorID ) << " to ";
            switch ( controlCommand )
            {
            case 0x01:
                std::cout << "HOLD." << std::endl;
                break;
            case 0x02:
                std::cout << "SKIP FORWARD." << std::endl;
                break;
            case 0x03:
                std::cout << "SKIP BACKWARD." << std::endl;
                break;
            case 0x04:
                std::cout << "PLAY." << std::endl;
                break;
            default:
                break; // should never reach here.
            }

            std::vector< unsigned char > response( 4, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong).
            response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2.
            response[ COMMAND_BYTE ] = 0x23; // prog sequence.

            commsObserver.sendPacket( response );
        }
        else
        {
            std::cout << commsObserver.getLocation() << " received invalid Control Sequence playback request." << std::endl;
            std::vector< unsigned char > response( 4, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong).
            response[ LSB_LENGTH_BYTE ] = 0x02; // length of 2.
            response[ COMMAND_BYTE ] = 0x23; // prog sequence.
            response[ ERROR_BYTE ] = BADCMD_ERROR;

            commsObserver.sendPacket( response );
        }
    }


    //
    // processRetSummaryStat
    //
    void COECommsObserver::processRetSummaryStat( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) >= ( 2 + NUM_LENGTH_BYTES ) ), "Incorrect message size for program Summary Status command!" );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x3a ), "Command is not for Summary Status!" );

        std::vector< unsigned char > response( 5, 0x00 ); // error remains set to 0x00 (i.e. nothing wrong).
        response[ LSB_LENGTH_BYTE ] = 0x03; // length of 2.
        response[ COMMAND_BYTE ] = 0x3a; // prog sequence.

        unsigned char onBit = 0x01;
        if ( commsObserver.isActive() )
        {
            response[ COMMAND_BYTE + 1 ] = ( response[ COMMAND_BYTE + 1 ] | ( onBit << 0 ) );
            if ( COEEquipmentState::getInstance().getSummaryAlarm( COEEquipmentState::SYS_SUM ) )
            {
                response[ COMMAND_BYTE + 1 ] = ( response[ COMMAND_BYTE + 1 ] | ( onBit << 7 ) );
            }
            if ( COEEquipmentState::getInstance().getSummaryAlarm( COEEquipmentState::TRANS_SUM ) )
            {
                response[ COMMAND_BYTE + 1 ] = ( response[ COMMAND_BYTE + 1 ] | ( onBit << 6 ) );
            }
            if ( COEEquipmentState::getInstance().getSummaryAlarm( COEEquipmentState::CAM_SUM ) )
            {
                response[ COMMAND_BYTE + 1 ] = ( response[ COMMAND_BYTE + 1 ] | ( onBit << 5 ) );
            }
            if ( COEEquipmentState::getInstance().getSummaryAlarm( COEEquipmentState::ANC_SUM ) )
            {
                response[ COMMAND_BYTE + 1 ] = ( response[ COMMAND_BYTE + 1 ] | ( onBit << 4 ) );
            }
			if ( COEEquipmentState::getInstance().getSummaryAlarm( COEEquipmentState::DVR_SUM ) )
			{
				response[ COMMAND_BYTE + 1 ] = ( response[ COMMAND_BYTE + 1 ] | ( onBit << 3 ) );
			}
        }
        else
        {
            response[ COMMAND_BYTE + 1 ] = ( response[ COMMAND_BYTE + 1 ] | ( onBit << 2 ) );
        }

        commsObserver.sendPacket( response );
    }


    //
    // processRetTriggeringEvents
    //
    void COECommsObserver::processRetTriggeringEvents( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) >= ( 2 + NUM_LENGTH_BYTES ) ), "Incorrect message size for retrieve triggering events command!" );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x3b ), "Command is not for Triggering Events!" );

		commsObserver.sendTriggeringEvents(COEEquipmentState::getInstance().getTriggeringEvents(commsObserver.getLocation()));
    }


    //
    // processAckTriggeringEvent
    //
    void COECommsObserver::processAckTriggeringEvent( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) >= ( 5 + NUM_LENGTH_BYTES ) ), "Incorrect message size for ack triggering event command!" );

		// TD16829
		COECommsObserver::printValidPacketReceived( messageBegin, messageEnd, commsObserver );
		// TD16829
		messageBegin += COMMAND_BYTE;

		TA_ASSERT( ( ( *messageBegin ) == 0x3c ), "Command is not for Ack Triggering Events!" );

		messageBegin++;
		unsigned char stationId = (*messageBegin);

		messageBegin++;
		unsigned char eventId = (*messageBegin);

		messageBegin++;
		unsigned char monitorId = (*messageBegin);

        COEEquipmentState::getInstance().ackTriggeringEvent(commsObserver.getLocation(), stationId, eventId, monitorId);
    }


    //
    // processAckTriggeringEvent
    //
    void COECommsObserver::processClearTriggeringEvent( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) >= ( 4 + NUM_LENGTH_BYTES ) ), "Incorrect message size for ack triggering event command!" );

        messageBegin += COMMAND_BYTE;

		TA_ASSERT( ( ( *messageBegin ) == 0x3d ), "Command is not for Ack Triggering Events!" );

		messageBegin++;
		unsigned char stationId = (*messageBegin);

		messageBegin++;
		unsigned char eventId = (*messageBegin);

        COEEquipmentState::getInstance().closeTriggeringEvent(commsObserver.getLocation(), stationId, eventId);
    }


    //
    // processRetMonStat
    //
    void COECommsObserver::processRetMonStat( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) >= ( 3 + NUM_LENGTH_BYTES ) ), "Incorrect message size for request Monitor Status command!" );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x38 ), "Command not request for Monitor State!" );

        messageBegin++; // increment to Monitor ID.
        commsObserver.sendMonitorState( ( *messageBegin ), COEEquipmentState::getInstance().
            getMonitorState( commsObserver.getLocation(), ( *messageBegin ) ) );
    }


    //
    // processRetSeqStat
    //
    void COECommsObserver::processRetSeqStat( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) >= ( 3 + NUM_LENGTH_BYTES ) ), "Incorrect message size for request Sequence Status command!" );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x36 ), "Command not request for Sequence State!" );

        messageBegin++; // increment to Sequence ID.
        commsObserver.sendSequenceState( ( *messageBegin ), COEEquipmentState::getInstance().
            getSequenceState( commsObserver.getLocation(), ( *messageBegin ) ) );
    }


    //
    // processRetQuadStat
    //
    void COECommsObserver::processRetQuadStat( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) >= ( 3 + NUM_LENGTH_BYTES ) ), "Incorrect message size for request Quad Status command!" );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x35 ), "Command not request for Quad State!" );

        messageBegin++; // increment to Quad ID.
        commsObserver.sendQuadState( ( *messageBegin ), COEEquipmentState::getInstance().
            getQuadState( commsObserver.getLocation(), ( *messageBegin ) ) );
    }


    //
    // processRetRecUnitStat
    //
    void COECommsObserver::processRetRecUnitStat( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        TA_ASSERT( ( ( messageEnd - messageBegin ) >= ( 3 + NUM_LENGTH_BYTES ) ), "Incorrect message size for request Recording Unit Status command!" );

        messageBegin += COMMAND_BYTE;

        TA_ASSERT( ( ( *messageBegin ) == 0x37 ), "Command not request for Recording Unit State!" );

        messageBegin++; // increment to Rec Unit ID.
        commsObserver.sendRecordingUnitState( ( *messageBegin ), COEEquipmentState::getInstance().
            getRecordingUnitState( commsObserver.getLocation(), ( *messageBegin ) ) );
    }


    //
    // validateRequestPacketHeader
    //
    void COECommsObserver::validateRequestPacketHeader()
    {
        TA_ASSERT( (m_messageBuffer.size() > COMMAND_BYTE), "Insufficient packet header bytes to process." );
    
        PacketCommandToRangeAndProcessor::iterator commandToRangeAndProcessorIt =
            m_packetCommandToRangeAndProcessor.find( m_messageBuffer[ COMMAND_BYTE ] );
        if ( commandToRangeAndProcessorIt != m_packetCommandToRangeAndProcessor.end() )
        {
            m_currentPacketLength = getUnsignedShortFromMessageData( 
                m_messageBuffer[ MSB_LENGTH_BYTE ], m_messageBuffer[ LSB_LENGTH_BYTE ] );
            if ( ( m_currentPacketLength >= ( *commandToRangeAndProcessorIt ).second.first.first ) &&
                ( m_currentPacketLength <= ( *commandToRangeAndProcessorIt ).second.first.second ) )
            {
                // Its valid so return.
                return;
            }

        }

        // Invalid so get rid of the first byte and reset the current packet length back to zero.
        std::cout << "Invalid packet header bytes received, rolling over to next byte.\n";
        m_currentPacketLength = 0;
        m_messageBuffer.erase( m_messageBuffer.begin() );
    }


    //
    // processCurrentPacket
    //
    void COECommsObserver::processCurrentPacket()
    {
        PacketCommandToRangeAndProcessor::iterator commandToRangeAndProcessorIt =
            m_packetCommandToRangeAndProcessor.find( m_messageBuffer[ COMMAND_BYTE ] );

        TA_ASSERT( ( commandToRangeAndProcessorIt != m_packetCommandToRangeAndProcessor.end() ),
            "No processor function maps to current packet, header should not have been validated!" );

        ( *commandToRangeAndProcessorIt ).second.second( m_messageBuffer.begin(), m_messageBuffer.end(), *this );
    }


    //
    // printValidPacketReceived
    //
    void COECommsObserver::printValidPacketReceived( std::vector< unsigned char >::const_iterator messageBegin, std::vector< unsigned char >::const_iterator messageEnd, COECommsObserver& commsObserver )
    {
        std::stringstream printMessage;
        printMessage << commsObserver.getSimulatorName() << " received valid request packet: ";
        while ( messageBegin != messageEnd )
        {
            char t = printMessage.fill();
            printMessage << "0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<unsigned short>(*messageBegin) << std::dec << " ";
            messageBegin++;
        }
        std::cout << printMessage.str() << std::endl;
    }


} // namespace TA_IRS_App