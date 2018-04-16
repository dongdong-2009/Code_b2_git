
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/RadioSimulatorLibrary/src/RadioSimulator.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  *
  * RadioSimulator implements ISimulationModule, for use with the generic simulator.
  */

#pragma warning (disable : 4786)

#include "bus/radio/RadioSimulatorLibrary/src/RadioSimulator.h"
#include "bus/radio/RadioSimulatorLibrary/src/RadioCommsObserver.h"
#include "bus/radio/RadioSimulatorLibrary/src/RadioSimulatorCommandProcessor.h"
#include "bus/radio/RadioSimulatorLibrary/src/SimulatorUseCase.h"
#include "bus/radio/RadioSimulatorLibrary/src/SimulatorHelper.h"
#include "bus/radio/RadioSimulatorLibrary/src/SimulatorParser.h"

#include "bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h"

#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/ThreasdafeScreenOutput.h"

#include <algorithm>
#include <utility>
#include <functional>


namespace TA_IRS_Bus
{

    RadioSimulator::RadioSimulator( const std::string &name,
                                    const char * host,
                                    const char * port )
          : m_name( name ),
            m_socketPort( port ),
            m_socketHost( host ),
            m_commandProcessor( NULL ),
            m_threadIsRunning( false ),
            m_doThreadTerminate( false )
    {
        FUNCTION_ENTRY( "RadioSimulator" );

        m_commandProcessor = new RadioSimulatorCommandProcessor( *this );
        
        FUNCTION_EXIT;
    }


    RadioSimulator::~RadioSimulator()
    {
        FUNCTION_ENTRY( "~RadioSimulator" );
        
        stopSim();

        FUNCTION_EXIT;
    }


    void RadioSimulator::run()
    {
        FUNCTION_ENTRY( "run" );

        m_threadIsRunning = true;

        COUT( "Radio Simulator listener thread has started" << std::endl );

        typedef TA_Base_Core::TcpServerSocket< TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > > RadioSimulatorServerSocket;

        // Create the listening socket
        RadioSimulatorServerSocket* serverSocket = new RadioSimulatorServerSocket( m_socketHost, m_socketPort );
        serverSocket->bind();
        serverSocket->listen();

        while ( false == m_doThreadTerminate )
        {
            try
            {
                // See if we have any connections waiting to be accepted.
                
                TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket >* acceptSocket = serverSocket->accept( TA_Base_Core::SERVER_NON_BLOCKING_ACCEPT );

                if ( 0 != acceptSocket )
                {
                    RadioCommsObserver* com = new RadioCommsObserver( acceptSocket );
                    
                    addCommsObserver( com );
                    
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "New Connection accepted on socket." );
                }

                // we should check to see if all of our Observed Sockets are still connected.
                removeDisconnectedCommsObserver();
            }
            catch ( ... )
            {
                m_threadIsRunning = false;

                TA_ASSERT( false, "Exception in listening socket" );
                throw;
            }

            sleep( 500 );
        }

        delete serverSocket;
        serverSocket = NULL;

        COUT( "Radio Simulator listener thread has exited" << std::endl );
        
        m_threadIsRunning = false;
        m_doThreadTerminate = false;

        FUNCTION_EXIT;
    }


    void RadioSimulator::terminate()
    {
        FUNCTION_ENTRY( "terminate" );

        m_doThreadTerminate = true;

        FUNCTION_EXIT;
    }


    void RadioSimulator::startSim()
    {
        FUNCTION_ENTRY( "startSim" );

        if ( true == m_threadIsRunning )
        {
            COUT( "Radio Simulator already running." << std::endl );

            FUNCTION_EXIT;
            return;
        }

        // start the listener thread.
        m_doThreadTerminate = false;

        start();

        // Wait for it to get up to speed!
        while ( false == m_threadIsRunning )
        {
            sleep( 100 );
        }

        FUNCTION_EXIT;
    }


    void RadioSimulator::stopSim()
    {
        FUNCTION_ENTRY( "stopSim" );

        terminateAndWait();
        removeAllCommsObservers();

        FUNCTION_EXIT;
    }

    //
    // restartSim()
    //
    void RadioSimulator::restartSim()
    {
        FUNCTION_ENTRY( "restartSim" );
        
        stopSim();
        startSim();

        FUNCTION_EXIT;
    }

    //
    // destroySim()
    //
    void RadioSimulator::destroySim()
    {
        FUNCTION_ENTRY( "destroySim" );
        FUNCTION_EXIT;
    }

    //
    // getName()
    //
    std::string RadioSimulator::getName()
    {
        FUNCTION_ENTRY( "getName" );
        FUNCTION_EXIT;
        return m_name;
    }

    //
    // getSimDescription()
    //
    std::string RadioSimulator::getSimDescription()
    {
        FUNCTION_ENTRY( "getSimDescription" );
        FUNCTION_EXIT;
        return std::string( "Radio simulator" );
    }

    //
    // getCommandProcessor()
    //
    TA_Base_Bus::ISimulationCommandProcessor& RadioSimulator::getCommandProcessor()
    {
        FUNCTION_ENTRY( "getCommandProcessor" );
        FUNCTION_EXIT;
        return *m_commandProcessor;
    }


    bool RadioSimulator::execStatus()
    {
        FUNCTION_ENTRY( "execStatus" );

        int numConnections = m_commsObserverVector.size();
        
        COUT( std::endl << "Radio Simulator Status" << std::endl
              << "-------------------------" << std::endl
              << "Number of Connections:  "
              << numConnections << std::endl << std::endl );

        FUNCTION_EXIT;
        return true;
    }


    bool RadioSimulator::newCall( std::string parameters )
    {
        FUNCTION_ENTRY( "newCall" );

        unsigned long IncomingCallLength = 90;
        unsigned long IncomingCallEvent  = 0xA001;
        unsigned long StatusOK           = 0;

        char buffer[100];
        char sessionId[30];
        char callId[30];
        char callType[30];
        char callNumber[100];
        char calledNumber[100];

        // Get passed parameters
        int numTokens = sscanf( parameters.c_str(),
                                "%s %s %s %s %s",
                                sessionId,
                                callId,
                                callType,
                                callNumber,
                                calledNumber );

        if ( numTokens == 5 )
        {
            // Create new call message
            setMessageHeader( buffer, IncomingCallLength, atol( sessionId ), 0, StatusOK, IncomingCallEvent );
            setMessage( &buffer[20], ( unsigned long )atol( callId ) );
            setMessage( &buffer[24], ( short )atol( callType ) );
            setMessage( &buffer[26], callNumber );
            setMessage( &buffer[58], calledNumber );

            // Send out to session observers
            sendFromAllCommsObserver( buffer, IncomingCallLength );
        }
        else
        {
            // Insufficient parameters
            COUT( "Invalid parameters for newcall command." << std::endl );

            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return true;
    }


    bool RadioSimulator::sendSDSData( std::string parameters )
    {
        FUNCTION_ENTRY( "sendSDSData" );

        const unsigned long IncomingSDSLength = 92 + 189;
        const unsigned long IncomingSDSEvent  = 0xA002;
        const unsigned long StatusOK          = 0;

        char buffer[IncomingSDSLength];
        char sessionId[30];
        char sourceNumber[100];
        char messageText[500];

        // Get passed parameters
        int numTokens = sscanf( parameters.c_str(),
                                "%s %s %s",
                                sessionId,
                                sourceNumber,
                                messageText );

        if ( numTokens == 3 )
        {
            std::vector<unsigned char> messageData;
            char* messageChar = messageText;
            char tempText[4] = "";

            while ( *messageChar != '\0' )
            {
                if ( *messageChar == ',' )
                {
                    // Add to array
                    if ( strlen( tempText ) > 0 )
                    {
                        int value = atoi( tempText );

                        if ( ( value < 0 ) || ( value > 255 ) )
                        {
                            CERR( "Invalid message data, values must be between 0 and 255." << std::endl );

                            FUNCTION_EXIT;
                            return false;
                        }

                        messageData.push_back( value );

                        tempText[0] = '\0';
                    }
                }
                else if ( strlen( tempText ) < 3 )
                {
                    tempText[strlen( tempText )+1] = '\0';
                    tempText[strlen( tempText )] = *messageChar;
                }
                else
                {
                    // Invalid data
                    CERR( "Invalid message data, values must be between 0 and 255." << std::endl );

                    FUNCTION_EXIT;
                    return false;
                }

                // Next character
                messageChar++;
            }

            if ( strlen( tempText ) > 0 )
            {
                unsigned char value = atoi( tempText );
                messageData.push_back( value );
                tempText[0] = '\0';
            }

            // Format the message data

            // Create new call message
            setMessageHeader( buffer, IncomingSDSLength, atol( sessionId ), 0, StatusOK, IncomingSDSEvent );
            setMessage( &buffer[20], ( unsigned long ) 1234 );  // SdsReference
            setMessage( &buffer[24], sourceNumber );
            setMessage( &buffer[56], "123-456-789" );  // targetITSI
            setMessage( &buffer[88], ( unsigned long ) 0 ); // DataType - SDS_TEXT_MODE
            setMessage( &buffer[92], ( unsigned long ) messageData.size() ); // Data length

            for ( unsigned int index = 96;
                  index < sizeof( buffer ); ++index )
            {
                if ( messageData.size() > 0 )
                {
                    // Get character from vector
                    buffer[index] = messageData.front();
                    messageData.erase( messageData.begin() );
                }
                else
                {
                    buffer[index] = 0;
                }
            }

            // Send out to session observers
            sendFromAllCommsObserver( buffer, sizeof( buffer ) );
        }
        else
        {
            // Insufficient parameters
            COUT( "Invalid parameters for newcall command." << std::endl );

            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return true;
    }


    bool RadioSimulator::calcChecksumForSDSData( std::string parameters )
    {
        FUNCTION_ENTRY( "calcChecksumForSDSData" );

        const unsigned long IncomingSDSLength = 92 + 189;
        const unsigned long IncomingSDSEvent  = 0xA002;
        const unsigned long StatusOK          = 0;

        char sessionId[30];
        char sourceNumber[100];
        char messageText[500];

        // Get passed parameters
        int numTokens = sscanf( parameters.c_str(),
                                "%s %s %s",
                                sessionId,
                                sourceNumber,
                                messageText );

        if ( numTokens == 3 )
        {
            std::vector<unsigned char> messageData;
            char* messageChar = messageText;
            char  tempText[4] = "";

            while ( *messageChar != '\0' )
            {
                if ( *messageChar == ',' )
                {
                    // Add to array
                    if ( strlen( tempText ) > 0 )
                    {
                        int value = atoi( tempText );

                        if ( ( value < 0 ) || ( value > 255 ) )
                        {
                            CERR( "Invalid message data, values must be between 0 and 255." << std::endl );

                            FUNCTION_EXIT;
                            return false;
                        }

                        messageData.push_back( value );

                        tempText[0] = '\0';
                    }
                }
                else if ( strlen( tempText ) < 3 )
                {
                    tempText[strlen( tempText )+1] = '\0';
                    tempText[strlen( tempText )] = *messageChar;
                }
                else
                {
                    // Invalid data
                    CERR( "Invalid message data, values must be between 0 and 255." << std::endl );
                    
                    FUNCTION_EXIT;
                    return false;
                }

                // Next character
                messageChar++;
            }

            if ( strlen( tempText ) > 0 )
            {
                unsigned char value = atoi( tempText );
                messageData.push_back( value );
                tempText[0] = '\0';
            }

            // fudge the checksum
            unsigned short calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate( messageData, messageData.size() );

            // low byte first
            COUT( "> " << messageText << ","
                  << ( 0xff & calcCRC ) << ","
                  << ( ( 0xff00 & calcCRC ) >> 8 )
                  << std::endl );
        }
        else
        {
            // Insufficient parameters
            CERR( "Invalid parameters for newcall command." << std::endl );

            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return true;
    }


    bool RadioSimulator::simulatorLoadTable( std::string parameters )
    {
        FUNCTION_ENTRY( "simulatorLoadTable" );

        if ( parameters.empty() )
        {
            CERR( "Error: no filename provided" << std::endl );

            FUNCTION_EXIT;
            return false;
        }

        // parse parameters
        std::stringstream s;
        s << parameters << std::ends;
        std::string filename;
        s >> filename;

        try
        {
            SimulatorHelper::getInstance().loadTable( filename );
        }
        catch ( ... )
        {
            CERR( "Error: could not load [" << filename << "]" << std::endl );
        }

        FUNCTION_EXIT;
        return true;
    }


    bool RadioSimulator::simulatorClearTable( std::string parameters )
    {
        FUNCTION_ENTRY( "simulatorClearTable" );

        SimulatorHelper::getInstance().clearTable();

        FUNCTION_EXIT;
        return true;
    }


    bool RadioSimulator::simulatorShowTable( std::string parameters )
    {
        FUNCTION_ENTRY( "simulatorShowTable" );
    
        if ( true == parameters.empty() )
        {
            SimulatorHelper::getInstance().showTable();
        }
        else
        {
            // parse parameters
            std::stringstream s;
            s << parameters << std::ends;
            unsigned int which;
            s >> which;

            SimulatorHelper::getInstance().showEntry( which );
        }

        FUNCTION_EXIT;
        return true;
    }

    bool RadioSimulator::simulatorRun( std::string parameters )
    {
        FUNCTION_ENTRY( "simulatorRun" );

        if ( true == parameters.empty() )
        {
            CERR( "Error: no entry specified" << std::endl );

            FUNCTION_EXIT;
            return false;
        }

        // parse parameters
        std::stringstream s;
        s << parameters << std::ends;
        unsigned int which;
        s >> which;

        if ( true == m_commsObserverVector.empty() )
        {
            CERR( "Error: no observers..." << std::endl );

            FUNCTION_EXIT;
            return false;
        }

        if ( which >= SimulatorHelper::getInstance().getUseCaseListRef().size() )
        {
            CERR( "Error: no entry [" << which << "]" << std::endl );

            FUNCTION_EXIT;
            return false;
        }

        simulatorRunUseCase( SimulatorHelper::getInstance().getUseCaseListRef()[which] );

        FUNCTION_EXIT;
        return true;
    }


    void RadioSimulator::simulatorRunUseCase( const SimulatorUseCase& uc )
    {
        FUNCTION_ENTRY( "simulatorRunUseCase" );

        if ( true == uc.resp.empty() )
        {
            CERR( "Debug: No response defined for [" << uc.name << "]" << std::endl
                  << "Error: Cannot run [" << uc.name
                  << "] it is NOT a valid rule to run from this command, check you are executing the correct rule...."
                  << std::endl );
            
            FUNCTION_EXIT;
            return;
        }

        if ( false == uc.mask.empty() )
        {
            CERR( "Debug: An input mask has been defined for [" << uc.name
                  << "], cannot generate response." << std::endl
                  << "Error: Cannot run [" << uc.name
                  << "] it is NOT a valid rule to run from this command, check you are executing the correct rule...."
                  << std::endl );

            FUNCTION_EXIT;
            return;
        }

        SimulatorHelper::getInstance().sendMessage( uc, *this );

        FUNCTION_EXIT;
    }


    std::vector< RadioCommsObserver* >& RadioSimulator::getCommsObserverVector()
    {
        FUNCTION_ENTRY( "getCommsObserverVector" );
        FUNCTION_EXIT;
        return m_commsObserverVector;
    }


    void RadioSimulator::addCommsObserver( RadioCommsObserver* com )
    {
        FUNCTION_ENTRY( "addCommsObserver" );
        
        m_commsObserverVector.push_back( com );

        FUNCTION_EXIT;
    }


    void RadioSimulator::removeDisconnectedCommsObserver()
    {
        FUNCTION_ENTRY( "removeDisconnectedCommsObserver" );

        std::vector< RadioCommsObserver * >::iterator it;
        bool someDeleted;

        do
        {
            someDeleted = false;

            for ( it = m_commsObserverVector.begin(); it != m_commsObserverVector.end();it++ )
            {
                if ( !( *it )->stillConnected() )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                 "CommsObserver removed from vector" );
                    
                    RadioCommsObserver* com = ( *it );
                    
                    m_commsObserverVector.erase( it );
                    
                    delete com;
                    someDeleted = true;
                    
                    break;
                }
            }
        }
        while ( someDeleted );

        FUNCTION_EXIT;
    }


    void RadioSimulator::removeAllCommsObservers()
    {
        FUNCTION_ENTRY( "removeAllCommsObservers" );

        for ( std::vector< RadioCommsObserver * >::iterator it = m_commsObserverVector.begin();
              m_commsObserverVector.end() !=  it; ++it )
        {
            delete (*it);
            (*it) = NULL;
        }

        m_commsObserverVector.clear();

        FUNCTION_EXIT;
    }


    void RadioSimulator::sendFromAllCommsObserver( const char * pBuffer,
                                                   const unsigned int dataLength )
    {
        FUNCTION_ENTRY( "sendFromAllCommsObserver" );

        for ( std::vector< RadioCommsObserver * >::iterator it = m_commsObserverVector.begin();
              m_commsObserverVector.end() !=  it; ++it )
        {
            TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket>* observedSocket = ( *it )->getSocket();

            observedSocket->write( pBuffer, dataLength );
        }

        FUNCTION_EXIT;
    }


    void RadioSimulator::simulatorSendEvent( const char * buf,
                                             const unsigned int length )
    {
        FUNCTION_ENTRY( "simulatorSendEvent" );

        sendFromAllCommsObserver( buf, length );

        FUNCTION_EXIT;
    }


    void RadioSimulator::setMessageHeader( char* buffer,
                                           unsigned long  messageLength,
                                           unsigned long  sessionId,
                                           unsigned long  transactionId,
                                           unsigned long  status,
                                           unsigned long  event )
    {
        FUNCTION_ENTRY( "setMessageHeader" );

        // Zero fill the buffer
        memset( buffer, 0, messageLength );

        setMessage( buffer, messageLength );
        setMessage( &buffer[4], sessionId );
        setMessage( &buffer[8], transactionId );
        setMessage( &buffer[12], status );
        setMessage( &buffer[16], event );

        FUNCTION_EXIT;
    }

    void RadioSimulator::setMessage( char* buffer,
                                     unsigned long data )
    {
        FUNCTION_ENTRY( "setMessage" );
        
        // Convert into buffer in little endian format.
        // Can't use bit shift as this is platform specific.
        
        buffer[3] = ( char )( data / 0x01000000 );
        
        data = data % 0x01000000;
        
        buffer[2] = ( char )( data / 0x00010000 );
        
        data = data % 0x00010000;
        
        buffer[1] = ( char )( data / 0x00000100 );
        buffer[0] = ( char )( data % 0x00000100 );

        FUNCTION_EXIT;
    }

    void RadioSimulator::setMessage( char* buffer,
                                     short data )
    {
        FUNCTION_ENTRY( "setMessage" );
        
        buffer[1] = ( char )( data / 0x0100 );
        buffer[0] = ( char )( data % 0x0100 );

        FUNCTION_EXIT;
    }


    void RadioSimulator::setMessage( char* buffer,
                                     const char* data )
    {
        FUNCTION_ENTRY( "setMessage" );
        
        strcpy( buffer, data );

        FUNCTION_EXIT;
    }


    bool RadioSimulator::showSessions()
    {
        FUNCTION_ENTRY( "showSessions" );
        
        std::ostringstream output;

        output << std::endl << "Active Sessions" << std::endl;
        output << "------------------------" << std::endl;

        int count = 1;

        for ( std::vector< RadioCommsObserver * >::iterator it = m_commsObserverVector.begin();
              m_commsObserverVector.end() != it; ++it )
        {
            std::map<unsigned long, std::string> sessions = ( *it )->getSessionReferences();

            output << "Observer#" << count++ << ":  " << sessions.size() << std::endl;

            for ( std::map<unsigned long, std::string>::iterator sIt = sessions.begin();
                  sessions.end() != sIt; ++sIt )
            {
                output << sIt->first << "  <>  " << sIt->second << std::endl;
            }
        }

        COUT( output.str() );

        FUNCTION_EXIT;
        return true;
    }

}
