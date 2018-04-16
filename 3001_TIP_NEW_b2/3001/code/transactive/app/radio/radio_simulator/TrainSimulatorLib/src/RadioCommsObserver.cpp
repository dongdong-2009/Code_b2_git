/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/RadioCommsObserver.cpp $
  * @author:  LIU YU
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  * 
  * 
  * CommsObserver allows the CommsHandler to delegate incoming socket connections.
  * Each new connection is passed to a new CommsObserver object
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <map>

#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMHelper.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "app/radio/radio_simulator/TrainSimulatorLib/src/RadioCommsObserver.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/LivePaReponseThread.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/PreRecordedPaTimeOutThread.h"


#include "bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h"

#include "core/threads/src/ThreadPoolSingletonManager.h"


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "boost/shared_ptr.hpp"

using namespace TA_IRS_App;
using namespace TA_Base_Core;
using namespace std;

// Message format
static const int CM_PDUTYPE_INDEX = 2;
static const int ADAPTATION_LENGTH = 2;
//static const int TL_LENGTH = 2;

static const int CM_REGISTER_REQUEST_LENGTH = 11;
static const int CM_REG_PASSNUMBER_INDEX = 3;
static const int CM_REG_ISSI_INDEX = 7;
static const int CM_REG_REGISTRATIONTYPE_INDEX = 10;

//static const int CM_SDTS_BRAODCAST_LENGTH = 19;
static const int CM_SDTS_BROADCAST_TLLENGTH_INDEX = 17;

//static const int CM_SDTS_DATA_LENGTH = 15;
//static const int CM_SDTS_DATA_SRCADD_INDEX = 3;
//static const int CM_SDTS_DATA_DSTADD_INDEX = 6;
//static const int CM_SDTS_DATA_PI_INDEX = 9;
//static const int CM_SDTS_DATA_FLAGS_INDEX = 10;
//static const int CM_SDTS_DATA_MSGREF_INDEX = 11;
//static const int CM_SDTS_DATA_AREASEL_INDEX = 12;
//static const int CM_SDTS_DATA_TLLENGTH_INDEX = 13;

//static const int CM_SDTS_TL_UNITDATA_INDEX = 1;
//static const int CM_SDTS_TL_DATA_INDEX = 6;

const int RadioCommsObserver::USER_DATA_INDEX = 22;
//static const int USER_DATA_COMMAND_INDEX = 22;
//static const int USER_DATA_FLAG = 7;


// bool operator<(RadioIdentity a, RadioIdentity b)
// {
// 	return a.getSSI()<b.getSSI();
// }

std::map<RadioIdentity, std::string> radioIdMap;
std::map<RadioIdentity, std::string>::iterator radioId;


//////////////////////////////////////////////////////////////////////////////
//            Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

RadioCommsObserver::RadioCommsObserver(TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > * socket, IModelLogger* modelLogger )
:m_clientSocket(socket),
 m_stillConnected(true),
 m_passNumberCheck(true),
 m_cmDeliveryStatus( CMDELIVERYSTATUS_SUCCESS ),
 m_cmHeader("00160000045700045882600000"),
 m_tlHeader("008000000000"),
 m_ttisLibVer("00000000"),
 m_protocolHelper(NULL),
 m_modelLogger(modelLogger),
 m_threadPool( TA_Base_Core::ThreadPoolSingletonManager::getInstance())
 
{
    m_clientSocket->addObserver(* this);
    
    m_clientSocket->start();

	showMsg = true;
}

RadioCommsObserver::~RadioCommsObserver()
{
    FUNCTION_ENTRY("~RadioCommsObserver");
    if( 0 != m_clientSocket )
    {
        //terminate the thread the socket is holding.
        m_clientSocket->terminateAndWait();
        delete m_clientSocket;
    }

	TA_Base_Core::ThreadPoolSingletonManager::removeInstance( m_threadPool );
	m_threadPool = NULL;

    FUNCTION_EXIT;
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                            ITcpObserver methods                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
/**
  * processReceivedData
  *
  * @param pData the tcp data stream
  * @param pSocket the ISocket that received the data
  */

void RadioCommsObserver::processReceivedData( std::vector<unsigned char>&  pData, TA_Base_Core::ISocket * pSocket )
{
    FUNCTION_ENTRY("processReceivedData");
    // Log a debug warning message

	TA_ASSERT( NULL != m_threadPool,"The threadpool is NULL");

	// get all radio methods
	std::vector< IncomingPacketPtr > newPackets = extractPacketsFromRawData( pData );

	// queue each for processing
	for ( std::vector< IncomingPacketPtr >::iterator packetIter = newPackets.begin();
		newPackets.end() != packetIter; ++packetIter )
	{
		TA_Base_Core::IWorkItemPtr workItem( new RadioMethodTask( *this, *packetIter ) );

		m_threadPool->queueWorkItem( workItem );

		//Just put a little bit delay so that the processor won't be high
		TA_Base_Core::Thread::sleep(100);
	}  

    FUNCTION_EXIT;
}

std::vector< IncomingPacketPtr > RadioCommsObserver::extractPacketsFromRawData( std::vector<unsigned char>& pData )
{
	FUNCTION_ENTRY( "extractPacketsFromRawData" );

	unsigned long headerLength = 0;

	headerLength |= (pData[0] << 8) & 0xFF00;
	headerLength |= (pData[1] & 0xFF);

	unsigned long totalLength = headerLength + CRC_LENGTH;

	if ( pData.size() > 21 )
	{
		FUNCTION_EXIT;
		return getReceivedPackets(pData,totalLength);
	}
	else
	{
		FUNCTION_EXIT;
		return getReceivedPackets(pData,pData.size());
	}
	
}

std::vector< IncomingPacketPtr > RadioCommsObserver::getReceivedPackets(std::vector<unsigned char>& pData, unsigned long copyLength )
{
	FUNCTION_ENTRY("getNewPackets");

	std::vector< IncomingPacketPtr > newPackets;

	// copy the data to a local vector
	boost::shared_ptr< IncomingPacket > newPacket( new IncomingPacket() );
	for ( unsigned long count = 0; count < copyLength; count++)
	{
		//push the next element on
		newPacket->push_back(pData[0]);

		//remove the next element from the source
		pData.erase(pData.begin());
	}

	 newPackets.push_back(newPacket);

	 FUNCTION_EXIT;
	 return newPackets;
	
}

void RadioCommsObserver::registerProtocolHelper(TA_IRS_App::IProtocolHelper* protocolHelper)
{
	FUNCTION_ENTRY("registerProtocolHelper");

	m_protocolHelper = protocolHelper;

	FUNCTION_EXIT;
}


//
//writeFailed
//
void RadioCommsObserver::writeFailed()
{
    FUNCTION_ENTRY("writeFailed");
    //should probably throw some sort of exception.
    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Write failed!" );
    FUNCTION_EXIT;
}


//
//connectionLost
//
bool RadioCommsObserver::connectionLost()
{
    FUNCTION_ENTRY("writeFailed");
    m_stillConnected = false;
    // Do NOT call removeObserver. Its not threadsafe, and besides you don't
    // want to stop observing just because a connection is lost.
    // m_clientSocket->removeObserver ( * this );

    FUNCTION_EXIT;
    return false; // Tell observer to terminate, don't attempt re-connect
}


//
//connectionEstablished
//
void RadioCommsObserver::connectionEstablished()
{
    FUNCTION_ENTRY("connectionEstablished");
    m_stillConnected = true;
    FUNCTION_EXIT;
}

//
//sendData
//
void RadioCommsObserver::sendData( const std::vector<unsigned char> & pData)
{
	FUNCTION_ENTRY("sendData");

	std::ostringstream ouput;
	std::copy( pData.begin(), pData.end(), std::ostreambuf_iterator<char>(ouput));
	std::string strData = ouput.str();

	m_clientSocket->write(strData.c_str(),strData.length());

    FUNCTION_EXIT;
}

//
//displayData
//
void RadioCommsObserver::displayData(const std::vector<unsigned char>& pData)
{
	FUNCTION_ENTRY("displayData");

	/*
	std::vector<unsigned char>::const_iterator iter;

	std::string text;
	for ( iter = pData.begin(); iter = pData.end(); iter++ )
	{
		text += *iter;
	}

	std::cout << text << std::endl;
	*/

	for(unsigned int j=0; j<pData.size(); j++)
	{
		if (showMsg)
		printf("%c", pData[j]);
	}
	if (showMsg)
	printf("\n\n");

	FUNCTION_EXIT;
}

//
//execute methods
//
bool RadioCommsObserver::execPassnumber_check(bool bEnable)
{
	FUNCTION_ENTRY("execPassnumber_check");
	setPassNumberCheck(bEnable);


	FUNCTION_EXIT;
    return true;
}

bool RadioCommsObserver::execAddISSI(const std::string& cmdValue)
{
	FUNCTION_ENTRY("execAddISSI");
    FUNCTION_EXIT;

	return m_protocolHelper->execAddISSI(cmdValue);
}

bool RadioCommsObserver::execListISSIPassNum()
{
	FUNCTION_ENTRY("execListISSIPassNum");
	FUNCTION_EXIT;

	return m_protocolHelper->execListISSIPassNum();
}

bool RadioCommsObserver::execDeliveryStatus(const std::string& cmdValue)
{
	FUNCTION_ENTRY("execDeliveryStatus");

	unsigned int statusValue = atoi(cmdValue.c_str());
	CMDeliveryStatus status = CMDELIVERYSTATUS_SUCCESS;

	switch (statusValue & 0xFF)
	{
		case CMDELIVERYSTATUS_SUCCESS:
				status = CMDELIVERYSTATUS_SUCCESS;
				break;
		case CMDELIVERYSTATUS_NET_OVERLOAD:
				status = CMDELIVERYSTATUS_NET_OVERLOAD;
				break;
		case CMDELIVERYSTATUS_SERVICE_DISABLED:
				status = CMDELIVERYSTATUS_SERVICE_DISABLED;
				break;
		case CMDELIVERYSTATUS_SERVICE_TEMP_NAVAIL:
				status = CMDELIVERYSTATUS_SERVICE_TEMP_NAVAIL;
				break;
		case CMDELIVERYSTATUS_DEST_NAUTHORIZED:
				status = CMDELIVERYSTATUS_DEST_NAUTHORIZED;
				break;
		case CMDELIVERYSTATUS_UKNOWN_DEST:
				status = CMDELIVERYSTATUS_UKNOWN_DEST;
				break;
		case CMDELIVERYSTATUS_GROUP_ADDR_W_INDIVIDUAL:
				status = CMDELIVERYSTATUS_GROUP_ADDR_W_INDIVIDUAL;
				break;
		case CMDELIVERYSTATUS_NREACHABLE:
				status = CMDELIVERYSTATUS_NREACHABLE;
				break;
		case CMDELIVERYSTATUS_NREGISTERED:
				status = CMDELIVERYSTATUS_NREGISTERED;
				break;
		case CMDELIVERYSTATUS_DEST_QUEUE_FULL:
				status = CMDELIVERYSTATUS_DEST_QUEUE_FULL;
				break;
		case CMDELIVERYSTATUS_MSG_TOO_LONG:
				status = CMDELIVERYSTATUS_MSG_TOO_LONG;
				break;
		case CMDELIVERYSTATUS_DEST_NSUPPORT_SDS:
				status = CMDELIVERYSTATUS_DEST_NSUPPORT_SDS;
				break;
		case CMDELIVERYSTATUS_DEST_NCONNECTED:
				status = CMDELIVERYSTATUS_DEST_NCONNECTED;
				break;
		case CMDELIVERYSTATUS_PROTOCOL_NSUPPORTED:
				status = CMDELIVERYSTATUS_PROTOCOL_NSUPPORTED;
				break;
		case CMDELIVERYSTATUS_DEST_NACCEPTING_SDS:
				status = CMDELIVERYSTATUS_DEST_NACCEPTING_SDS;
				break;
		case CMDELIVERYSTATUS_DEST_ADM_PROHIBITED:
				status = CMDELIVERYSTATUS_DEST_ADM_PROHIBITED;
				break;
		case CMDELIVERYSTATUS_UNKNOWN_SUBSCRIBER:
				status = CMDELIVERYSTATUS_UNKNOWN_SUBSCRIBER;
				break;
		case CMDELIVERYSTATUS_DEST_NREACHABLE_FAILED:
				status = CMDELIVERYSTATUS_DEST_NREACHABLE_FAILED;
				break;
		case CMDELIVERYSTATUS_CORRUPT_DISCARDED:
				status = CMDELIVERYSTATUS_CORRUPT_DISCARDED;
				break;
		default:
			if (showMsg)
			std::cout << "Unknown status" << std::endl;
			FUNCTION_EXIT;
			return false;
	}

	setCMDeliveryStatus(status);

	FUNCTION_EXIT;
	return true;
}

static std::string toHex(int x)
{
	std::string ret;
	const std::string def = "0123456789ABCDEF";
	ret = ret + def[(x >> 4) & 0xF];
	ret = ret + def[x & 0xF];
	return ret;
}

bool RadioCommsObserver::execSetCmdata(const std::string& cmdValue)
{
	FUNCTION_ENTRY("execSetCmdata");

	// user data
	m_userData = "";

	for(unsigned int j = 0; j<cmdValue.size(); j++)
	{
		m_userData += cmdValue[j];
	}

	// TL Length
	int decLength = (cmdValue.size() + 7) * 8;

	m_tlLength = toHex((decLength >> 8) & 0xFF);
	m_tlLength += toHex(decLength & 0xFF);

//	cout << "user data = " << m_userData << endl;
//	cout << "length = " << m_tlLength << endl;

	FUNCTION_EXIT;
	return true;
}

bool RadioCommsObserver::execSendCmdata()
{
	FUNCTION_ENTRY("execSendCmdata");

	std::string cmData = m_userData; //m_cmHeader + m_tlLength + m_tlHeader + m_userData;
	std::vector<unsigned char> msg;

	
	// 2. Convert input string to vector:
	unsigned int code = 0;
	unsigned int i = 0;
	int counter = 0;
	msg.clear();

	//printf("String size = %i\n\n", str.size());
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

	/*
	int x = msg.size() - 2;
	msg[0] = (x & 0xFF00) >> 8;
	msg[1] = x & 0xFF;
	*/


	sendData(msg);

	printf("\n");
	for(unsigned int j=0; j<msg.size(); j++)
	{
		printf("%2x", msg[j]);
	}
	printf("\n");
	printData(msg,false,true);

	FUNCTION_EXIT;
	return true;
}

void RadioCommsObserver::setCMHeader(std::string header)
{
	FUNCTION_ENTRY("setCMHeader");

	m_cmHeader = header;

	FUNCTION_EXIT;
}

void RadioCommsObserver::setTLHeader(std::string header)
{
	FUNCTION_ENTRY("setTLHeader");

	m_tlHeader = header;

	FUNCTION_EXIT;
}

std::string RadioCommsObserver::getCMHeader() 
{
	FUNCTION_ENTRY("getCMHeader");
	FUNCTION_EXIT;

	return m_cmHeader;


}

std::string RadioCommsObserver::getTLHeader()
{
	FUNCTION_ENTRY("getTLHeader");
	FUNCTION_EXIT;

	return m_tlHeader;
}

void RadioCommsObserver::printData(const std::vector<unsigned char>& rawMessage, bool isReceived, bool display)
{
	ostringstream data;

	//data<<(isReceived==true ? "---------- Data Received from RadioAgent ---------" : "---------- Data Sent to Radio Agent ---------") <<"\n"; 

	data << hex;

	int counter = 0;
	std::vector<unsigned char>::const_iterator iter;

	for(iter=rawMessage.begin();iter!=rawMessage.end();iter++)
	{
		data << setw(2) << setfill('0') << (int)(*iter) << ' ';

		if (counter == 16)
		{
			data << "\n";
			counter = 0;
		}
		else
			counter++;
	}

	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,data.str().c_str());		
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"DATA SIZE = %d",rawMessage.size());

	char log[1024];
	std::sprintf(log,"Received:  DATA SIZE = %d \n Data: %s",rawMessage.size(),data.str().c_str());
	m_modelLogger->setLogView(log);
}
void RadioCommsObserver::execShowMessage(const std::string& params)
{
	if (params=="0") {
		showMsg = true;
	} else {
		showMsg = false;
	}
}

void RadioCommsObserver::setTtsiLibVer(const std::string& ver)
{
	m_ttisLibVer = ver;
}

void RadioCommsObserver::processRadioMethod( IncomingPacketPtr incomingMethod )
{
	FUNCTION_ENTRY("processRadioMethod");

	IncomingPacket& pData = *incomingMethod;

	m_protocolHelper->processIncomingSDS(pData);

	FUNCTION_EXIT;	
}

RadioMethodTask::RadioMethodTask( IRadioMethodProcessor& callback,
								 IncomingPacketPtr data )
								 : m_callback( callback ),
								 m_data( data )
{
	FUNCTION_ENTRY("RadioMethodTask");
	FUNCTION_EXIT;
}


RadioMethodTask::~RadioMethodTask()
{
	FUNCTION_ENTRY("~RadioMethodTask");
	FUNCTION_EXIT;
}


void RadioMethodTask::executeWorkItem()
{
	FUNCTION_ENTRY("executeWorkItem");

	m_callback.processRadioMethod( m_data );

	FUNCTION_EXIT;
}