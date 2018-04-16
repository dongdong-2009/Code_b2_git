
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimulator.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * 
  * Definition of the ATSSimulator class.
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include <iostream>
#include <sstream>
#include <iomanip>

#include "app/ats/ATSSimulatorLibrary/src/ATSSimulator.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableQueryProcessor.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommsObserver.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimulatorCommandProcessor.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"

#include "app/ats/ATSSimulatorLibrary/cmd/HierarchicalCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/SetCommand.h"
#include "app/ats/ATSSimulatorLibrary/cmd/ShowCommand.h"

#include "core/sockets/src/TcpSocket.h"
#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/ITcpSocketObserver.h"
#include "core/sockets/src/TcpServerSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"


// using namespace TA_IRS_App;
using namespace TA_IRS_App::ATS_Sim;

/** some port string args are just a number, which are passed straight back.
  * Some are LOCATION=PORT pairs, which we must register with the
  * TableAddressRegister
  */
static std::string getPortString(const std::string & portstr, LocationType location)
{
    std::list<std::string> port_params;
    ATSSimUtility::tokenise(portstr.c_str(), port_params, "=");
    std::string s="";

    if (port_params.size()==2)
    {
        std::string locationstr = port_params.front();
        port_params.pop_front();
        TableAddressRegister::getInstance().registerLocationIdentifier(location, locationstr);

        s = port_params.front();
    }
    else if (port_params.size()==1)
    {
        s = portstr;
    }
    else // wacky number of parameters
    {
        std::cout << "ERROR: invalid port argument: " << portstr << std::endl;
    }
        
    return s;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ATSSimulator::ATSSimulator
(   const std::string & name,
    LocationType location,
    const char * host,
    const char * port)
: m_name(name)
, m_threadIsRunning(false)
, m_doThreadTerminate(false)
, m_location(location)
, m_socketHost(host)
, m_commandProcessor(0)
, m_queryProcessor(0)
{
    TokenList params;
    int numports = ATSSimUtility::tokenise(port, params, ",");

    if (numports == 3)
    {
        if (location == STATION)
        {
            std::string portstr = params.front();
            params.pop_front();
            if ((portstr = getPortString( portstr, MSS )) != "" )
            {
                m_socketPorts[MSS] = portstr;
            }

            portstr = params.front();
            params.pop_front();
            if ((portstr = getPortString( portstr, SS1 )) != "" )
            {
                m_socketPorts[SS1] = portstr;
            }

            portstr = params.front();
            params.pop_front();
            if ((portstr = getPortString( portstr, SS2 )) != "" )
            {
                m_socketPorts[SS2] = portstr;
            }
        }
        else
        {
            std::cerr << "ERROR: wrong number of ports for location." << std::endl;
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "ERROR: wrong number of ports for location." );
        }
    }
    else if (numports == 1)
    {
        std::string portstr = params.front();
        params.pop_front();
        m_socketPorts[location] = portstr;
    }
    else
    {
        std::cerr << "ERROR: wrong number of parameters for module. Require <host> <portlist>" << std::endl;
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "ERROR: too few parameters for module. Require <host> <portlist>" );
    }

    m_commandProcessor = new ATSSimulatorCommandProcessor(this, location);

    m_queryProcessor = new ATSSimTableQueryProcessor(this, m_location);
}

ATSSimulator::~ATSSimulator()
{
    stopSim();

    { // block to restrict the scope of the ThreadGuard
        TA_Base_Core::ThreadGuard oguard(m_observerLock);

        CommsObserverContainer::iterator it = m_commsObservers.begin();
        while (it != m_commsObservers.end())
        {
			delete (*it);
			(*it) = NULL;

            it++;
        }
    } // ThreadGuard scope restriction block

    delete m_queryProcessor;
    
    delete m_commandProcessor;
}

//
// run()
//
// @todo something more useful in the catch(...) blocks
void ATSSimulator::run()
{

    FUNCTION_ENTRY("ATSSimulator::run");
    std::cout << "ATS Simulator thread has started" << std::endl;

    // the run method is really the guts of it all
    m_threadIsRunning = true;

    std::string socketPort;
    std::list< ATSSimCommsObserver::ServerSocketType * > serverSockets;

    try
    {
        for ( PortLocationMap::iterator port_iter = m_socketPorts.begin() ;
              port_iter != m_socketPorts.end() ;
              port_iter++
            )
        {
            socketPort = (*port_iter).second;
            ATSSimCommsObserver::ServerSocketType * serverSocket = new ATSSimCommsObserver::ServerSocketType( m_socketHost, socketPort );
            serverSocket->bind();
            serverSocket->listen();
            std::cout   << "ATS Simulator listening on "
                        << m_socketHost << ":" << socketPort << std::endl;
            serverSockets.push_back(serverSocket);
        }
    }
    catch ( ... )
    {
        std::cout << "ATS Simulator listener thread caught exception during the construction of server socket at address "
                << m_socketHost << ":" << socketPort << ", and will be terminated." << std::endl;
        m_doThreadTerminate = true;
    }

    // single-threaded connection-at-a-time
    while(!m_doThreadTerminate)
    {
        try
        {
            
            { // block to restrict the scope of the ThreadGuard
                TA_Base_Core::ThreadGuard oguard(m_observerLock);

                for ( std::list< ATSSimCommsObserver::ServerSocketType * >::iterator socket_iter = serverSockets.begin() ;
                      socket_iter != serverSockets.end() ;
                      socket_iter++
                    )
                {
                    ATSSimCommsObserver::ObservedSocketType * acceptSocket = (*socket_iter)->accept(TA_Base_Core::SERVER_NON_BLOCKING_ACCEPT);
                    
                    if ( acceptSocket != 0 )
                    {

                        ATSSimCommsObserver * com = new ATSSimCommsObserver(acceptSocket, m_location, m_queryProcessor);
                        
                        addCommsObserver(com);
                    }
                }
            }

			std::vector<ATSSimCommsObserver*> deleteList;

            { // block to restrict the scope of the ThreadGuard
                TA_Base_Core::ThreadGuard oguard(m_observerLock);

                // check to see if all of our Observed Sockets are still connected.
                CommsObserverContainer::iterator it = m_commsObservers.begin();

                while(it != m_commsObservers.end())
                {
                    try
                    {
                        if ((*it) != NULL)
						{
							if(!((*it)->getSocket()->stillConnected()))
							{
								deleteList.push_back(*it);
								(*it) = NULL;
								it = removeCommsObserver(it);
							}
							else
							{
								it++;
							}
						}
						else
						{
							it++;
						}
                    }
                    catch (...)
                    {
                        deleteList.push_back(*it);
						(*it) = NULL;
						it = removeCommsObserver(it);
                    }
                }
            }

			// Now need to delete these - this could take a while...
			std::vector<ATSSimCommsObserver*>::iterator deleteIt;

			for (deleteIt = deleteList.begin(); deleteIt != deleteList.end(); deleteIt++)
			{
				try
				{
					delete (*deleteIt);
				}
				catch (...)
				{
				}

				(*deleteIt) = NULL;
			}
        }
        catch ( ... )
        {
            m_threadIsRunning = false;
            throw;
        }

        sleep(250);
    }

    m_threadIsRunning = false;

    FUNCTION_EXIT;

}

//
// terminate()
//
void ATSSimulator::terminate()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ATSSimulator::terminate()");
}

//////////////////////////////////////////////////////////////////
//                                                              //
//                   ISimulationModule stuff                    //
//                                                              //
//////////////////////////////////////////////////////////////////

//
// startSim()
//
void ATSSimulator::startSim()
{
    m_doThreadTerminate = false;
    start();
    while( !m_threadIsRunning )
    {
        sleep(100);
    }
}

//
// stopSim()
//
void ATSSimulator::stopSim()
{
    std::cout << getSimDescription() << " " << getName() << " stopping..." << std::endl;

    // Stop all the comms observer threads to simulate Ats hardware failure
    { // block to restrict the scope of the ThreadGuard
        TA_Base_Core::ThreadGuard oguard(m_observerLock);

        for ( CommsObserverContainer::iterator it = m_commsObservers.begin();
              it != m_commsObservers.end();
              it++
            )
        {
			if ((*it) != NULL)
			{
				(*it)->getSocket()->terminateAndWait();
			}
        }
    } // ThreadGuard scope restriction block

    m_doThreadTerminate = true;
    
    while( m_threadIsRunning )
    {
        sleep(100);
    }

    // it should already be dead but this is a good cleanup
    terminateAndWait();
}

//
// restartSim()
//
void ATSSimulator::restartSim()
{
    stopSim();
    startSim();
}

//
// destroySim()
//
void ATSSimulator::destroySim()
{
    delete this;
}

//
// getName()
//
std::string ATSSimulator::getName()
{
    return m_name;
}

//
// getSimDescription()
//
std::string ATSSimulator::getSimDescription()
{
    return std::string( "Ats Simulator" );
}

//
// getCommandProcessor()
//
TA_Base_Bus::ISimulationCommandProcessor& ATSSimulator::getCommandProcessor()
{
    return *m_commandProcessor;
}


bool ATSSimulator::execStatus(std::ostream & streem)
{
    // TODO: search and replace std::cout to streem
    int numConnections = m_commsObservers.size();
    std::cout << std::endl;
    std::cout << "ATS Simulator Status. Number of Connections=" << numConnections << std::endl << std::endl;
    if (numConnections < 1) return true;

    TA_Base_Core::ThreadGuard oguard(m_observerLock);

    std::cout << "+-----------+-----------+---------------------" << std::endl;
    std::cout << "| socket id | connected | client name         " << std::endl;
    std::cout << "+-----------+-----------+---------------------" << std::endl;
    CommsObserverContainer::iterator iter = m_commsObservers.begin();
    while ( iter != m_commsObservers.end() )
    {
        try
        {
			if ((*iter) != NULL)
			{
				TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket> * socket = (*iter)->getSocket();
				char buffStr[16];
				sprintf(buffStr, ".%7d", socket->getSocketId());
				std::string socketIdStr=buffStr;
				std::string connectedStr;
				ATSSimUtility::bool2string(socket->stillConnected(), connectedStr);
				// std::string cname = socket->getClientName();
				std::string cname = (*iter)->getClientName();
				std::cout << "|  " << socketIdStr.c_str() << "  |   " << connectedStr.c_str() << "   | " << cname.c_str() << std::endl;
			}
			else
			{
				std::cout << " [Invalid connection]" << std::endl;
			}

            iter++;
        }
        catch (...)
        {
            std::cout << " [Exception Caught, invalid connection]" << std::endl;
        }
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                                Simulator methods                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


void ATSSimulator::addCommsObserver(ATSSimCommsObserver * com)
{
    TA_Base_Core::ThreadGuard oguard(m_observerLock);

    std::cout << "New ISCS connected to " << m_name << " simulator" << std::endl;
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "New ISCS connected to %s simulator", m_name.c_str());

    m_commsObservers.push_back(com);
}

ATSSimulator::CommsObserverContainer::iterator ATSSimulator::removeCommsObserver( CommsObserverContainer::iterator iter )
{
    TA_Base_Core::ThreadGuard oguard(m_observerLock);

    std::cout << "ISCS disconnected from " << m_name << " simulator" << std::endl;
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ISCS disconnected from %s simulator", m_name.c_str());

    // remove the CommsObserver. DO NOT destroy, that is done elsewhere
    return m_commsObservers.erase(iter);
}


TableOutputFormat ATSSimulator::getOutputFormat() const
{
    return m_queryProcessor->getTableOutputFormat();
}










