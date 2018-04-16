/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/radio/RadioSimulatorLibrary/src/RadioCommsObserver.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * CommsObserver allows the CommsHandler to delegate incoming socket connections.
  * Each new connection is passed to a new CommsObserver object, a vector of which
  * is held by the WattsSim singleton object.
  */

#include "bus/radio/RadioSimulatorLibrary/src/RadioCommsObserver.h"
#include "bus/radio/RadioSimulatorLibrary/src/SimulatorHelper.h"

#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/ThreasdafeScreenOutput.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>


namespace TA_IRS_Bus
{

    RadioCommsObserver::RadioCommsObserver( TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket >* socket )
     :  m_clientSocket(socket),
        m_stillConnected(true),
        m_unconditionalForwardNumber(""),
        m_notReachableForwardNumber("987-654-321"),
        m_threadPool( TA_Base_Core::ThreadPoolSingletonManager::getInstance() )
    {
        FUNCTION_ENTRY( "RadioCommsObserver" );
    
        m_clientSocket->addObserver(*this);
        m_clientSocket->start();
        
        FUNCTION_EXIT;
    }


    RadioCommsObserver::~RadioCommsObserver()
    {
        FUNCTION_ENTRY( "~RadioCommsObserver" );
        
        if( 0 != m_clientSocket )
        {
            m_clientSocket->terminate();
            
            delete m_clientSocket;
            m_clientSocket = 0;
        }

        TA_Base_Core::ThreadPoolSingletonManager::removeInstance( m_threadPool );
        m_threadPool = NULL;

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::processReceivedData( std::vector<unsigned char>& pData,
                                                  TA_Base_Core::ISocket * pSocket )
    {
        FUNCTION_ENTRY( "processReceivedData" );

        TA_ASSERT( NULL != m_threadPool, "The threadpool is NULL" )

        // The data must be broken into complete packets.
        // The first 4 bytes of every event are the length
        // however the shortest message is 20 bytes

        if ( pData.size() < 20 )
        {
            // wait for more data
            FUNCTION_EXIT;
            return;
        }

        // get all radio methods
        std::vector< IncomingPacketPtr > newPackets = extractPacketsFromRawData( pData );

        // queue each for processing
        for ( std::vector< IncomingPacketPtr >::iterator packetIter = newPackets.begin();
              newPackets.end() != packetIter; ++packetIter )
        {
            TA_Base_Core::IWorkItemPtr workItem( new RadioMethodTask( *this, *packetIter ) );

            m_threadPool->queueWorkItem( workItem );
        }    

        FUNCTION_EXIT;
    }


    std::vector< IncomingPacketPtr > RadioCommsObserver::extractPacketsFromRawData( std::vector<unsigned char>& pData )
    {
        FUNCTION_ENTRY( "extractPacketsFromRawData" );

        std::vector< IncomingPacketPtr > newPackets;

        // we need at least 4 bytes to read the length
        while ( pData.size() > 3 )
        {
            // read the length
            // this is done in little endian, 4 byte length is like this:
            // AA BB CC DD
            // it must be re-arranged to:
            // DD CC BB AA
            unsigned long length = 0;

            length |= ( pData[0] & 0xFF );
            length |= ( ( pData[1] & 0xFF ) << 8 ) & 0xFF00;
            length |= ( ( pData[2] & 0xFF ) << 16 ) & 0xFF0000;
            length |= ( ( pData[3] & 0xFF ) << 24 ) & 0xFF000000;

            // now extract that many bytes from the data
            if ( pData.size() < length )
            {
                // the last packet is not complete
                // leave the remaining data in the socket, and return to wait for more

                FUNCTION_EXIT;
                return newPackets;
            }
            else
            {
                // copy the data to a local vector
                boost::shared_ptr< IncomingPacket > newPacket( new IncomingPacket() );

                for ( unsigned long i = 0; i < length; ++i )
                {
                    // push the next element on
                    newPacket->push_back( pData[0] );

                    // remove the next element from the source
                    pData.erase( pData.begin() );
                }

                newPackets.push_back( newPacket );
            }
        }

        FUNCTION_EXIT;
        return newPackets;
    }


    void RadioCommsObserver::processRadioMethod( IncomingPacketPtr incomingMethod )
    {
        FUNCTION_ENTRY( "processEvent" );

        IncomingPacket& packet = *incomingMethod;

        // print the full packet
        std::ostringstream dump;
        dump << "Packet Received: ";

        for ( IncomingPacket::iterator dumpIter = packet.begin();
              packet.end() != dumpIter; ++dumpIter )
        {
            // add to printable string
            unsigned int value = (*dumpIter);
            value &= 0xFF;
            dump << "<";
            dump << std::hex << std::setw(2) << std::setfill('0') << std::internal << value;
            dump << ">";
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     dump.str().c_str() );

        // now process the packet (assume it is full)

        // Get message size
        unsigned long messageSize = getMessageLong( packet, 0 );

        TA_ASSERT( messageSize == packet.size(), "Size mismatch when processing incoming data" );

        unsigned long methodType    = getMessageLong( packet, 16 );
        unsigned long transactionId = getMessageLong( packet, 8 );
        unsigned long sessionId     = getMessageLong( packet, 4 );

        // Check for a matching simulator case
        SimulatorUseCaseList ucMatch = SimulatorHelper::getInstance().findUseCases( packet );

        // now iterate over the list and run each (non-continue) use case
        if ( ucMatch.size() > 1 )
        {

            COUT( "Multiple use-cases will be executed for received message (" 
                  << ucMatch.size() << ")..." << std::endl );
        }

        // run each case until a non-continue case is found
        bool completedChain = false;
        
        for ( SimulatorUseCaseList::iterator uc = ucMatch.begin();
              uc != ucMatch.end(); ++uc )
        {
            simulatorRunUseCase( packet, *uc );

            if ( false == (*uc).continueMode )
            {
                completedChain = true;
            }
        }

        // to assist debugging train messages that didn't send a train response, but did send the radio ack
        // this will help identify incorrectly configured train scripts
        // it basically says if a match was found, and there was no endpoint, it could be an error
        if ( false == ucMatch.empty() )
        {
            if ( false == completedChain )
            {
                CERR( "Error: Chain of use cases incomplete, no end point could be found for execution of received data" << std::endl
                      << "Data: " << SimulatorUseCase::Mask( packet ) << std::endl );
            }

            FUNCTION_EXIT;
            return; // we ran at least one, so get out of here
        }


        // no matches could be found.
        // attempt to run the hard coded crap...

        CERR( std::endl << "Simulator use case cannot be found for following message (attempting default behaviour)" << std::endl
              << "Received: [" << SimulatorUseCase::Mask( packet ) << "]" << std::endl );

        switch ( methodType )
        {
/*
            case 2: // Attach session
                attachSession( transactionId );
                break;

            case 5: // Initialise session
                initialiseSession( transactionId, sessionId );
                break;
*/

            case 10: // Query Reference
                {
                    // read the data
                    unsigned long reference = getMessageLong( packet, 20 );
                    unsigned long key = getMessageLong( packet, 24 );

                    // process the data
                    queryReference( transactionId, sessionId, reference, key );
                }
                break;
        
            case 11: // Change Reference
                {
                    // read the data
                    unsigned long reference = getMessageLong( packet, 20 );
                    std::string details = getMessageLongString( packet, 24 );
                
                    // process the data
                    changeReference( transactionId, sessionId, reference, details );
                }
                break;

            case 12: // Search Subscribers
                {
                    // read the data
                    unsigned char view = packet[20];
                    std::string criteria = getMessageLongString( packet, 21 );

                    // process the data
                    searchSubscribers( transactionId, sessionId, view, criteria );                           
                }
                break;

/*
            case 13: // New reference request
                {
                    // read the data
                    char refType = packet[20];
                    
                    // process the data
                    newReference( transactionId, sessionId, refType );
                }
                break;
*/

            case 14: // Text 2 Subscriber reference request
                {
                    // read the data
                    std::string textReference = getMessageShortString( packet, 20 );

                    // process the data
                    text2SubRef( transactionId, sessionId, textReference );
                }
                break;

/*
            case 15: // Delete reference request
                {
                    // read the data
                    char refType[4];
                    refType[0] = packet[20];
                    refType[1] = packet[21];
                    refType[2] = packet[22];
                    refType[3] = packet[23];
                    unsigned long reference = getMessageLong( packet, 24 );

                    // process the data
                    deleteReference( transactionId, sessionId, refType, reference );
                }
                break;
*/

            case 21: // SetupCall request
                {
                    // read the data
                    unsigned long callRef = getMessageLong( packet, 20 );
                    unsigned short callType = getMessageShort( packet, 24 );
                    
                    // process the data
                    setupCall( transactionId, sessionId, callType, callRef );
                }
                break;

            case 23: // Disconnect request
                {
                    // read the data
                    unsigned long callRef = getMessageLong( packet, 20 );

                    // process the data
                    disconnectCall( transactionId, sessionId, callRef );
                }
                break;

            case 24: // SendSDS request
                {
                    // read the data
                    unsigned long sdsRef = getMessageLong( packet, 20 );

                    // process the data
                    sendSDS( transactionId, sessionId, sdsRef );
                }
                break;

            case 35: // GetGroupDetails method
                {
                    // read the data
                    unsigned long reference = getMessageLong( packet, 20 );
                    unsigned long rowsPerEvent = getMessageLong( packet, 24 );
                    std::string fields = getMessageLongString( packet, 28 );

                    // process the data
                    getGroupDetails( transactionId, sessionId, reference, rowsPerEvent, fields );
                }
                break;

            default:  // Message not processed.
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                                 "Unknown method request: %lu, transaction id: %lu session id: %lu",
                                 methodType,
                                 transactionId,
                                 sessionId );

                    CERR( "Warning: Unknown method request" << std::endl << std::endl );
                }
                break;
        }

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::simulatorRunUseCase( const std::vector<unsigned char>& data,
                                                  const SimulatorUseCase& uc )
    {
        FUNCTION_ENTRY( "simulatorRunUseCase" );

        // Take a copy of the data, and stuff it in a copy of the usecase...
        SimulatorUseCase theCase( uc );
        theCase.data = SimulatorUseCase::Mask( data );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                     "Simulator use case '%s' activated.",
                     uc.name.c_str() );
        
        std::ostringstream screenOutput;
        screenOutput << "Simulator use case [" << theCase.name << "] activated." << std::endl;

        if ( theCase.verboseMode >= 1 )
        {
            screenOutput << "Received: [" << theCase.data;
            
            if ( theCase.verboseMode >= 2 )
            {
                screenOutput << theCase.data.renderAsPrintableString();
            }
            screenOutput << "]" << std::endl;
        }
        
        // Now dump any configured loggable fields...
        screenOutput << SimulatorHelper::getInstance().displayLoggable( theCase );

        COUT( std::endl << screenOutput.str() );

        // evaluate the data to send, and send
        SimulatorHelper::getInstance().sendMessage( theCase, *this );

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::writeFailed()
    {
        FUNCTION_ENTRY( "writeFailed" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Socket write failed!" );

        FUNCTION_EXIT;
    }


    bool RadioCommsObserver::connectionLost()
    {
        FUNCTION_ENTRY( "connectionLost" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                     "Connection to client lost." );

        m_stillConnected = false;

        FUNCTION_EXIT;
        return false;
    }


    void RadioCommsObserver::connectionEstablished()
    {
        FUNCTION_ENTRY( "connectionEstablished" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                     "Connection to client established." );
        
        m_stillConnected = true;

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::attachSession( unsigned long transactionId )
    {
        FUNCTION_ENTRY( "attachSession" );

        static unsigned long sessionId = 1;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                     "AttachSession method received, transaction id: %lu, session id: %lu",
                     transactionId,
                     sessionId );

        char event[20];
        
        // Create the event
        setMessageLong( event, sizeof( event ) );
        setMessageLong( &event[4], sessionId++ );
        setMessageLong( &event[8], transactionId );
        setMessageLong( &event[12], 0 );
        setMessageLong( &event[16], 2+0x8000 );

        sendEvent( event, sizeof( event ) );

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::initialiseSession( unsigned long transactionId,
                                                unsigned long sessionId )
    {
        FUNCTION_ENTRY( "initialiseSession" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                     "Initialise method received, transaction id: %lu, session id: %lu",
                     transactionId,
                     sessionId );

        char event[20];
        
        // Create the event
        setMessageLong( event, sizeof( event ) );
        setMessageLong( &event[4], sessionId );
        setMessageLong( &event[8], transactionId );
        setMessageLong( &event[12], 0 );
        setMessageLong( &event[16], 5+0x8000 );

        sendEvent( event, sizeof( event ) );

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::newReference( unsigned long transactionId,
                                           unsigned long sessionId,
                                           char refType )
    {
        FUNCTION_ENTRY( "newReference" );

        static unsigned long callCounter = 0xC000;
        unsigned long callReference = callCounter++;

        LOG_GENERIC( SourceInfo,  TA_Base_Core::DebugUtil::DebugDebug, 
                     "NewReference method received, transaction id: %lu, session id: %lu, type: %c, reference %lu",
                     transactionId,
                     sessionId,
                     refType,
                     callReference );

        char event[28];
        
        // Create the reply event
        setMessageLong( event, sizeof( event ) );
        setMessageLong( &event[4], sessionId );
        setMessageLong( &event[8], transactionId );
        setMessageLong( &event[12], 0);
        setMessageLong( &event[16], 13+0x8000 );
        event[20] = refType;
        event[21] = 0;
        event[22] = 0;
        event[23] = 0;
        setMessageLong( &event[24], callReference );

        sendEvent( event, sizeof( event ) );

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::setupCall( unsigned long transactionId,
                                        unsigned long sessionId,
                                        unsigned short refType,
                                        unsigned long callRef )
    {
        FUNCTION_ENTRY( "setupCall" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                     "SetupCall method received, sending SetupCall event, transaction id: %lu, session id: %lu, type: %hu, reference: %lu",
                      transactionId,
                      sessionId,
                      refType,
                      callRef );
        
        char event[26];
        
        // Create the reply event
        setMessageLong( event, sizeof( event ) );
        setMessageLong( &event[4], sessionId );
        setMessageLong( &event[8], transactionId );
        setMessageLong( &event[12], 0 );
        setMessageLong( &event[16], 21+0x8000 );
        setMessageLong( &event[20], callRef );
        setMessageShort( &event[24], refType );

        sendEvent( event, sizeof( event ) );

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::disconnectCall( unsigned long transactionId,
                                             unsigned long sessionId,
                                             unsigned long callRef )
    {
        FUNCTION_ENTRY( "disconnectCall" );

        char message[32] = "DisconnectByCaller";

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                     "Disconnect method received, transaction id: %lu, session id: %lu, type: %hu, reference: %lu, reason: %s",
                      transactionId,
                      sessionId,
                      callRef,
                      message );

        char event[56];
        
        // Create the reply event
        setMessageLong( event, sizeof( event ) );
        setMessageLong( &event[4], sessionId );
        setMessageLong( &event[8], transactionId );
        setMessageLong( &event[12], 0 );
        setMessageLong( &event[16], 23+0x8000 );
        setMessageLong( &event[20], callRef );
        strcpy( &event[24], message );

        sendEvent( event, sizeof( event ) );

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::sendSDS( unsigned long transactionId,
                                      unsigned long sessionId,
                                      unsigned long sdsRef )
    {
        FUNCTION_ENTRY( "sendSDS" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                     "SendSDS method received, transaction id: %lu, session id: %lu, reference: %lu",
                     transactionId,
                     sessionId,
                     sdsRef );

        char event[24];
        
        // Create the reply event
        setMessageLong( event, sizeof( event ) );
        setMessageLong( &event[4], sessionId );
        setMessageLong( &event[8], transactionId );
        setMessageLong( &event[12], 0 );
        setMessageLong( &event[16], 24+0x8000 );
        setMessageLong( &event[20], sdsRef );

        sendEvent( event, sizeof( event ) );

        FUNCTION_EXIT;
    }

    void RadioCommsObserver::queryReference( unsigned long transactionId,
                                             unsigned long sessionId,
                                             unsigned long reference,
                                             unsigned long key )
    {
        FUNCTION_ENTRY( "queryReference" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "QueryReference method received, transaction id: %lu, session id: %lu, reference: %lu, key %lu",
                     transactionId,
                     sessionId,
                     reference,
                     key );

        char event[1000]; // Make it longer than the message will be, and chop it before sending it.

        // Create the reply event
        setMessageLong( event, sizeof( event ) ); // will resize later...
        setMessageLong( &event[4], sessionId );
        setMessageLong( &event[8], transactionId );
        setMessageLong( &event[12], 0 );
        setMessageLong( &event[16], 10+0x8000 );
        setMessageLong( &event[20], reference );
        setMessageLong( &event[24], key );

        unsigned long stringLength = 0;

        if ( key != 10000 || key != 10001 )
        {
            // This is not an "all"
            switch ( key )
            {

            case 18: // K_TSI
                {
                    setMessageLong( &event[28], 12 );  // 11 characters, plus the new line.
                    
                    std::string tsi = "123-456-789";
                    
                    unsigned int i = 0;
                    for ( ; i < tsi.size(); ++i )
                    {
                        event[32+i] = tsi.at(i);
                    }

                    event[i + 32 + 1] = '\n';
                    
                    stringLength = 12;
                }
                break;

            case 34: // K_SUPPLEMENTARY_SERVICES
                {
                    setMessageLong( &event[28], 5 );
                    
                    std::string services("" );
                    
                    if ( ( m_notReachableForwardNumber != "" ) ||
                         ( m_unconditionalForwardNumber != "" ) )
                    {
                         services = "+Y+W";
                    }
                    else
                    {
                        services = "+Y";
                    }
                    
                    unsigned int i = 0;
                    for ( ; i < services.size(); ++i )
                    {
                        event[32+i] = services.at(i);
                    }
                    
                    event[i+32+1] = '\n';
                    stringLength = 5;
                }
                break;

            default:
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                                 "Unknown key in query: %lu",
                                 key );
                    return;
                }
            }
        }
        else
        {
            // Not used for now, so don't simulate
            return;
        }

        setMessageLong( &event[0], 28 + stringLength + 4 );
        sendEvent( event, 28 + stringLength + 4 );

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::searchSubscribers( unsigned long transactionId,
                                                unsigned long sessionId,
                                                unsigned char view,
                                                const std::string& criteria )
    {
        FUNCTION_ENTRY( "searchSubscribers" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "SearchSubscribers method received, transaction id: %lu, session id: %lu, view: %lu, criteria: %s",
                     transactionId,
                     sessionId,
                     view,
                     criteria.c_str() );

        char event[24]; // Make it longer than it will ever be, then chop it before sending.

        // Create the reply event
        setMessageLong( event, sizeof( event ) ); // will resize later...
        setMessageLong( &event[4], sessionId );
        setMessageLong( &event[8], transactionId );
        setMessageLong( &event[12], 0 );
        setMessageLong( &event[16], 12+0x8000 );

        // Set an ID. This ID represents a "result-set". Need to base it on the criterea.
        unsigned long flags = 0x0000;
        if ( std::string::npos != criteria.find_first_of("123-456-789") )
        {
            flags |= 0x0001;
        }
        
        // Add others here...

        setMessageLong( &event[20], flags );

        sendEvent( event, 24 );

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::getGroupDetails( unsigned long transactionId,
                                              unsigned long sessionId,
                                              unsigned long reference,
                                              unsigned long rowsPerEvent,
                                              const std::string& fields )
    {
        FUNCTION_ENTRY( "getGroupDetails" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                     "GetGroupDetails method received, transaction id: %lu, session id: %lu, reference: %lu, RowsPerEvent: %lu, fields: %s",
                     transactionId,
                     sessionId,
                     reference,
                     rowsPerEvent,
                     fields.c_str() );

        char event[1000];   // make it long now, and chop it down later...

        // Create the reply event
        setMessageLong( event, sizeof( event ) );    // will resize later...
        setMessageLong( &event[4], sessionId );
        setMessageLong( &event[8], transactionId );
        setMessageLong( &event[12], 0 );           // Status OK
        setMessageLong( &event[16], 35+0x8000 );   // Event number
        setMessageLong( &event[20], reference );

        // Now for the string - the string is set to a table, and depends on the reference, and the fields, 
        // so need to define a number of items that might be inserted into the string.
        unsigned char separator( 0x01 );

        // And parse the fields into a vector
        std::vector<std::string> fieldsVector;
        std::string::const_iterator stringIter = fields.begin();

        for( ; stringIter != fields.end(); ++stringIter )
        {
            std::string fieldValue;
            while( ( *stringIter != ',' ) &&
                   ( stringIter != fields.end() ) )
            {
                fieldValue += *stringIter;
                stringIter++;
            }

            fieldsVector.push_back( fieldValue );

            if ( stringIter == fields.end() )
            {
                break;
            }
        }

        std::string resultString("" );
    
        if ( reference & 0x0001 )
        {
            // Add the default field, which is the text reference
            resultString += "SubRef1";
            resultString += separator;

            // Set up a vector of the requested fields
            std::vector<std::string>::const_iterator vectorIter = fieldsVector.begin();

            while ( vectorIter != fieldsVector.end() )
            {
                // Iterate through the vector, completing the fields as necessary
                if ( 0 == (*vectorIter).compare("K_FORWARD_UNCONDITIONAL") )
                {
                    resultString += m_unconditionalForwardNumber;
                }
                else if ( 0 == (*vectorIter).compare("K_FORWARD_NOT_REACHABLE") )
                {
                    resultString += m_notReachableForwardNumber;
                }

                vectorIter++;
                if ( vectorIter != fieldsVector.end() )
                {
                    resultString += separator;
                }
            }

            resultString += "\r\n";
        }

        setMessageLong( &event[24], resultString.size() );
    
        unsigned int i = 0;
        for ( ; i < resultString.size(); ++i)
        {
            event[28+i] = resultString.at(i);
        }

        setMessageLong( &event[0], 28 + resultString.size() );
        sendEvent( event, 28 + resultString.size() );

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::deleteReference( unsigned long transactionId,
                                              unsigned long sessionId,
                                              const char refType[],
                                              unsigned long reference )
    {
        FUNCTION_ENTRY( "deleteReference" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                     "DeleteReference method received, transaction id: %lu, session id: %lu, type: %c%c%c%c, reference %lu",
                      transactionId,
                      sessionId,
                      refType[0],
                      refType[1],
                      refType[2],
                      refType[3],
                      reference );

        char event[28];

        // Create the reply event
        setMessageLong( event, sizeof( event ) );   // will resize later...
        setMessageLong( &event[4], sessionId );
        setMessageLong( &event[8], transactionId );
        setMessageLong( &event[12], 0 );            // Status OK
        setMessageLong( &event[16], 15+0x8000 );    // Event number

        memcpy( &event[20], &refType, 4 );          // Copy the ref-type into the event array
        setMessageLong( &event[24], reference );    // Set reference
    
        sendEvent( event, sizeof( event ) );
        FUNCTION_EXIT;
    }


    void RadioCommsObserver::text2SubRef( unsigned long transactionId,
                                          unsigned long sessionId,
                                          std::string textReference )
    {
        FUNCTION_ENTRY( "text2SubRef" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                     "Text2SubscriberReference method received, transaction id: %lu, session id: %lu, text reference: %s",
                     transactionId,
                     sessionId,
                     textReference.c_str() );

        char event[56];

        // Create the reply event
        setMessageLong( event, sizeof( event ) );    // will resize later...
        setMessageLong( &event[4], sessionId );
        setMessageLong( &event[8], transactionId );
        setMessageLong( &event[12], 0 );           // Status OK
        setMessageLong( &event[16], 14+0x8000 );   // Event number

        strcpy( &event[20], textReference.c_str() );          // Copy the reference string
        setMessageLong( &event[52], 1 );   // Set reference
    
        sendEvent( event, sizeof( event ) );

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::changeReference( unsigned long transactionId,
                                              unsigned long sessionId,
                                              unsigned long reference,
                                              const std::string& details )
    {
        FUNCTION_ENTRY( "changeReference" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "ChangeReference method received, transaction id: %lu, session id: %lu, reference: %lu, key %s",
                     transactionId,
                     sessionId,
                     reference,
                     details.c_str() );

        char event[28];

        // Process the details into a map.
        std::map<std::string,std::string> detailsMap;

        std::string::const_iterator iter = details.begin();

        while ( iter != details.end() )
        {
            std::string key("" );
            std::string value("" );

            while ( *iter != '=' )
            {
                // Everything until an '=' is a key
                key += *iter;
                iter++;
            }

            // Don't want the equals, so move
            iter++;

            while ( *iter != '\n' )
            {
                // Everything until a new line is a value
                value += *iter;
                iter++;
            }
            
            // Don't want the new line, so move on
            iter++;

            // And add the value and key to the map
            detailsMap.insert( std::map<std::string,std::string>::value_type( key, value ) );
        }

        // Set the variables
        std::map<std::string,std::string>::iterator unconditionalForwardIter = detailsMap.find("K_FORWARD_UNCONDITIONAL" );
        std::map<std::string,std::string>::iterator unreachableForwardIter = detailsMap.find("K_FORWARD_NOT_REACHABLE" );
    
        // If unconditional forwarding is found, then set variabls appropriately.
        if ( unconditionalForwardIter != detailsMap.end() )
        {
            std::string value = unconditionalForwardIter->second;
            
            // This will actually be the value twice with a comma, so strip it at the comma
            m_unconditionalForwardNumber = value.substr( 0, value.find_first_of( ",", 0 ) );
            m_notReachableForwardNumber = "";
        }
        else if ( unreachableForwardIter != detailsMap.end() )
        {
            std::string value = unreachableForwardIter->second;
            
            // This will actually be the value twice with a comma, so strip it at the comma
            m_unconditionalForwardNumber = "";
            m_notReachableForwardNumber = value.substr( 0, value.find_first_of( ",", 0 ) );
        }
        else
        {
            // Nothing found
            m_unconditionalForwardNumber = "";
            m_notReachableForwardNumber = "";
        }

    

        // Create the reply event
        setMessageLong( event, sizeof( event ) );    // will resize later...
        setMessageLong( &event[4], sessionId );
        setMessageLong( &event[8], transactionId );
        setMessageLong( &event[12], 0 );           // Status OK
        setMessageLong( &event[16], 11+0x8000 );   // Event number

        setMessageLong( &event[20], reference );          // Add the reference back
        setMessageLong( &event[24], 0 );   // Set the length of the result (error) string to 0.
    
        sendEvent( event, sizeof( event ) );

        FUNCTION_EXIT;
    }


    SimulatorUseCaseList& RadioCommsObserver::simulatorUseCasesRef()
    {
        FUNCTION_ENTRY( "simulatorUseCasesRef" );
        FUNCTION_EXIT;
        return m_simulatorUseCases;
    }


    unsigned long RadioCommsObserver::getMessageLong( const std::vector< unsigned char >& buffer,
                                                      unsigned int iPosition ) const
    {
        FUNCTION_ENTRY( "getMessageLong" );
        
        unsigned long value;

        value = getMessageShort( buffer, iPosition + 2 );
        value = value * 0x00010000;
        value = value + getMessageShort( buffer, iPosition );

        FUNCTION_EXIT;
        return value;
    }


    unsigned short RadioCommsObserver::getMessageShort( const std::vector< unsigned char >& buffer,
                                                        unsigned int iPosition ) const
    {
        FUNCTION_ENTRY( "getMessageShort" );

        // Check position not beyond buffer
        TA_ASSERT( iPosition + 1 < buffer.size(), "Attempt to read beyond message buffer." );

        unsigned short  value;

        value = buffer[iPosition+1];
        value = value * 0x0100;
        value = value + buffer[iPosition];

        FUNCTION_EXIT;
        return value;
    }


    std::string RadioCommsObserver::getMessageLongString( const std::vector< unsigned char >& buffer,
                                                          unsigned int iPosition ) const
    {
        FUNCTION_ENTRY( "getMessageLongString" );

        // Check position not beyond buffer
        TA_ASSERT( iPosition + 1 < buffer.size(), "Attempt to read beyond message buffer." );

        unsigned long stringLength = getMessageLong( buffer, iPosition );

        // Add 4 for the length of the long, and add another 1 as position is 0-based.
        TA_ASSERT( iPosition + stringLength + 4 <= buffer.size(), "String boundary extends beyond message buffer" );

        std::string value;
        for ( unsigned int i = 0; i < stringLength; ++i )
        {
            // Add 4 for the length of the long stating the string length
            value += buffer[iPosition + 4 + i];
        }

        FUNCTION_EXIT;
        return value;
    }


    std::string RadioCommsObserver::getMessageShortString( const std::vector< unsigned char >& buffer,
                                                           unsigned int iPosition ) const
    {
        FUNCTION_ENTRY( "getMessageShortString" );

        // Check position not beyond buffer
        TA_ASSERT( iPosition + 32 <= buffer.size(), "Attempt to read beyond message buffer." );

        std::string value;

        for ( unsigned int i = 0; i < 32; ++i )
        {
            // Add 4 for the length of the long stating the string length
            value += buffer[iPosition + i];
        }

        FUNCTION_EXIT;
        return value;
    }


    void RadioCommsObserver::setMessageLong( char * buffer,
                                             unsigned long value ) const
    {
        FUNCTION_ENTRY( "setMessageLong" );

        setMessageShort( buffer, (unsigned short)( value%0x00010000 ) );
        
        setMessageShort( buffer+2, (unsigned short)( value/0x00010000 ) );
        
        FUNCTION_EXIT;
    }


    void RadioCommsObserver::setMessageShort( char * buffer,
                                              unsigned short value ) const
    {
        FUNCTION_ENTRY( "setMessageShort" );
        
        buffer[0] = value%0x0100;
        buffer[1] = value/0x0100;

        FUNCTION_EXIT;
    }


    void RadioCommsObserver::sendEvent( const char * event,
                                        unsigned long eventLength )
    {
        FUNCTION_ENTRY( "sendEvent" );

        std::ostringstream dump;
        dump << "Packet Sent: ";

        for ( unsigned int index = 0; index < eventLength; ++index )
        {
            unsigned int value = event[index];
            value &= 0xFF;
            dump << "<";
            dump << std::hex << std::setw(2) << std::setfill('0') << std::internal << value;
            dump << ">";
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     dump.str().c_str() );

        // Send it
        m_clientSocket->write(&event[0],eventLength);

        FUNCTION_EXIT;
    }

    void RadioCommsObserver::simulatorSendEvent( const char * buf,
                                                 const unsigned int length )
    {
        FUNCTION_ENTRY( "simulatorSendEvent" );

        FUNCTION_EXIT;
        sendEvent( buf, length );
    }


    void RadioCommsObserver::addSessionReference( unsigned long ref )
    {
        FUNCTION_ENTRY( "addSessionReference" );

        if ( m_sessionReferences.find(ref) == m_sessionReferences.end() )
        {
            m_sessionReferences[ref] = "default";
        }

        FUNCTION_EXIT;
    }

    void RadioCommsObserver::deleteSessionReference( unsigned long ref )
    {
        FUNCTION_ENTRY( "deleteSessionReference" );

        std::map<unsigned long,std::string>::iterator it = m_sessionReferences.find( ref );

        if ( it != m_sessionReferences.end() )
        {
            m_sessionReferences.erase(it);
        }

        FUNCTION_EXIT;
    }

    void RadioCommsObserver::updateSessionReference( unsigned long ref, const std::string& status )
    {
        FUNCTION_ENTRY( "updateSessionReference" );

        if ( m_sessionReferences.find(ref) != m_sessionReferences.end() )
        {
            m_sessionReferences[ref] = status;
        }

        FUNCTION_EXIT;
    }


    RadioMethodTask::RadioMethodTask( IRadioMethodProcessor& callback,
                                      IncomingPacketPtr data )
      : m_callback( callback ),
        m_data( data )
    {
    }

    
    RadioMethodTask::~RadioMethodTask()
    {
    }


    void RadioMethodTask::executeWorkItem()
    {
        m_callback.processRadioMethod( m_data );
    }


} // TA_IRS_Bus
