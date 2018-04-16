/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_simulator/protocols/COE/src/COESimulator.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * COESimulator implements ISimulationModule, for use with the generic simulator.
  *
  */


#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_simulator/protocols/COE/src/COESimulator.h"
#include "app/cctv/video_switch_simulator/protocols/COE/src/COESimulatorCommandProcessor.h"
#include "app/cctv/video_switch_simulator/protocols/COE/src/COECommsObserver.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"


using namespace TA_IRS_App;

using TA_Base_Core::ThreadGuard;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COESimulator::COESimulator( const std::string& name, const char * location, const char * host, const char * port, bool isActive )
        :    m_name( name ),
        m_location( location ),
        m_socketHost( host ),
        m_socketPort( port ),
        m_isActive( isActive ),
        m_commandProcessor( 0 ),
        m_threadIsRunning( false ),
        m_doThreadTerminate( false )
{
    m_commandProcessor = new COESimulatorCommandProcessor( *this );
    COEEquipmentState::getInstance().registerSimulator( m_location, *this );
}

COESimulator::~COESimulator()
{
    stopSim();
    COEEquipmentState::getInstance().deRegisterSimulator( m_location, *this );
}

//
// run()
//
void COESimulator::run()
{

    std::cout << "COE Simulator listener thread has started" << std::endl;

    // the run method is really the guts of it all
    m_threadIsRunning = true;
    TA_Base_Core::TcpServerSocket<TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket> >* serverSocket = 0;
    try
    {
        serverSocket = new TA_Base_Core::TcpServerSocket<TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > >( m_socketHost, m_socketPort );
        serverSocket->bind();
        serverSocket->listen();
    }
    catch ( ... )
    {
        // TODO: do something here
        std::cout << "COE Simulator listener thread caught exception during the construction of socket at address "
                  << m_socketHost << ":" << m_socketPort
                  << ", and will be terminated." << std::endl;
        m_doThreadTerminate = true;
    }

    // single-threaded connection-at-a-time
    while(!m_doThreadTerminate)
    {
        try
        {
            
            TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket >* acceptSocket = serverSocket->accept(TA_Base_Core::SERVER_NON_BLOCKING_ACCEPT);

            
            if ( 0 != acceptSocket )
            {
                COECommsObserver* com = new COECommsObserver( m_name, m_location, m_isActive, acceptSocket);
                
                addCommsObserver(com);

                m_isConnected = true;

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "New Connection accepted on socket." );
            }


            {
                ThreadGuard guard( m_observersLock );

                //we should check to see if all of our Observed Sockets are still connected.
                std::vector< COECommsObserver * >::iterator it = m_commsObserverVector.begin();

                while(it != m_commsObserverVector.end())
                {
                    if((*it)->stillConnected() == false)
                    {
                        COEEquipmentState::CommandLineOutGuard cmdLineOutGuard( COEEquipmentState::getInstance() );

                        std::cout << "Client has disconnected from " << m_name << std::endl;
                        removeCommsObserver(*it);
                        //This removes the comms observer from the vector, and removes the socket
                        //from the vector.
                                        
                        it = m_commsObserverVector.begin();
                    }
                    else
                    {
                        it++;
                    }
                }
            }
            sleep(500);
        }
        catch ( ... )
        {
            std::cout << "COE Simulator listener thread has produced exception and will be terminated" << std::endl;
            m_doThreadTerminate = false;
        }
    }
    if (serverSocket != 0)
    {
        delete serverSocket;
    }
    m_threadIsRunning = false;
}

//
// terminate()
//
void COESimulator::terminate()
{
    m_doThreadTerminate = true;
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "COESimulator::terminate()");
}

//////////////////////////////////////////////////////////////////
//                                                              //
//                  ISimulationModule stuff                     //
//                                                              //
//////////////////////////////////////////////////////////////////

//
// startSim()
//
void COESimulator::startSim()
{
    // start the listener thread.
    m_doThreadTerminate = false;
    start();
    while( false == m_threadIsRunning )
    {
        sleep(100);
    }
}

//
// stopSim()
//
void COESimulator::stopSim()
{
    m_doThreadTerminate = true;
    while( true == m_threadIsRunning )
    {
        sleep(100);
    }
    removeAllCommsObservers();
    terminateAndWait();
}

//
// restartSim()
//
void COESimulator::restartSim()
{
    stopSim();
    startSim();
}

//
// destroySim()
//
void COESimulator::destroySim()
{
    delete this;
}

//
// getName()
//
std::string COESimulator::getName()
{
    return m_name;
}

//
// getSimDescription()
//
std::string COESimulator::getSimDescription()
{
    return std::string( "COE simulator" );
}

//
// getCommandProcessor()
//
TA_Base_Bus::ISimulationCommandProcessor& COESimulator::getCommandProcessor()
{
    return *m_commandProcessor;
}





//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                              Execute methods                             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


//
//execStatus
//
bool COESimulator::execStatus()
{
    int numConnections = m_commsObserverVector.size();
    std::cout << std::endl;
    std::cout << "COE Simulator Status" << std::endl;   
    std::cout << "-------------------------" << std::endl;
    std::cout << "Number of Connections:  " << numConnections << std::endl << std::endl;
    return true;

}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                              Simulator methods                           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


//
//getCommsObserverVector
//
std::vector< COECommsObserver * >& COESimulator::getCommsObserverVector()
{ 
    return m_commsObserverVector; 
}

//
//addCommsObserver
//
void COESimulator::addCommsObserver(COECommsObserver* com)
{
    ThreadGuard guard( m_observersLock );
    COEEquipmentState::CommandLineOutGuard cmdLineOutGuard( COEEquipmentState::getInstance() );
    std::cout << "Client has connected to " << m_name << std::endl;
    m_commsObserverVector.push_back(com);
}

//
//removeCommsObserver
//
void COESimulator::removeCommsObserver(COECommsObserver* com)
{
    std::vector< COECommsObserver * >::iterator it = std::find(m_commsObserverVector.begin(), m_commsObserverVector.end(), com);
    if( it != m_commsObserverVector.end() )
    {
        //remove the CommsObserver
        m_commsObserverVector.erase( it );
    }
}


//
//removeAllCommsObservers
//
void COESimulator::removeAllCommsObservers()
{
    ThreadGuard guard( m_observersLock );

    //remove all the CommsObservers
    std::vector< COECommsObserver * >::iterator it = m_commsObserverVector.begin();

    while(it != m_commsObserverVector.end())
    {
        delete *it;
        m_commsObserverVector.erase(it);
    }
}


//
// updateObserversWithMonitorState
//
void COESimulator::updateObserversWithMonitorState( unsigned char monitorID, const COEEquipmentState::MonitorState& state )
{
    ThreadGuard guard( m_observersLock );

    std::vector< COECommsObserver * >::iterator it = m_commsObserverVector.begin();
    while(it != m_commsObserverVector.end())
    {
        ( *it )->sendMonitorState( monitorID, state );
        it++;
    }    
}


//
// updateObserversWithQuadState
//
void COESimulator::updateObserversWithQuadState( unsigned char quadID, const COEEquipmentState::QuadState& state )
{
    ThreadGuard guard( m_observersLock );

    std::vector< COECommsObserver * >::iterator it = m_commsObserverVector.begin();
    while(it != m_commsObserverVector.end())
    {
        ( *it )->sendQuadState( quadID, state );
        it++;
    }    
}


//
// updateObserversWithSequenceState
//
void COESimulator::updateObserversWithSequenceState( unsigned char sequenceID, const COEEquipmentState::SequenceState& state )
{
    ThreadGuard guard( m_observersLock );

    std::vector< COECommsObserver * >::iterator it = m_commsObserverVector.begin();
    while(it != m_commsObserverVector.end())
    {
        ( *it )->sendSequenceState( sequenceID, state );
        it++;
    }  
}


//
// updateObserversWithRecordingUnitState
//
void COESimulator::updateObserversWithRecordingUnitState( unsigned char recordingUnitID, COEEquipmentState::ERecordingUnitState state )
{
    ThreadGuard guard( m_observersLock );

    std::vector< COECommsObserver * >::iterator it = m_commsObserverVector.begin();
    while(it != m_commsObserverVector.end())
    {
        ( *it )->sendRecordingUnitState( recordingUnitID, state );
        it++;
    }    
}



// 
// updateObserversWithSwitchStatus
//
void COESimulator::updateObserversWithSwitchStatus( bool isActive )
{
    ThreadGuard guard( m_observersLock );

    std::vector< COECommsObserver * >::iterator it = m_commsObserverVector.begin();
    while(it != m_commsObserverVector.end())
    {
        ( *it )->setSwitchStatus( isActive );
        it++;
    }

    // Set the simulator flag so new observers will be given current status also.
    m_isActive = isActive;
}

// 
// updateObserversWithTriggeringEvents
//
void COESimulator::updateObserversWithTriggeringEvents( TriggeringEventList& events )
{
	ThreadGuard guard( m_observersLock );

    std::vector< COECommsObserver * >::iterator it = m_commsObserverVector.begin();
    while(it != m_commsObserverVector.end())
    {
        ( *it )->sendTriggeringEvents( events );
        it++;
    }    


}
