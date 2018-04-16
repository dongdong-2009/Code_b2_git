
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/RadioSimulator.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  * 
  * 
  * Definition of the PASimulator class.
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <string>
#include <algorithm>


#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMHelper.h"

#include "app/radio/radio_simulator/TrainSimulator/RadioSimulator.h"
#include "app/radio/radio_simulator/TrainSimulator/RadioSimulatorCommandProcessor.h"
#include "app/radio/radio_simulator/TrainSimulatorLib/src/RadioCommsObserver.h"

#include "app/radio/radio_simulator/TrainSimulator/SimulatorHelper.h"

//#include "bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h"
#include "app/radio/radio_simulator/TrainSimulator/FrameCheckSequence.h"


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "app/radio/radio_simulator/TrainSimulator/SimulatorConfig.h"

#include "core/synchronisation/src/ThreadGuard.h"

using namespace TA_IRS_App;
using namespace TA_IRS_Bus;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RadioSimulator::RadioSimulator(const std::string &name,const char* host,const char* port, TA_IRS_App::IModelLogger* modelLogger)
: m_name(name)
, m_threadIsRunning(false)
, m_doThreadTerminate(false)
, m_socketPort(port)
, m_socketHost(host)
, m_commandProcessor(0)
, m_motorolaSDSProtocolHelper(NULL)
, m_radioDispatcher(NULL)
, m_pecCommandProcessor(NULL)
, m_paCommandProcessor(NULL)
, m_ttisCommandProcessor(NULL)
,m_modelLogger(modelLogger)
,m_endOfTtisDownload(NULL)
,m_changeoverStatus(NULL)
,m_requestOccCall(NULL)
,m_threadPool( TA_Base_Core::ThreadPoolSingletonManager::getInstance())
,m_regStatus(false)
,m_changeCommandProcessor(NULL)
,m_testCallCommand(NULL)
{
	initialize();
}

RadioSimulator::~RadioSimulator()
{
    stopSim();

	m_radioDispatcher->detachCommandProcessor(m_ttisCommandProcessor);
	m_radioDispatcher->detachCommandProcessor(m_paCommandProcessor);
	m_radioDispatcher->detachCommandProcessor(m_pecCommandProcessor);
	m_radioDispatcher->detachCommandProcessor(m_changeCommandProcessor);


	if ( NULL != m_pecCommandProcessor )
	{
		delete m_pecCommandProcessor;
		m_pecCommandProcessor = NULL;
	}
	
	if ( NULL != m_paCommandProcessor )
	{
		delete m_paCommandProcessor;
		m_paCommandProcessor = NULL;
	}

	if ( NULL != m_ttisCommandProcessor )
	{
		delete m_ttisCommandProcessor;
		m_ttisCommandProcessor = NULL;
	}

	if ( NULL != m_changeCommandProcessor )
	{
		delete m_changeCommandProcessor;
		m_changeCommandProcessor = NULL;
	}

	HeartBeatProcessorData::iterator iter = m_HeartBeatProcessors.begin();
	for ( iter; iter != m_HeartBeatProcessors.end(); iter++ )
	{
		
		if ( NULL != (*iter) )
		{
			delete (*iter);
			(*iter) = NULL;
		}
		
	}

	if ( NULL != m_motorolaSDSProtocolHelper)
	{
		delete m_motorolaSDSProtocolHelper;
		m_motorolaSDSProtocolHelper = NULL;
	}

	if ( NULL != m_radioDispatcher )
	{
		delete m_radioDispatcher;
		m_radioDispatcher = NULL;
	}

	if ( NULL != m_endOfTtisDownload )
	{
		delete m_endOfTtisDownload;
		m_endOfTtisDownload = NULL;
	}

	if ( NULL != m_changeoverStatus )
	{
		delete m_changeoverStatus;
		m_changeoverStatus = NULL;
	}

	if ( NULL != m_requestOccCall )
	{
		delete m_requestOccCall;
		m_requestOccCall = NULL;
	}

	if ( NULL != m_testCallCommand )
	{
		delete m_testCallCommand;
		m_testCallCommand = NULL;
	}

	TA_Base_Core::ThreadPoolSingletonManager::removeInstance( m_threadPool );
	m_threadPool = NULL;

}

void RadioSimulator::initialize()
{
	 m_commandProcessor = new RadioSimulatorCommandProcessor(this);

	m_motorolaSDSProtocolHelper = new MotorolaSDSProtocolHelper(this);
	m_radioDispatcher = new RadioDispatcher();
	m_motorolaSDSProtocolHelper->registerSDSMessage(m_radioDispatcher);

	m_pecCommandProcessor = new PECCommandProcessor(*m_motorolaSDSProtocolHelper);
	m_paCommandProcessor = new PACommandProcessor(*m_motorolaSDSProtocolHelper);
	m_ttisCommandProcessor = new TTISCommandProcessor(*m_motorolaSDSProtocolHelper);
	m_changeCommandProcessor = new ChangeModeCommandProcessor(*m_motorolaSDSProtocolHelper);

	//register logger
	m_pecCommandProcessor->registerLogger(m_modelLogger);
	m_paCommandProcessor->registerLogger(m_modelLogger);
	m_ttisCommandProcessor->registerLogger(m_modelLogger);
	m_changeCommandProcessor->registerLogger(m_modelLogger);

	//attached the interested processors.
	m_radioDispatcher->attachCommandProcessor(m_pecCommandProcessor);
	m_radioDispatcher->attachCommandProcessor(m_paCommandProcessor);
	m_radioDispatcher->attachCommandProcessor(m_ttisCommandProcessor);
	m_radioDispatcher->attachCommandProcessor(m_changeCommandProcessor);

	m_endOfTtisDownload = new EndOfttisDownload;
	m_changeoverStatus  = new ChangeoverStatus;
	m_requestOccCall    = new RequestOccCall;
	m_testCallCommand   = new TestCallCommand;

	//Initialize HeartBeat Processors
	initializeTrainHeartBeat();

}

MotorolaSDSProtocolHelper* RadioSimulator::getInstanceMotorolaHelper()
{
	return m_motorolaSDSProtocolHelper;
}

TTISCommandProcessor* RadioSimulator::getTTISCommandProcessor()
{
	return m_ttisCommandProcessor;
}

//
// run()
//
// @todo something more useful in the catch(...) blocks
void RadioSimulator::run()
{

    FUNCTION_ENTRY("RadioSimulator::run");
    //std::cout << "Radio Simulator thread has started" << std::endl;
	m_modelLogger->setLogView("Radio Simulator thread has started");


    // the run method is really the guts of it all
    m_threadIsRunning = true;
	
	typedef TA_Base_Core::TcpServerSocket< TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > > RadioSimulatorServerSocket;
    RadioSimulatorServerSocket* serverSocket = 0;
    try
    {
        serverSocket = new RadioSimulatorServerSocket( m_socketHost, m_socketPort );
        serverSocket->bind();
        serverSocket->listen();
		

		std::string log = "Radio Simulator listening on " + m_socketHost + ":" + m_socketPort;
		m_modelLogger->setLogView(log);
    }
    catch ( ... )
    {
        std::cerr << "Radio Simulator listener thread caught exception during the construction of socket at address " << m_socketHost << ":" 
                  << m_socketPort << ", and will be terminated." << std::endl;
        //m_doThreadTerminate = true;
    }

    // single-threaded connection-at-a-time
    while( false == m_doThreadTerminate)
    {
        try
        {
			// See if we have any connections waiting to be accepted.
            
            TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket >* acceptSocket = serverSocket->accept(TA_Base_Core::SERVER_NON_BLOCKING_ACCEPT);

			
		
            if ( 0 != acceptSocket )
            {
                m_isConnected = true;
            
				m_modelLogger->setLogView("Client Connected..");

                RadioCommsObserver* com = new RadioCommsObserver(acceptSocket, m_modelLogger);

				com->registerProtocolHelper(m_motorolaSDSProtocolHelper);
				m_motorolaSDSProtocolHelper->registerSocketSendData(com);

				addCommsObserver(com);

				m_pecCommandProcessor->setDataSender(com);
				m_paCommandProcessor->setDataSender(com);
				m_ttisCommandProcessor->setDataSender(com);
				m_changeCommandProcessor->setDataSender(com);

                //Start sending HeartBeat when connected to client
				startSendingHeartBeat();

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "New Connection accepted on socket." );
				m_modelLogger->setLogView("New Connection accepted on socket.");
            }

			// we should check to see if all of our Observed Sockets are still connected.
			removeDisconnectedCommsObserver();

			// Check Heart Beat Status
			checkHeartBeatStatus();

        }
        catch ( ... )
        {
            m_threadIsRunning = false;
            throw;
        }

        sleep(500);
    }

	delete serverSocket;
	serverSocket = NULL;

	m_threadIsRunning = false;
    //m_doThreadTerminate = false;

	m_modelLogger->setLogView("Radio Simulator listener thread has exited");

    FUNCTION_EXIT;

}

void RadioSimulator::checkHeartBeatStatus()
{
	HeartBeatProcessorData::iterator iter = m_HeartBeatProcessors.begin();

	for ( iter; iter != m_HeartBeatProcessors.end(); iter++)
	{
		{
			TA_Base_Core::ThreadGuard guard(*(*iter));

			if ( NULL != (*iter) )
			{
				TrainHeartBeatInfo _info = (*iter)->getHeartBeatInfo();

				std::vector<int>::iterator iterTrain = std::find(m_heartBeatInfo.TrainID.begin(),
					m_heartBeatInfo.TrainID.end(),
					_info.TrainID);

				if ( iterTrain != m_heartBeatInfo.TrainID.end())
				{
					size_t index = std::distance(m_heartBeatInfo.TrainID.begin(),iterTrain);

					TA_Base_Core::ThreadGuard guard(m_heartBeatStatusLock);
					m_heartBeatInfo.TrainStatus.at(index) = _info.StatusInfo;

				}

			}
		}
			
	}

	//m_modelLogger->setHeartBeatView(m_heartBeatInfo);
}

HeartBeatInfo RadioSimulator::getTrainHeartBeatInfo() 
{
	TA_Base_Core::ThreadGuard guard(m_heartBeatStatusLock);
	return m_heartBeatInfo ;
}

int RadioSimulator::getNumberOfConnection() const
{
	return m_commsObservers.size();
}

void RadioSimulator::removeDisconnectedCommsObserver()
{
	std::vector< RadioCommsObserver*>::iterator iter = m_commsObservers.begin();
	bool someDeleted;
	RadioCommsObserver* com;

	do 
	{
		someDeleted = false;

		for( iter; iter != m_commsObservers.end(); iter++ )
		{
			if ( false == (*iter)->stillConnected() )
			{
				m_modelLogger->setLogView("CommsObserver removed from vector");

				//Stop sending HeartBeat when disconnected to client
				stopSendingHeartBeat();

				com = (*iter);
				m_commsObservers.erase(iter);
				delete com;
				com = NULL;
				someDeleted = true;

				iter = m_commsObservers.end();

				m_regStatus = false;
				break;
			}
		}
	} while (someDeleted);
	
}


//
// terminate()
//
void RadioSimulator::terminate()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RadioSimulator::terminate()");
	m_modelLogger->setLogView("RadioSimulator::terminate()");

	m_doThreadTerminate = true;
	m_regStatus = false;

	m_pecCommandProcessor->terminateAndWait();
	m_paCommandProcessor->terminateAndWait();
	m_ttisCommandProcessor->terminateAndWait();
	m_changeCommandProcessor->terminateAndWait();

	//stop sending HeartBeat
	stopSendingHeartBeat();
}

//////////////////////////////////////////////////////////////////
//                                                                //
//                    ISimulationModule stuff                        //
//                                                                //
//////////////////////////////////////////////////////////////////

//
// startSim()
//
void RadioSimulator::startSim()
{
	if ( true == m_threadIsRunning )
	{
		m_modelLogger->setLogView("Train Simulator is already running..");
		return;
	}

	// start the listener thread.
    m_doThreadTerminate = false;
    start();

	m_ttisCommandProcessor->start();
	m_paCommandProcessor->start();
	m_pecCommandProcessor->start();
	m_changeCommandProcessor->start();


    while( false == m_threadIsRunning )
    {
        sleep(100);
    }

	
}

//
// stopSim()
//
void RadioSimulator::stopSim()
{

	std::string log = getSimDescription() + "=" + getName() + "stopped";
	m_modelLogger->setLogView(log);

    // it should already be dead but this is a good cleanup
    terminateAndWait();
	removeAllCommsObservers();
	

// 	m_pecCommandProcessor->terminateAndWait();
// 	m_paCommandProcessor->terminateAndWait();
// 	m_ttisCommandProcessor->terminateAndWait();
}

//
// restartSim()
//
void RadioSimulator::restartSim()
{
    stopSim();
    startSim();
}

//
// destroySim()
//
void RadioSimulator::destroySim()
{
    delete this;
}

//
// getName()
//
std::string RadioSimulator::getName()
{
    return m_name;
}

//
// getSimDescription()
//
std::string RadioSimulator::getSimDescription()
{
    return std::string( "RADIO simulator" );
}

//
// getCommandProcessor()
//
TA_Base_Bus::ISimulationCommandProcessor& RadioSimulator::getCommandProcessor()
{
    return *m_commandProcessor;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                                Execute methods                           //
//TODO : create Radio specific execute methods  user input methods                                                                     //
//////////////////////////////////////////////////////////////////////////////
bool RadioSimulator::execPassnumber_check(bool bEnable)
{
    std::vector< TA_IRS_App::RadioCommsObserver* >::iterator iter = m_commsObservers.begin();
    for ( ; iter != m_commsObservers.end() ; iter++ )
    {
        (*iter)->execPassnumber_check(bEnable);
    }
    return true;
}

bool RadioSimulator::execAddISSI(const std::string& cmdValue)
{
	//return RadioCommsObserver::execAddISSI(cmdValue);
	return m_motorolaSDSProtocolHelper->execAddISSI(cmdValue);

}

bool RadioSimulator::execListISSIPassNum()
{
	//RadioCommsObserver::execListISSIPassNum();

	return m_motorolaSDSProtocolHelper->execListISSIPassNum();

	//return true;
}

bool RadioSimulator::execListDeliveryStatusOptions()
{
	std::cout << "+-----------------------------------------+" << std::endl;
    std::cout << "| SDTS-CM-DATA Delivery Status  |  Code   |" << std::endl;
    std::cout << "+-----------------------------------------+" << std::endl << std::endl;
	
	std::cout << "Successfully Delivered" << "\t\t\t\t\t\t" << "0" << std::endl;
	std::cout << "Network Overload" << "\t\t\t\t\t\t" << "64" << std::endl;
	std::cout << "Service disabled" << "\t\t\t\t\t\t" << "65" << std::endl;
	std::cout << "Service temporary not avaiable" << "\t\t\t\t\t" << "66" << std::endl;
	std::cout << "Desination is not authorised" << "\t\t\t\t\t" << "68" << std::endl;
	std::cout << "Unknown destination address" << "\t\t\t\t\t" << "69" << std::endl;
	std::cout << "Group address with individual service" << "\t\t\t\t" << "71" << std::endl;
	std::cout << "Not reachable" << "\t\t\t\t\t\t\t" << "74" << std::endl;
	std::cout << "Not registered" << "\t\t\t\t\t\t\t" << "75" << std::endl;
	std::cout << "Destination queue full" << "\t\t\t\t\t\t" << "76" << std::endl;
	std::cout << "Message too long" << "\t\t\t\t\t\t" << "77" << std::endl;
	std::cout << "Destination not support SDS-TL data transfer service PDUs" << "\t" << "78" << std::endl;
	std::cout << "Destination not connected" << "\t\t\t\t\t" << "79" << std::endl;
	std::cout << "Protocol not supported" << "\t\t\t\t\t\t" << "80" << std::endl;
	std::cout << "Destination not accepting SDS message" << "\t\t\t\t" << "83" << std::endl;
	std::cout << "Destination address administratively prohibited" << "\t\t\t" << "86" << std::endl;
	std::cout << "Unknown ecternal subscriber number" << "\t\t\t\t" << "88" << std::endl;
	std::cout << "Destination not reachable" << "\t\t\t\t\t" << "90" << std::endl;
	std::cout << "Text distribution error" << "\t\t\t\t\t\t" << "141" << std::endl;
	std::cout << "Corrupt Information Element, Message discarded" << "\t\t\t" << "142" << std::endl << std::endl;
	

	return true;
}

bool RadioSimulator::execDeliveryStatus(const std::string& cmdValue)
{
	bool status = true;
	std::vector< TA_IRS_App::RadioCommsObserver* >::iterator iter = m_commsObservers.begin();
    for ( ; iter != m_commsObservers.end() ; iter++ )
    {
        status = (*iter)->execDeliveryStatus(cmdValue);

		if(!status)
			return false;

    }
	return true;
}

bool RadioSimulator::execSetCmdata(const std::string& cmdValue)
{
	std::vector< TA_IRS_App::RadioCommsObserver* >::iterator iter = m_commsObservers.begin();
    for ( ; iter != m_commsObservers.end() ; iter++ )
    {
        (*iter)->RadioCommsObserver::execSetCmdata(cmdValue);
    }
    return true;
}

bool RadioSimulator::execSendCmdata()
{
	std::vector< TA_IRS_App::RadioCommsObserver* >::iterator iter = m_commsObservers.begin();
    for ( ; iter != m_commsObservers.end() ; iter++ )
    {
        (*iter)->RadioCommsObserver::execSendCmdata();
    }
    return true;
}

bool RadioSimulator::execShowMessage(const std::string& params) 
{

	std::vector< TA_IRS_App::RadioCommsObserver* >::iterator iter = m_commsObservers.begin();
    for ( ; iter != m_commsObservers.end() ; iter++ )
    {
        (*iter)->RadioCommsObserver::execShowMessage(params);
    }
    return true;
}

//
//execStatus
//
bool RadioSimulator::execStatus()
{
    int numConnections = m_commsObservers.size();
    std::cout << std::endl;
    std::cout << "Radio Simulator Status. Number of Connections=" << numConnections << std::endl << std::endl;
    if (numConnections < 1) return true;

    std::cout << "+-----------+-----------+---------------------" << std::endl;
    std::cout << "| socket id | connected | client name         " << std::endl;
    std::cout << "+-----------+-----------+---------------------" << std::endl;
    std::vector< TA_IRS_App::RadioCommsObserver* >::iterator iter = m_commsObservers.begin();
    for ( ; iter != m_commsObservers.end() ; iter++ )
    {
        TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket> * socket = (*iter)->getSocket();
        char buffStr[16] = {0};
        sprintf(buffStr, "%.7d", socket->getSocketId());
        std::string socketIdStr=buffStr;
        std::string connectedStr = socket->stillConnected() ? "true" : "false" ;
        std::string cname = socket->getClientName();
        std::cout << "|  " << socketIdStr.c_str() << "  |   " << connectedStr.c_str() << "   | " << cname.c_str() << std::endl;
    }

    return true;

}

bool RadioSimulator::execLoadTable(std::string param)
{
	
    if (param.empty())
    {
        std::cerr << "Error: no filename provided" << std::endl;
        return false;
    }

    // parse parameters
    std::stringstream s;
    s << param << std::ends;
    std::string filename;
    s >> filename;
    /// @todo Add second parameter to indicate which observer, if not present, load in all

    try
    {
		SimulatorHelper::getInstance().loadTable(filename);
#if 0
        SimulatorParser p(filename);
        p.parse();
        for (std::vector<RadioCommsObserver*>::iterator i = m_commsObserverVector.begin(); 
            i != m_commsObserverVector.end(); ++i) 
        {
            SimulatorUseCaseList& x = p.getUseCases();
            std::copy(x.begin(), x.end(), std::back_inserter((*i)->simulatorUseCasesRef()));
        }
#endif
    }
    catch (...)
    {
        std::cout << "Error: could not load [" << filename << "]" << std::endl;
    }

    return true;
}

bool RadioSimulator::execClearTable()
{	
	SimulatorHelper::getInstance().clearTable();
#if 0
    for (std::vector<RadioCommsObserver*>::iterator i = m_commsObserverVector.begin(); 
        i != m_commsObserverVector.end(); ++i) 
    {
        (*i)->simulatorUseCasesRef().clear();
    }
#endif    
		return true;
}

bool RadioSimulator::execShowTable(std::string params)
{
    std::cout << "param: " << params << " " << params.size() << std::endl;
    if (params.empty())
    {
		std::cout << "params are empty" << std::endl;
		SimulatorHelper::getInstance().showTable();
#if 0
        if (m_commsObserverVector.size() == 0)
        {
            std::cout << "Error: no observers..." << std::endl;
            return false;
        }
        std::cout << "Obs\tUC ID\tUC name" << std::endl;
        for (unsigned int obs = 0; obs < m_commsObserverVector.size(); ++obs) 
        {
            SimulatorUseCaseList& ucList = m_commsObserverVector[obs]->simulatorUseCasesRef();
            for (unsigned int uc = 0; uc < ucList.size(); ++uc)
            {
                std::cout << obs << "\t" << uc << "\t" << ucList[uc].name << std::endl;
            }
        }
#endif
    }
    else 
    {
        // parse parameters
        std::stringstream s;
        s << params << std::ends;
        unsigned int which;
        s >> which;
		
		SimulatorHelper::getInstance().showEntry(which);
#if 0
        if (m_commsObserverVector.size() == 0 ||
            (*m_commsObserverVector.begin())->simulatorUseCasesRef().size() == 0 ||
            (*m_commsObserverVector.begin())->simulatorUseCasesRef().size() < which )
        { 
            std::cout << "Error: nothing to do, either no observers, or no use cases loaded" << std::endl;
            return false;
        }
        std::cout << (*m_commsObserverVector.begin())->simulatorUseCasesRef()[which] << std::endl;
#endif
    }
	return true;
}

bool RadioSimulator::execRun(std::string params)
{
    /// @todo add additional parameter to send to different observers. eg. runsim <obs> <id>
    // Take the id; find it in the UCList; and send message (delegate to observer)
    
    if (params.empty())
    {
        std::cerr << "Error: no entry specified" << std::endl;
        return false;
    }

    // parse parameters
    std::stringstream s;
    s << params << std::ends;
    unsigned int which;
    s >> which;

    if (m_commsObservers.size() == 0)
    { 
        std::cout << "Error: no observers..." << std::endl;
        return false;
    }
	if ( which >= SimulatorHelper::getInstance().getUseCaseListRef().size() )
	{
		std::cout << "Error: no entry [" << which << "]" << std::endl;
		return false;
	}
	simulatorRunUseCase( SimulatorHelper::getInstance().getUseCaseListRef()[which] );
/*
#if 0
    if (m_commsObserverVector.size() == 0 ||
        (*m_commsObserverVector.begin())->simulatorUseCasesRef().size() == 0 ||
        (*m_commsObserverVector.begin())->simulatorUseCasesRef().size() < which )
    { 
        std::cout << "Error: nothing to do, either no observers, or no use cases loaded" << std::endl;
        return false;
    }

    ///@todo call something in RadioCommsObserver...
    simulatorRunUseCase( (*m_commsObserverVector.begin())->simulatorUseCasesRef()[which] );
#endif
*/
    return true;
}

bool RadioSimulator::execSetTtsiVer(const std::string& params)
{   
	if (params.empty())
    {
        std::cerr << "Error: no entry specified" << std::endl;
        return false;
    }
	    
	if (m_commsObservers.size() == 0)
    { 
        std::cout << "Error: no observers..." << std::endl;
        return false;
    }

	if (params.length() < 8)
    {
        std::cerr << "Error: Minimum 8-length version number needed" << std::endl;
        return false;
    }

	std::vector< TA_IRS_App::RadioCommsObserver* >::iterator iter = m_commsObservers.begin();
    for ( ; iter != m_commsObservers.end() ; iter++ )
    {
        (*iter)->RadioCommsObserver::setTtsiLibVer(params);
    }
	return true;
}

/// @todo Remove duplication of methods (see RadioCommsObserver)
void RadioSimulator::simulatorRunUseCase(TA_IRS_Bus::SimulatorUseCase& uc)
{
	std::vector<unsigned char> cmHeader;
	std::vector<unsigned char> tlLength;
	std::string tlHeader = "";
	std::string userData = "";

	CMHelper::buildCMheader(uc.cmData, cmHeader);
	tlLength = CMHelper::encode_u16(uc.cmData.getTLlength());
	tlHeader = uc.cmData.getTLheader();
	userData = uc.cmData.getUserData();

	
	std::string cmData = tlHeader + userData;
	std::vector<unsigned char> msg;

	// 2. Convert input string to vector:
	unsigned int code = 0;
	unsigned int i = 0;
	int counter = 0;
	msg.clear();

	//printf("String size = %i\n\n", str.size());
	msg.insert(msg.end(),cmHeader.begin(),cmHeader.end());
	msg.insert(msg.end(),tlLength.begin(),tlLength.end());
	for (unsigned int index=0;index<cmData.size();index++)
	{
		char   character = cmData[index];
		unsigned int pos = 1 - index%2;
		i = CMHelper::Hex2Int(character, pos);

		if(index>0 && (index%2)==0)
			code = 0;

		code += i;

		if(index>0 && (index%2)==1)
		{
			std::vector<unsigned char> ret;
			ret.push_back((code & 0xFF));
			msg.insert( msg.end(), ret.begin(), ret.end() );
		}
	}

	std::vector<unsigned char> crcData;

	crcData.insert(crcData.end(), msg.begin()+RadioCommsObserver::USER_DATA_INDEX-1, msg.end());//,msg[msg.size()-1]);


	std::vector<unsigned char>::iterator a = crcData.begin();
	for (;a!=crcData.end();a++) {
		printf("%02x ", (*a));
	}
	unsigned int calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(crcData,crcData.size());
	std::cout << "CRC = " << std::hex << calcCRC << std::endl;
	crcData.clear();
	crcData = CMHelper::encode_u16(calcCRC);
	//msg.insert(msg.end(), crcData.at(1));
	//msg.insert(msg.end(), crcData.at(0));
	
	msg.insert(msg.end(), crcData.at(1));
	msg.insert(msg.end(), crcData.at(0));

	int x = msg.size() - 2;
	msg[0] = (x & 0xFF00) >> 8;
	msg[1] = x & 0xFF;
	
	
	std::vector< TA_IRS_App::RadioCommsObserver* >::iterator iter = m_commsObservers.begin();
    for ( ; iter != m_commsObservers.end() ; iter++ )
    {
        (*iter)->RadioCommsObserver::sendData(msg);
    }

	printf("\n");
	/*for(unsigned int j=0; j<msg.size(); j++)
	{
		printf("%2x ", msg[j]);
	}*/
	//std::ostringstream data;
	int c = 0;
	std::vector<unsigned char>::const_iterator iter1;
	
	for(iter1=msg.begin();iter1!=msg.end();iter1++)
	{
		//data << std::setw(2) << std::setfill('0') << (int)(*iter1) << ' ';

		printf("%02x ", (*iter1));
		if (c == 16)
		{
			printf("\n");
			c = 0;
		}
		else
			c++;
	}
	//printf("%s", data.str().c_str());
	printf("\n");





	/*
    if (uc.resp.size() == 0)
    {
        std::cout << "Debug: No response defined for [" << uc.name << "]" << std::endl;
		std::cout << "Error: Cannot run [" << uc.name << "] it is NOT a valid rule to run from this command, check you are executing the correct rule...." << std::endl; 
		return;
    }
	// quick and dirty way of preventing obscure errors with mapping of vars, when a user tries to run a rule expecting input data and mapping vars
	if (uc.mask.size() != 0)
	{
        std::cout << "Debug: An input mask has been defined for [" << uc.name << "], cannot generate response." << std::endl;
		std::cout << "Error: Cannot run [" << uc.name << "] it is NOT a valid rule to run from this command, check you are executing the correct rule...." << std::endl; 
		return;
	}*/

	//    std::cout << uc << std::endl;

    //SimulatorHelper::getInstance().sendMessage( uc, *this);
/*
#if 0
    /// @todo Handle generated output vars...

    for (unsigned int respNo = 0 ; respNo < uc.resp.size(); ++respNo)
    {
        std::cout << "Waiting " << uc.delay[respNo] << "ms...\r";
        sleep(uc.delay[respNo]); // wait before sending

        const unsigned long IncomingSDSLength = 92+189;  
        const unsigned long IncomingSDSEvent  = 0xA002;
        const unsigned long StatusOK          = 0;
        
        char   buffer[IncomingSDSLength];
        
        // Just copy the response as an initial starting point, then tweak it later...
        std::fill(&buffer[respNo],&buffer[IncomingSDSLength],0);
        std::copy(uc.resp[respNo].begin(), uc.resp[respNo].end(), buffer);

        std::cout << "Sending [" << respNo << "][" << uc.name << "]" << std::endl;

        // Send out to session observers
        sendFromAllCommsObserver(buffer,sizeof(buffer));

    }
#endif*/
}


void RadioSimulator::execSendData(std::vector<unsigned char> &v)
{std::vector< TA_IRS_App::RadioCommsObserver* >::iterator iter = m_commsObservers.begin();
for ( ; iter != m_commsObservers.end() ; iter++ )
{
	(*iter)->RadioCommsObserver::sendData(v);
}
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                                Simulator methods                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


//
//getCommsObserverVector
//
std::vector< RadioCommsObserver * >& RadioSimulator::getCommsObserverVector()
{ 
    return m_commsObservers; 
}

//
//addCommsObserver
//
// @param com the CommsObserver to add
//
void RadioSimulator::addCommsObserver(RadioCommsObserver* com)
{

    std::cout << "New ISCS connected to " << m_name << " simulator" << std::endl;
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "New ISCS connected to %s simulator", m_name.c_str());
    m_commsObservers.push_back(com);
	
	char log[1024] = {0};
	std::sprintf(log,"New ISCS connected to %s simulator",m_name.c_str());
	m_modelLogger->setLogView(log);

}

//
// removeCommsObserver removes the given 
//
// @param com the CommsObserver to remove
//
void RadioSimulator::removeCommsObserver(RadioCommsObserver* com)
{
    std::cout << "ISCS disconnected from " << m_name << " simulator" << std::endl;
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ISCS disconnected from %s simulator", m_name.c_str());

	char log[1024] = {0};
	std::sprintf(log,"ISCS disconnected from %s simulator",m_name.c_str());
	m_modelLogger->setLogView(log);

    std::vector< RadioCommsObserver * >::iterator it = std::find(m_commsObservers.begin(), m_commsObservers.end(), com);
    if( it != m_commsObservers.end() )
    {
        //remove the CommsObserver
		if ( NULL != (*it))
		{
			delete (*it);
			(*it) = NULL;
		}
        m_commsObservers.erase(it);
    }
}

void RadioSimulator::removeAllCommsObservers()
{
	std::vector<RadioCommsObserver*>::iterator iter = m_commsObservers.begin();
	for ( iter; iter != m_commsObservers.end(); iter++ )
	{
		delete (*iter);
		(*iter) = NULL;
	}
	
	char log[1024];
	std::sprintf(log,"ISCS disconnected from %s simulator",m_name.c_str());
	m_modelLogger->setLogView(log);

	m_commsObservers.clear();
	
}

void RadioSimulator::addHeartBeatProcessor(TA_IRS_App::HeartBeatCommandProcessor *cmdProcessor)
{
	FUNCTION_ENTRY("addHeartBeatProcessor");

	HeartBeatProcessorData::iterator iter = std::find( m_HeartBeatProcessors.begin(),
		                                               m_HeartBeatProcessors.end(),
													   cmdProcessor);
	if ( iter == m_HeartBeatProcessors.end())
	{
		
		//Insert the new HeartBeat Processor object
		m_HeartBeatProcessors.push_back(cmdProcessor);

		TrainHeartBeatInfo _info = cmdProcessor->getHeartBeatInfo();

		std::vector<int>::iterator iterTrain = std::find(m_heartBeatInfo.TrainID.begin(),
			m_heartBeatInfo.TrainID.end(),
			_info.TrainID);

		//Add the new Train is the list
		if ( iterTrain == m_heartBeatInfo.TrainID.end())
		{
			//TA_Base_Core::ThreadGuard guard(m_heartBeatStatusLock);

			m_heartBeatInfo.TrainID.push_back(_info.TrainID);
			m_heartBeatInfo.TimeDelay.push_back(_info.DelayTime);
			m_heartBeatInfo.TrainStatus.push_back(_info.StatusInfo);
		}

		
		startSendingHeartBeat(cmdProcessor);
	}
	
	FUNCTION_EXIT;
}

void RadioSimulator::removeHeartBeatProcessor(TA_IRS_App::HeartBeatCommandProcessor *cmdProcessor)
{
	FUNCTION_ENTRY("removeHeartBeatProcessor");

	HeartBeatProcessorData::iterator iter = std::find( m_HeartBeatProcessors.begin(),
		                                               m_HeartBeatProcessors.end(),
													   cmdProcessor);
	
	{
		TA_Base_Core::ThreadGuard guard(*(*iter));

		if ( iter != m_HeartBeatProcessors.end())
		{
			m_HeartBeatProcessors.erase(iter);
			if ( NULL != *iter )
			{
				delete *iter;
				*iter = NULL;
			}
		}
	}

	FUNCTION_EXIT;
}

void RadioSimulator::initializeTrainHeartBeat()
{
	std::vector<int> TrainsList = SimulatorConfig::getInstance()->getTrainsListHeartBeat();
	int heartBeatDelay = SimulatorConfig::getInstance()->getHeartBeatDelay();

	TrainHeartBeatInfo _heartBeatInfo;

	if ( 0 != TrainsList.at(0) )
	{
		for (int count = 0; count < TrainsList.size(); count++ )
		{
			_heartBeatInfo.TrainID = TrainsList.at(count);
			_heartBeatInfo.DelayTime = heartBeatDelay;
			addHeartBeatProcessor(new TA_IRS_App::HeartBeatCommandProcessor(_heartBeatInfo,getInstanceMotorolaHelper()));
		}
	}
		
	
	
}

void RadioSimulator::stopSendingHeartBeat()
{
	HeartBeatProcessorData::iterator iter = m_HeartBeatProcessors.begin();
	for (iter; iter != m_HeartBeatProcessors.end(); iter++ )
	{
		{
			TA_Base_Core::ThreadGuard guard(*(*iter));

			if ( Thread::THREAD_STATE_RUNNING == (*iter)->getCurrentState())
			{
				(*iter)->terminateAndWait();
			}
		}
		
		
	}

	//Check HeartBeat Information
	checkHeartBeatStatus();
}

void RadioSimulator::startSendingHeartBeat()
{
	//Check if the thread state is already running
	if ( Thread::THREAD_STATE_RUNNING == getCurrentState() && 
		(true == checkCommObserverStatus()) && (true == m_regStatus))
	{
		HeartBeatProcessorData::iterator iter = m_HeartBeatProcessors.begin();
		for (iter; iter != m_HeartBeatProcessors.end(); iter++ )
		{
			{
				TA_Base_Core::ThreadGuard guard(*(*iter));

				if ( Thread::THREAD_STATE_RUNNING != (*iter)->getCurrentState())
				{
					(*iter)->registerDataSender(m_commsObservers.at(0));
					(*iter)->start();
				}
				TA_Base_Core::Thread::sleep(100);

			}
			
		}

		//Check HeartBeat Information
		checkHeartBeatStatus();
	}
	
}

void RadioSimulator::startSendingHeartBeat(TA_IRS_App::HeartBeatCommandProcessor* cmdProcessor)
{
	//Check if the thread state is already running
	if ( Thread::THREAD_STATE_RUNNING == getCurrentState() && (true == checkCommObserverStatus()))
	{
		HeartBeatProcessorData::iterator iter = std::find(m_HeartBeatProcessors.begin(),
														  m_HeartBeatProcessors.end(),
														  cmdProcessor);
		if ( iter != m_HeartBeatProcessors.end())
		{
			{
				TA_Base_Core::ThreadGuard guard(*(*iter));

				if ( Thread::THREAD_STATE_RUNNING != (*iter)->getCurrentState())
				{
					(*iter)->registerDataSender(m_commsObservers.at(0));
					(*iter)->start();
				}
				TA_Base_Core::Thread::sleep(100);
			}
		}

	}
}

void RadioSimulator::addTrainHeartBeat(int trainID, int delay)
{
	TrainHeartBeatInfo _trainInfo;
	_trainInfo.TrainID = trainID;
	_trainInfo.DelayTime = delay;
	_trainInfo.StatusInfo = "Stopped";
	addHeartBeatProcessor(new TA_IRS_App::HeartBeatCommandProcessor(_trainInfo,getInstanceMotorolaHelper()));
}

int RadioSimulator::getCommsObserverStatus()
{
	int _ret = 0;
	if ( m_commsObservers.size() > 0)
	{
		_ret = 1;
	}
	return _ret;
}

void RadioSimulator::sendStatusTrainHeartBeat(TrainStatus stat, int trainID)
{
	
	HeartBeatProcessorData::iterator iter = m_HeartBeatProcessors.begin();
	bool flag = false;
	for (iter; iter != m_HeartBeatProcessors.end(); iter++ )
	{

		{
			//TA_Base_Core::ThreadGuard guard(*(*iter));

			TrainHeartBeatInfo _info = (*iter)->getHeartBeatInfo();

			if ( _info.TrainID == trainID )
			{
				switch (stat)
				{
				case TrainStatus::REMOVE:
					{
						//(*iter)->terminateAndWait();						
						delete (*iter);
 						(*iter) = NULL;
						m_HeartBeatProcessors.erase(iter);
						removeHeartBeatInfo(trainID);
						flag = true;
						break;
					}

				case TrainStatus::START:
					(*iter)->start();
					flag = true;
					break;

				case TrainStatus::STOP:
					(*iter)->terminateAndWait();
					flag = true;
					break;
				}

			}

			if ( true == flag)
			{
				checkHeartBeatStatus();
				return;
			}

		}
		
	}

}

void RadioSimulator::removeHeartBeatInfo(int trainID)
{
	std::vector<int>::iterator iter = std::find(m_heartBeatInfo.TrainID.begin(),
											    m_heartBeatInfo.TrainID.end(),
												trainID );

	if ( iter != m_heartBeatInfo.TrainID.end())
	{
		size_t item = std::distance(m_heartBeatInfo.TrainID.begin(),iter);
		//remove the data
		m_heartBeatInfo.TrainID.erase(m_heartBeatInfo.TrainID.begin() + item );
		m_heartBeatInfo.TimeDelay.erase(m_heartBeatInfo.TimeDelay.begin() + item );
		m_heartBeatInfo.TrainStatus.erase(m_heartBeatInfo.TrainStatus.begin() + item );
	}
	

}

std::vector<TA_IRS_App::HeartBeatCommandProcessor*> RadioSimulator::getHeartBeatProcessor()const
{
	return m_HeartBeatProcessors;
}

void RadioSimulator::addTrainResponseStrategy(const TrainStrategyInfo &info)
{
	m_pecCommandProcessor->addTrainResponseStrategy(info);
	m_paCommandProcessor->addTrainResponseStrategy(info);
	m_ttisCommandProcessor->addTrainResponseStrategy(info);
	m_changeCommandProcessor->addTrainResponseStrategy(info);
}

std::map<int, TrainStrategyInfo> RadioSimulator::getTrainStrategyMap()
{
	return m_pecCommandProcessor->getTrainStrategyMap();
}

void RadioSimulator::removeTrainResponseStrategy(int trainID)
{
	m_pecCommandProcessor->removeTrainResponseStrategy(trainID);
	m_paCommandProcessor->removeTrainResponseStrategy(trainID);
	m_ttisCommandProcessor->removeTrainResponseStrategy(trainID);
	m_changeCommandProcessor->removeTrainResponseStrategy(trainID);
}

bool RadioSimulator::checkCommObserverStatus()
{
	bool hasInstance = false;
	std::vector<TA_IRS_App::RadioCommsObserver*>::iterator iter = m_commsObservers.begin();
	for ( iter; iter != m_commsObservers.end(); iter++)
	{
		if ( NULL != *iter )
		{
			hasInstance = true;
			break;
		}
	}
	return hasInstance;
}

bool RadioSimulator::sendTIMSCommand(CommandType cmdType, int status, const std::vector<int>& trains, bool bIsPrimary)
{
	FUNCTION_ENTRY("sendTIMSCommand");

	TA_ASSERT( NULL != m_threadPool, "The threadpool is NULL");

	FUNCTION_EXIT;

	//Check if the status is connected
	if ( false == m_regStatus)
	{
		return false;
	}

	sTimsCmdData _tmsData;
	_tmsData.CmdType = cmdType;
	_tmsData.Status = status;
	_tmsData.isPrimaryTsi = bIsPrimary;

	std::vector<int>::const_iterator iter = trains.begin();
	for ( iter; iter != trains.end(); iter++ )
	{
		_tmsData.TrainID = *iter;

		TA_Base_Core::IWorkItemPtr workItem( new RadioSimTask(*this, _tmsData) );
		m_threadPool->queueWorkItem(workItem);

		//Just put a little bit delay so that the processor won't be high
		TA_Base_Core::Thread::sleep(100);
	}

	return true;
}

TimsCommand* RadioSimulator::getTimsCommandHandler(CommandType cmdType)
{
	switch(cmdType)
	{
	case END_OF_TTIS_DOWNLOAD:
		return m_endOfTtisDownload;
		break;

	case CHANGEOVER_STATUS:
		return m_changeoverStatus;
		break;

	case REQUEST_FOR_OCC_CALL:
		return m_requestOccCall;

	case TEST_CALL:
		return m_testCallCommand;

	default:
		return NULL;
	}
}

void RadioSimulator::processTimsCommand(sTimsCmdData data )
{
	TimsCommand* tmsCmnd = getTimsCommandHandler(data.CmdType);

	if ( NULL == tmsCmnd)
		return;

	tmsCmnd->setStatusID(data.Status);
	tmsCmnd->setTrainID(data.TrainID);
	

	m_modelLogger->setLogView(tmsCmnd->getCommandLog());
	m_motorolaSDSProtocolHelper->buildMotorolaHeadersTIMSCommand(tmsCmnd->getCommandData(),data.isPrimaryTsi);
}

void RadioSimulator::updateRegStatus(bool status)
{
	m_regStatus = status;
	//start sending heartbeat
	if ( true == status )
	{
		startSendingHeartBeat();
	}
	else
		stopSendingHeartBeat();
	
}


RadioSimTask::RadioSimTask(TA_IRS_App::ITIMSTrainsProcessor &callback, TA_IRS_App::sTimsCmdData data)
: m_callback(callback), m_data(data)
{
	FUNCTION_ENTRY("RadioSimTask");
	FUNCTION_EXIT;
}

RadioSimTask::~RadioSimTask()
{
	FUNCTION_ENTRY("~RadioSimTask");
	FUNCTION_EXIT;
}

void RadioSimTask::executeWorkItem()
{
	FUNCTION_ENTRY("executeWorkItem");
	
	m_callback.processTimsCommand(m_data);

	FUNCTION_EXIT;
}