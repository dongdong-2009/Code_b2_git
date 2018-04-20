
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/stis_simulator/src/STISSimulator.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * STISSimulator implements ISimulationModule, for use with the generic simulator.
  */

#pragma warning (disable : 4786)

#include <iostream>
#include <sstream>
#include <iomanip>

#include "app/signs/tis_agent/stis_simulator/src/STISSimulator.h"
#include "app/signs/tis_agent/stis_simulator/src/STISMessageProcessor.h"
#include "app/signs/tis_agent/stis_simulator/src/STISSimulatorCommandProcessor.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"


namespace TA_IRS_App
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	STISSimulator::STISSimulator(const std::string &name,const char* port, const std::string& location)
	:			m_name(name),
				m_threadIsRunning(false),
				m_doThreadTerminate(false),
				m_socketPort(port),
				m_location(location),
                m_commandProcessor(0),
                m_stisMessageProcessor(0),
                m_ratisResponseType(POSITIVE_RESPONSE),
                m_displayRequestResponseType(POSITIVE_RESPONSE)

	{
		m_commandProcessor = new STISSimulatorCommandProcessor(this);

		
     //   m_stisMessageProcessor = new STISMessageProcessor();

	}

	STISSimulator::~STISSimulator()
	{
		//TD 14399
		//zhou yuan++
		if(m_commsObserverVector.size()>0)
		{
			std::vector< TA_IRS_App::STISCommsObserver * >::iterator it = m_commsObserverVector.begin();
			
			// Disconnect all others clients
			while(it != m_commsObserverVector.end())
			{
				try
				{
					std::cout << "TIS Agent disconnected [" << (*it)->getSocket()->getClientName() << "]" << std::endl;
				}
				catch (...) 
				{
					//do nothing , because the getClientName() may throw exception
				}
				delete *it;
				it = m_commsObserverVector.erase(it);
			}
		}
		
		stopSim();

	}

	//
	// run()
	//
	void STISSimulator::run()
	{

        FUNCTION_ENTRY("STISSimulator::run");
		std::cout << "STIS Simulator thread has started" << std::endl;
	
		// the run method is really the guts of it all
		m_threadIsRunning = true;

		TA_Base_Core::TcpServerSocket<TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket> >* serverSocket = 0;
		try
		{
			//serverSocket = new TA_Base_Core::TcpServerSocket<TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > >( m_socketHost, m_socketPort );
			serverSocket = new TA_Base_Core::TcpServerSocket<TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > >( "localhost", m_socketPort );
			serverSocket->bind();
			serverSocket->listen();
		}
		catch ( ... )
		{
			// TODO: do something here
			std::cout << "COE Simulator listener thread caught exception during the construction of socket at address " << m_socketHost << ":" 
				<< m_socketPort << ", and will be terminated." << std::endl;
			m_doThreadTerminate = true;
		}

		// single-threaded connection-at-a-time
		while(!m_doThreadTerminate)
		{
			try
			{
				TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket >* acceptSocket;

				try
				{
					acceptSocket = serverSocket->accept(TA_Base_Core::SERVER_NON_BLOCKING_ACCEPT);
				}
				catch ( const TA_Base_Core::TcpSocketException& )
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "accept() threw an exception" );
				}

				
				if ( 0 != acceptSocket )
				{
					m_isConnected = true;
				
					TA_IRS_App::STISCommsObserver* com = new TA_IRS_App::STISCommsObserver(acceptSocket,m_location, m_ratisResponseType, m_displayRequestResponseType);
					
					//TD 14399
					//zhou yuan++
					if(m_commsObserverVector.size()>0)
					{
						std::vector< TA_IRS_App::STISCommsObserver * >::iterator it = m_commsObserverVector.begin();

						// Disconnect all others clients
						while(it != m_commsObserverVector.end())
						{
							try
							{
								std::cout << "TIS Agent disconnected [" << (*it)->getSocket()->getClientName() << "]" << std::endl;
							}
							catch (...) 
							{
								//do nothing , because the getClientName() may throw exception
							}
							delete *it;
							it = m_commsObserverVector.erase(it);
						}
					}



					addCommsObserver(com);

					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "New Connection accepted on socket." );
				}

				//TD 8191
				//zhou yuan++
				std::vector<STISCommsObserver*> deleteList;

				{ 
					// block to restrict the scope of the ThreadGuard
					// check to see if all of our Observed Sockets are still connected.
					std::vector< TA_IRS_App::STISCommsObserver * >::iterator it = m_commsObserverVector.begin();

					while(it != m_commsObserverVector.end())
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
				std::vector<STISCommsObserver*>::iterator deleteIt;

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
				//++zhou yuan
			}
			catch ( ... )
			{
				// TODO: do something here
				m_threadIsRunning = false;
				m_isConnected = false;
				//throw;
			}

			sleep(500);
		}

		m_threadIsRunning = false;

        FUNCTION_EXIT;

	}

	//
	// terminate()
	//
	void STISSimulator::terminate()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "STISSimulator::terminate()");
	}

	//////////////////////////////////////////////////////////////////
	//																//
	//					ISimulationModule stuff						//
	//																//
	//////////////////////////////////////////////////////////////////

	//
	// startSim()
	//
	void STISSimulator::startSim()
	{
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
	void STISSimulator::stopSim()
	{
		m_doThreadTerminate = true;
        
		while( true == m_threadIsRunning )
		{
			sleep(100);
		}

  /*      if( 0 != m_commandProcessor )
        {
            delete m_commandProcessor;
        } 

        if( 0 != m_stisMessageProcessor )
        {
            delete m_stisMessageProcessor;
        } */

		// it should already be dead but this is a nice cleanup
		terminateAndWait();
	}

	//
	// restartSim()
	//
	void STISSimulator::restartSim()
	{
		stopSim();
		startSim();
	}

	//
	// destroySim()
	//
	void STISSimulator::destroySim()
	{
		delete this;
	}

	//
	// getName()
	//
	std::string STISSimulator::getName()
	{
		return m_name;
	}

	//
	// getSimDescription()
	//
	std::string STISSimulator::getSimDescription()
	{
		return std::string( "STIS simulator" );
	}

	//
	// getCommandProcessor()
	//
	TA_Base_Bus::ISimulationCommandProcessor& STISSimulator::getCommandProcessor()
	{
		return *m_commandProcessor;
	}
	
	
	//////////////////////////////////////////////////////////////////////////////
	//																			//
	//								Execute methods								//
	//																			//
	//////////////////////////////////////////////////////////////////////////////


    //
    //execSendPacket
    //
    bool STISSimulator::execSendPacket( const std::string& params )
    {
        //we assume that the entire parameter string is one packet, as the space character
        //can be used in a number of fields.

		TA_IRS_App::STISMessageProcessor::PacketLengthUnion plu;
        unsigned short hostSize = params.size();
        plu.us = htons(hostSize);

        std::vector< unsigned char > packet;

        //add the SOH byte
        packet.push_back('@');

        //insert the PKT_LEN field
        packet.push_back(plu.uc[0]);
        packet.push_back(plu.uc[1]);

        //now set the union for the checksum value, which is the packet sum + 2
        //hostSize += 2;
        //plu.us = htons(hostSize);

        //add the packet string specified by the user
        std::copy( params.begin(), params.end(), std::back_inserter(packet) );

        std::vector< unsigned char >::iterator iter = packet.begin();
        iter++;
        unsigned int checksumValue = 0;
        for(; iter != packet.end(); ++iter)
        {
            checksumValue += (int)(*iter);
        }

        plu.us = htons(checksumValue);

        //add the checksum
        packet.push_back(plu.uc[0]);
        packet.push_back(plu.uc[1]);

        //add the ETB
        packet.push_back('\n');

        //fire it off!
        m_stisMessageProcessor->addMessageRequest(packet);


        return true;
    }

	
	//
	//execStatus
	//
	bool STISSimulator::execStatus()
	{
		int numConnections = m_commsObserverVector.size();
		std::cout << std::endl;
		std::cout << "STIS Simulator Status" << std::endl;	
		std::cout << "-------------------------" << std::endl;
		std::cout << "Number of Connections:  " << numConnections << std::endl << std::endl;

        //TD9161
        std::cout << "Ratis Response Type: "<<m_ratisResponseType<< std::endl;
        std::cout << "Display Request Response Type: " <<m_displayRequestResponseType<< std::endl << std::endl;

        return true;

	}

    //
    //execSwitch
    //
    bool STISSimulator::pidStatusChange( const std::string& params)
    {

        // the PID address is the first param
        int tmpIndex = params.find(" ", 0);
        std::string pidAddress = params.substr( 0, tmpIndex );

        std::string newStatus = params.substr( tmpIndex+1, params.size() );

		// call updatePIDStatus in the comms observer's message processor

		std::vector< TA_IRS_App::STISCommsObserver* >::iterator iter;

		for( iter = m_commsObserverVector.begin() ; iter != m_commsObserverVector.end() ; iter++ )
		{
			(*iter)->updatePIDStatus( atoi(pidAddress.c_str()), atoi(newStatus.c_str()) );
		}
		//m_stisMessageProcessor->updatePIDStatus( atoi(pidAddress.c_str()), atoi(newStatus.c_str()) );

        return true;

    }

	bool STISSimulator::resourceStatusChange( const std::string& params )
	{
        // the resource name is the first param
        int tmpIndex = params.find(" ", 0);
        std::string resourceName = params.substr( 0, tmpIndex );

        std::string newStatus = params.substr( tmpIndex+1, params.size() );		

		std::vector< TA_IRS_App::STISCommsObserver* >::iterator iter;

		for( iter = m_commsObserverVector.begin() ; iter != m_commsObserverVector.end() ; iter++ )
		{
			(*iter)->updateResourceStatus( resourceName, atoi(newStatus.c_str()) );
		}
	
		return true;
	}

	bool STISSimulator::displayResourceStatus()
	{
		std::vector< TA_IRS_App::STISCommsObserver* >::iterator iter;

		for( iter = m_commsObserverVector.begin() ; iter != m_commsObserverVector.end() ; iter++ )
		{
			(*iter)->displayResourceStatus();
		}
	
		return true;
	}

	bool STISSimulator::newStisLibrary( const std::string& params )
	{
		int tmpIndex = params.find(" ",0);
		std::string libraryVersionStr = params.substr( 0, tmpIndex );

		std::vector< TA_IRS_App::STISCommsObserver* >::iterator iter;
		for( iter = m_commsObserverVector.begin() ; iter != m_commsObserverVector.end() ; iter++ )
		{
			(*iter)->updateLibraryVersion( "STIS", atoi(libraryVersionStr.c_str()) );
		}

		return true;
	}

	bool STISSimulator::newTtisLibrary( const std::string& params )
	{
		int tmpIndex = params.find(" ",0);
		std::string libraryVersionStr = params.substr( 0, tmpIndex );

		std::vector< TA_IRS_App::STISCommsObserver* >::iterator iter;
		for( iter = m_commsObserverVector.begin() ; iter != m_commsObserverVector.end() ; iter++ )
		{
			(*iter)->updateLibraryVersion( "TTIS", atoi(libraryVersionStr.c_str()) );
		}

		return true;
	}

	bool STISSimulator::currentStisLibrary( const std::string& params )
	{
		int tmpIndex = params.find(" ",0);
		std::string libraryVersionStr = params.substr( 0, tmpIndex );

		std::vector< TA_IRS_App::STISCommsObserver* >::iterator iter;
		for( iter = m_commsObserverVector.begin() ; iter != m_commsObserverVector.end() ; iter++ )
		{
			(*iter)->updateCurrentSTISLibraryVersion( atoi(libraryVersionStr.c_str()) );
		}

		return true;
	}
	
    //
    //sendRATISPacket
    //
    bool STISSimulator::sendRATISPacketToStation( const std::string& params )
    {
		std::vector< TA_IRS_App::STISCommsObserver* >::iterator iter;
		for( iter = m_commsObserverVector.begin() ; iter != m_commsObserverVector.end() ; iter++ )
		{
			(*iter)->sendRATISPacketToStation(params);
		}
        return true;
    }


    bool STISSimulator::sendRATISPacketToTrain( const std::string& params )
    {
		std::vector< TA_IRS_App::STISCommsObserver* >::iterator iter;
		for( iter = m_commsObserverVector.begin() ; iter != m_commsObserverVector.end() ; iter++ )
		{
			(*iter)->sendRATISPacketToTrain(params);
		}
        return true;
    }


    bool STISSimulator::sendRATISPacketToAllStations( const std::string& params )
    {
		std::vector< TA_IRS_App::STISCommsObserver* >::iterator iter;
		for( iter = m_commsObserverVector.begin() ; iter != m_commsObserverVector.end() ; iter++ )
		{
			(*iter)->sendRATISPacketToAllStations(params);
		}
        return true;
    }


    bool STISSimulator::sendRATISPacketToAllTrains( const std::string& params )
    {
		std::vector< TA_IRS_App::STISCommsObserver* >::iterator iter;
		for( iter = m_commsObserverVector.begin() ; iter != m_commsObserverVector.end() ; iter++ )
		{
			(*iter)->sendRATISPacketToAllTrains(params);
		}
        return true;
    }

	//TD9161
	bool STISSimulator::setDisplayRequestResponseType( const std::string& params)
	{
		std::stringstream commandString(params);
		int responseType;
		commandString>>responseType;

		if( responseType == 0)
		{
			m_displayRequestResponseType = POSITIVE_RESPONSE;
		}
		else if( responseType == 1)
		{
			m_displayRequestResponseType = NEGATIVE_RESPONSE;
		}
		else if( responseType == 2)
		{
			m_displayRequestResponseType = NO_RESPONSES;
		}

        std::vector< STISCommsObserver * >::iterator it;
        for( it=m_commsObserverVector.begin(); it != m_commsObserverVector.end(); ++it)
        {
            (*it)->setDisplayRequestResponseType(m_displayRequestResponseType);
        }

		return true;
	}

	//TD9161
	bool STISSimulator::setRATISResponseType( const std::string& params)
	{
		std::stringstream commandString(params);
		int responseType;
		commandString>>responseType;

		if( responseType == 0)
		{
			m_ratisResponseType = POSITIVE_RESPONSE;
		}
		else if( responseType == 1)
		{
			m_ratisResponseType = NEGATIVE_RESPONSE;
		}
		else if( responseType == 2)
		{
			m_ratisResponseType = NO_RESPONSES;
		}

        std::vector< STISCommsObserver * >::iterator it;

        for( it=m_commsObserverVector.begin(); it != m_commsObserverVector.end(); ++it)
        {
            (*it)->setRATISResponseType(m_ratisResponseType);
        }        


		return true;
	}
	//////////////////////////////////////////////////////////////////////////////
	//																			//
	//								Simulator methods							//
	//																			//
	//////////////////////////////////////////////////////////////////////////////

	
	//
	//getCommsObserverVector
	//
	std::vector< STISCommsObserver * >& STISSimulator::getCommsObserverVector()
	{ 
		return m_commsObserverVector; 
	}

	//
	//addCommsObserver
	//
	void STISSimulator::addCommsObserver(STISCommsObserver* com)
	{
		//TD 14399
		//zhou yuan++
		try
		{
			std::cout << "TIS Agent connected to simulator [" << com->getSocket()->getClientName() << "]" << std::endl;
		}
		catch(...)
		{
		}
		m_commsObserverVector.push_back(com);
		TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket >* sock = com->getSocket();

	//	char buf[11];
	//	sprintf(buf, "0123456789");
	//	sock->write( (const char*)buf, 11  );
	}

	//
	//removeCommsObserver
	//

	//TD 8191
	//zhou yuan++
	std::vector<STISCommsObserver*>::iterator STISSimulator::removeCommsObserver( std::vector<STISCommsObserver*>::iterator iter )
	{
		std::cout << "ISCS disconnected from " << m_name << " simulator" << std::endl;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ISCS disconnected from %s simulator", m_name.c_str());

		// remove the CommsObserver. DO NOT destroy, that is done elsewhere
		return m_commsObserverVector.erase(iter);
	}
	//++zhou yuan


}//close namespace
