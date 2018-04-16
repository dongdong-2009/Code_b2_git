
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


#include "app/radio/radio_simulator/src/CMHelper.h"
#include "app/radio/radio_simulator/src/RadioSimulator.h"
#include "app/radio/radio_simulator/src/RadioSimulatorCommandProcessor.h"
#include "app/radio/radio_simulator/src/RadioCommsObserver.h"
#include "SimulatorHelper.h"

#include "bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h"


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"


using namespace TA_IRS_App;
using namespace TA_IRS_Bus;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RadioSimulator::RadioSimulator(const std::string &name,const char* host,const char* port)
: m_name(name)
, m_threadIsRunning(false)
, m_doThreadTerminate(false)
, m_socketPort(port)
, m_socketHost(host)
, m_commandProcessor(0)
{
    m_commandProcessor = new RadioSimulatorCommandProcessor(this);
}

RadioSimulator::~RadioSimulator()
{
    stopSim();

}

//
// run()
//
// @todo something more useful in the catch(...) blocks
void RadioSimulator::run()
{

    FUNCTION_ENTRY("RadioSimulator::run");
    std::cout << "Radio Simulator thread has started" << std::endl;

    // the run method is really the guts of it all
    m_threadIsRunning = true;

    TA_Base_Core::TcpServerSocket<TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket> >* serverSocket = 0;
    try
    {
        serverSocket = new TA_Base_Core::TcpServerSocket<TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > >( m_socketHost, m_socketPort );
        serverSocket->bind();
        serverSocket->listen();
        std::cout   << "Radio Simulator listening on "
                    << m_socketHost << ":" << m_socketPort << std::endl;
    }
    catch ( ... )
    {
        std::cerr << "Radio Simulator listener thread caught exception during the construction of socket at address " << m_socketHost << ":" 
                  << m_socketPort << ", and will be terminated." << std::endl;
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
                m_isConnected = true;
            
                RadioCommsObserver* com = new RadioCommsObserver(acceptSocket);
                
                addCommsObserver(com);

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "New Connection accepted on socket." );
            }

            //we should check to see if all of our Observed Sockets are still connected.
            std::vector< TA_IRS_App::RadioCommsObserver * >::iterator it = m_commsObservers.begin();

            while(it != m_commsObservers.end())
            {
                if((*it)->stillConnected() == false)
                {
                    removeCommsObserver(*it);
                    delete *it;
                    //This removes the comms observer from the vector, and removes the socket
                    //from the vector.
                                        
                    it = m_commsObservers.begin();
                }
                else
                {
                    it++;
                }
            }
        }
        catch ( ... )
        {
            m_threadIsRunning = false;
            throw;
        }

        sleep(200);
    }

    m_threadIsRunning = false;

    FUNCTION_EXIT;

}

//
// terminate()
//
void RadioSimulator::terminate()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RadioSimulator::terminate()");
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
void RadioSimulator::stopSim()
{
    m_doThreadTerminate = true;
    
    while( m_threadIsRunning )
    {
        sleep(200);
    }

    std::cout << getSimDescription() << "=" << getName() << " stopped." << std::endl;
    //delete m_commandProcessor;

    // it should already be dead but this is a good cleanup
    terminateAndWait();
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
	return RadioCommsObserver::execAddISSI(cmdValue);

}

bool RadioSimulator::execListISSIPassNum()
{
	RadioCommsObserver::execListISSIPassNum();

	return true;
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
        char buffStr[16];
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

    std::vector< RadioCommsObserver * >::iterator it = std::find(m_commsObservers.begin(), m_commsObservers.end(), com);
    if( it != m_commsObservers.end() )
    {
        //remove the CommsObserver
        m_commsObservers.erase(it);
    }
}


