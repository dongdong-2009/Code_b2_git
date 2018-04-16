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

#include "app/radio/radio_simulator/src/CMHelper.h"
#include "app/radio/radio_simulator/src/RadioCommsObserver.h"
#include "app/radio/radio_simulator/src/LivePaReponseThread.h"
#include "app/radio/radio_simulator/src/PreRecordedPaTimeOutThread.h"


#include "bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h"


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;
using namespace TA_Base_Core;
using namespace std;

// Message format
static const int CM_PDUTYPE_INDEX = 2;
static const int ADAPTATION_LENGTH = 2;
static const int TL_LENGTH = 2;

static const int CM_REGISTER_REQUEST_LENGTH = 11;
static const int CM_REG_PASSNUMBER_INDEX = 3;
static const int CM_REG_ISSI_INDEX = 7;
static const int CM_REG_REGISTRATIONTYPE_INDEX = 10;

static const int CM_SDTS_BRAODCAST_LENGTH = 19;
static const int CM_SDTS_BROADCAST_TLLENGTH_INDEX = 17;

static const int CM_SDTS_DATA_LENGTH = 15;
static const int CM_SDTS_DATA_SRCADD_INDEX = 3;
static const int CM_SDTS_DATA_DSTADD_INDEX = 6;
static const int CM_SDTS_DATA_PI_INDEX = 9;
static const int CM_SDTS_DATA_FLAGS_INDEX = 10;
static const int CM_SDTS_DATA_MSGREF_INDEX = 11;
static const int CM_SDTS_DATA_AREASEL_INDEX = 12;
static const int CM_SDTS_DATA_TLLENGTH_INDEX = 13;

static const int CM_SDTS_TL_UNITDATA_INDEX = 1;
static const int CM_SDTS_TL_DATA_INDEX = 6;

const int RadioCommsObserver::USER_DATA_INDEX = 22;
static const int USER_DATA_COMMAND_INDEX = 22;
static const int USER_DATA_FLAG = 7;


bool operator<(RadioIdentity a, RadioIdentity b)
{
	return a.getSSI()<b.getSSI();
}

std::map<RadioIdentity, std::string> radioIdMap;
std::map<RadioIdentity, std::string>::iterator radioId;


//////////////////////////////////////////////////////////////////////////////
//            Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

RadioCommsObserver::RadioCommsObserver(TA_Base_Core::TcpObservedSocket< TA_Base_Core::TcpNonblockingSocket > * socket)
:m_clientSocket(socket),
 m_stillConnected(true),
 m_passNumberCheck(true),
 m_cmDeliveryStatus( CMDELIVERYSTATUS_SUCCESS ),
 m_cmHeader("00160000045700045882600000"),
 m_tlHeader("008000000000"),
 m_ttisLibVer("00000000")
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

void RadioCommsObserver::processReceivedData( std::vector<unsigned char> & pData, TA_Base_Core::ISocket * pSocket )
{
    FUNCTION_ENTRY("~processReceivedData");
    // Log a debug warning message

     bool isTranborPASDS = false ;
	 bool isTrainBornPids = false ;
	TA_ASSERT( pData.size() > 0, "Received data is empty");
	if ( pData.size() <3 ) return ;
     	unsigned char c1, c2 , c3, c4;
		c1 = pData [ 0 ] ;
		c2 = pData [ 1 ] ;
        c3 = pData [ 2 ] ;
		c4 = pData [ 3 ] ;

	if ( (( c1==0x00 ) && ( c2==0x1d )) || (( c1==0x00 ) && ( c2==0x21 ))) 
	{
		isTranborPASDS = true ;
	}

   	if ( ( c1==0x00 ) && ( c2>=0x20 && c2 <= 0xA2) && ( c3==0x00 ) && (c4==0x0F)) 
	{
		isTrainBornPids = true ;
	}
//	{

	//printData(pData,true);
	if (showMsg)
	printf("Received %u bytes of data.\n",pData.size());
	printData(pData,true);




	/*
	bihui to seperator the data
	*/
    if (isTranborPASDS ) 
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"isTranborSDS" );
		std::vector < std::vector < unsigned char > > pDatas ;

		std::vector < unsigned char > tempV ;

		int index = 0 ;

	
			while ( index +1 < pData.size() ) 
			{
				unsigned char c1, c2 ;
				c1 = pData [ index ] ;
				c2 = pData [ index+1 ] ;

				if ( (( c1==0x00 ) && ( c2==0x1d )) || (( c1==0x00 ) && ( c2==0x21 ))) 
				{
					if ( index > 0 ) 
					{
						std::vector <unsigned char > pv ( tempV.begin() , tempV.end () ) ;
						pDatas.push_back ( pv ) ;
						tempV.clear() ;
					}
				}
				tempV.push_back ( c1 ) ;
				index++ ;
			}
			tempV.push_back ( pData [ index ] ) ;
			std::vector <unsigned char > pv ( tempV.begin() , tempV.end () ) ;
			pDatas.push_back ( pv ) ;

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"DATA SIZE = %d",pDatas.size());

			std::vector <unsigned char > sdsData ;



			index = 0 ;

			while ( index < pDatas.size() )
			{
				std::vector <unsigned char > pv = pDatas [ index ] ;
				printData(pv,true);
			   processIncomeSDS ( pv) ;
			   Sleep ( 100 ) ;

			   index ++ ;
			 }
	}
	else if ( isTrainBornPids ) 
	{
		 LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"isTrainBornPids" );
		std::vector < std::vector < unsigned char > > pDatas ;

		std::vector < unsigned char > tempV ;

		int index = 0 ;

	
			while ( index +2 < pData.size() ) 
			{
				unsigned char c1, c2 , c3, c4;
				c1 = pData [ index ] ;
				c2 = pData [ index+1 ] ;
				c3 = pData [ index+2 ] ;
				c4 = pData[index+3];

				if  ( ( c1==0x00 ) && ( c2>=0x20 && c2 <= 0xA2) && ( c3==0x00 )&& (c4==0x0F)) 
				{
					if ( index > 0 ) 
					{
						std::vector <unsigned char > pv ( tempV.begin() , tempV.end () ) ;
						pDatas.push_back ( pv ) ;
						tempV.clear() ;
					}
				}
				tempV.push_back ( c1 ) ;
				index++ ;
			}
			tempV.push_back ( pData [ index ] ) ;
			tempV.push_back ( pData [ index +1 ] ) ;
			std::vector <unsigned char > pv ( tempV.begin() , tempV.end () ) ;
			pDatas.push_back ( pv ) ;

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"DATA SIZE = %d",pDatas.size());

			std::vector <unsigned char > sdsData ;



			index = 0 ;

			while ( index < pDatas.size() )
			{
				std::vector <unsigned char > pv = pDatas [ index ] ;
				printData(pv,true);
			   processIncomeSDS ( pv) ;
			   Sleep ( 100 ) ;

			   index ++ ;
			 }
	}

	else
	{
		processIncomeSDS ( pData ) ;
	}

	 pData.clear() ;


    FUNCTION_EXIT;
}

void RadioCommsObserver::processIncomeSDS ( std::vector<unsigned char> & pData ) 
{
	FUNCTION_ENTRY("~processIncomeSDS");
	switch(pData[CM_PDUTYPE_INDEX])
		{
			case CMPDUTYPE_REGISTER_REQ: 
				{
					CMRegistrationStatus status;
			
					RadioIdentity* id = new RadioIdentity();
					
					if(!parseRegistrationRequest(pData, id))
					{
						if (showMsg)
						std::cout << "Cannot parse the registration request!" << std::endl;
						status = CMREGISTERSTATUS_REGISTRATIONNOTAVAILABLE;
					}
					else
						status = processIdentity(id);

					buildRegisterResponse(pData, status);
				
					delete id;
							
				}				 
				break;
			case CMPDUTYPE_BROADCAST:
				{
					if(!parseCMBroadcast(pData))
						if (showMsg)
						std::cout << "Cannot parse SDTS-CM-BROADCAST message!" << std::endl;
				}
				break;
			case CMPDUTYPE_DATA_REQ:
				{	
					if(parseCMData(pData))
					{
						if(m_cmData.getTLPduType() == TLPDUTYPE_DATA)
						{
							if(m_cmDeliveryStatus == CMDELIVERYSTATUS_SUCCESS) {
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BuildTLReport!");
								buildTLReport();
							}
							else {
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BuildCMReport!");
								buildCMReport();
							}
							if (USER_DATA_INDEX < pData.size()) {
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BuildISCSResponse!");
								buildISCSResponse(pData);
							}
						}
					}
					else
						if (showMsg)
						std::cout << "Cannot parse SDTS-CM-DATA message!" << std::endl;	
				}
				break;
			default:
				if (showMsg)
				std::cout << "Message with invalid PDU type received!" << std::endl;
				
		}
	

	pData.clear();
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
//parseRegistrationData
//
bool RadioCommsObserver::parseRegistrationRequest(const std::vector<unsigned char>& pData, RadioIdentity* id)
{
	FUNCTION_ENTRY("parseRegistrationData");

	// Message length
	if (pData.size() != CM_REGISTER_REQUEST_LENGTH)
	{
		std::cout << "Invalid message length for registration request!" << std::endl;
		FUNCTION_EXIT;
		return false;
	}
	
	// Adaptation layer
	std::vector<unsigned char> vAdaptation(pData.begin(), pData.begin()+ADAPTATION_LENGTH);
	int regAdaptationLength = CMHelper::decode_u16(vAdaptation);

	if (regAdaptationLength != CM_REGISTER_REQUEST_LENGTH - ADAPTATION_LENGTH)
	{
		std::cout << "Invalid adaptation length for registration request!" << std::endl;
		FUNCTION_EXIT;
		return false;
	}

	// CM PDU Type
	std::vector<unsigned char> vPDUtype(1, pData[CM_PDUTYPE_INDEX]);
	CMPDUType pduType = CMHelper::decodeCM_PDUType(vPDUtype);
	if (pduType != CMPDUTYPE_REGISTER_REQ)
	{
		std::cout << "Invalid PDU type for registration request!" << std::endl;
		FUNCTION_EXIT;
		return false;
	}

	// Pass Number - 4 bytes
	std::vector<unsigned char> vPassNum(pData.begin() + CM_REG_PASSNUMBER_INDEX, 
										pData.begin() + CM_REG_PASSNUMBER_INDEX+4);
	unsigned long regPassNum = CMHelper::decode_u32(vPassNum);

	// ISSI - 3 bytes
	std::vector<unsigned char> vClientSSI(pData.begin() + CM_REG_ISSI_INDEX, 
										  pData.begin() + CM_REG_ISSI_INDEX +3);									  
	unsigned int regSSI = CMHelper::decodeSSI(vClientSSI);
	
	id->setSSI(regSSI);
	id->setPassNumber(regPassNum);		
    
    FUNCTION_EXIT;

	return true;
}

//
//processIdentity
//
CMRegistrationStatus RadioCommsObserver::processIdentity(RadioIdentity* id)
{
	FUNCTION_ENTRY("processIdentity");
	CMRegistrationStatus status;

	unsigned int SSI = id->getSSI();
	unsigned long passNumber = id->getPassNumber();
	bool bFound = false;

	// Format socket id
	char buffStr[16];
	sprintf(buffStr, "%d", m_clientSocket->getSocketId());
	std::string socketIdStr=buffStr;

	for(radioId=radioIdMap.begin(); radioId!=radioIdMap.end(); ++radioId)
	{
		RadioIdentity temp = radioId->first;

		// Check ISSI 
		if(temp.getSSI()==SSI)
		{
			bFound = true;

			// Check pass number
			if(m_passNumberCheck)
			{
				if(temp.getPassNumber()!=passNumber)
				{
					status = CMREGISTERSTATUS_INVALIDPASSNUMBER;
					break;
				}
			}
		
			if(radioId->second == "")
			{
				radioId->second = socketIdStr;
				status = CMREGISTERSTATUS_ADDRESSREGISTERED;
			}

			else if(radioId->second.compare(socketIdStr) == 0)
			{
				status = CMREGISTERSTATUS_DUPLICATEADDRESS;			
			}

			else
			{
				radioId->second = socketIdStr;
				status = CMREGISTERSTATUS_ADDRESSREGISTERED;
			}
			break;
		}	
	}

	if(!bFound)
		status = CMREGISTERSTATUS_INVALIDADDRESS;

	FUNCTION_EXIT;
	return status;
}

//
//buildRegisterResponse
//
void RadioCommsObserver::buildRegisterResponse(const std::vector<unsigned char>& pData, CMRegistrationStatus status)
{
	FUNCTION_ENTRY("buildRegisterResponse");

	std::vector<unsigned char> rawData;
	std::vector<unsigned char> tempData;

	// CM PDU Type
	tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_REGISTER_RESP);
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// Pass Number
	rawData.insert( rawData.end(), pData.begin() + CM_REG_PASSNUMBER_INDEX, 
										pData.begin() + CM_REG_PASSNUMBER_INDEX+4);

	// ISSI Address
	rawData.insert( rawData.end(), pData.begin() + CM_REG_ISSI_INDEX, 
										  pData.begin() + CM_REG_ISSI_INDEX +3);

	// Registration Type
	rawData.insert( rawData.end(), pData.begin() + CM_REG_REGISTRATIONTYPE_INDEX, 
										  pData.begin() + CM_REG_REGISTRATIONTYPE_INDEX +1);

	// Registration status
	tempData = CMHelper::encodeCM_RegistrationStatus(status);
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// Adaptation length, 2 bytes at beginning
	unsigned int nAdaptation = rawData.size();
	tempData = CMHelper::encode_u16(nAdaptation);
	rawData.insert( rawData.begin(), tempData.begin(), tempData.end() );


	// For debugging purposes 
    std::ostringstream dump;
    dump << std::hex;

    for (unsigned int k = 0; k < rawData.size(); k++)
    {
        // for debugging purposes
        dump << static_cast<unsigned int>(rawData[k]) << " ";
    }

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending Register Response: %s", dump.str().c_str());

	sendData(rawData);

	FUNCTION_EXIT;
}

//
//parseBroadcastData
//
bool RadioCommsObserver::parseCMBroadcast(const std::vector<unsigned char>& pData)
{
	FUNCTION_ENTRY("parseBroadcastData");

	// Adaptation layer:
	std::vector<unsigned char> vAdaptation(pData.begin(), pData.begin()+ADAPTATION_LENGTH);
	unsigned int AdaptationLength = CMHelper::decode_u16(vAdaptation);

	// Check for packet length
	if( AdaptationLength != pData.size() - ADAPTATION_LENGTH)
	{
		FUNCTION_EXIT;
		return false;
	}

	// Get TL Length (in byte)
	std::vector<unsigned char> vTLLength(pData.begin() + CM_SDTS_BROADCAST_TLLENGTH_INDEX,
										 pData.begin() + CM_SDTS_BROADCAST_TLLENGTH_INDEX + TL_LENGTH);
	int TLLength = CMHelper::decode_u16(vTLLength)/8; 

	// Check user data length
	if(pData.size()-TLLength !=  CM_SDTS_BRAODCAST_LENGTH)
	{
		FUNCTION_EXIT;
		return false;
	}
	
	// SDTS-TL PDU Type
	std::vector<unsigned char> vPDUtype(1, pData[CM_SDTS_BRAODCAST_LENGTH]);
	TLPDUType pduType = CMHelper::decodeTL_PDUType(vPDUtype);

	// User Data
	if (showMsg)
	std::cout << "Broadcast data received: " <<std::endl;
	if (pduType == TLPDUTYPE_UNITDATA)
	{
		int dataLength = TLLength - CM_SDTS_TL_UNITDATA_INDEX;

		std::vector<unsigned char> vUserData(pData.begin() + CM_SDTS_BRAODCAST_LENGTH + CM_SDTS_TL_UNITDATA_INDEX, 
			pData.begin() + CM_SDTS_BRAODCAST_LENGTH + CM_SDTS_TL_UNITDATA_INDEX + dataLength);

		displayData(vUserData);
	}
	
	else if (pduType == TLPDUTYPE_DATA)
	{
		int dataLength = TLLength - CM_SDTS_TL_DATA_INDEX;

		std::vector<unsigned char> vUserData(pData.begin() + CM_SDTS_BRAODCAST_LENGTH + CM_SDTS_TL_DATA_INDEX, 
			pData.begin() + CM_SDTS_BRAODCAST_LENGTH + CM_SDTS_TL_DATA_INDEX + dataLength);

		displayData(vUserData);
	}

	FUNCTION_EXIT;
	return true;
}

//
//parseCMData
//
bool RadioCommsObserver::parseCMData(const std::vector<unsigned char>& pData)
{
	FUNCTION_ENTRY("parseCMData");
	
	// Adaptation layer:
	std::vector<unsigned char> vAdaptation(pData.begin(), pData.begin()+ADAPTATION_LENGTH);
	unsigned int AdaptationLength = CMHelper::decode_u16(vAdaptation);

	// Check packet length
	if( AdaptationLength != pData.size() - ADAPTATION_LENGTH)
	{
		FUNCTION_EXIT;
		return false;
	}

	// Get TL Length (in byte)
	std::vector<unsigned char> vTLLength(pData.begin() + CM_SDTS_DATA_TLLENGTH_INDEX,
										 pData.begin() + CM_SDTS_DATA_TLLENGTH_INDEX + TL_LENGTH);
	int TLLength = CMHelper::decode_u16(vTLLength)/8;

	// Check User data length
	if(pData.size()-TLLength !=  CM_SDTS_DATA_LENGTH)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "pdata size = %u, TLLength = %u", pData.size(),TLLength);
		FUNCTION_EXIT;
		return false;
	}
	
	// Save CM Data Item:

	// Source Address
	std::vector<unsigned char> vSrcAddr(pData.begin() + CM_SDTS_DATA_SRCADD_INDEX, 
										pData.begin() + CM_SDTS_DATA_SRCADD_INDEX+3);
	unsigned int result = CMHelper::decodeSSI(vSrcAddr);
	m_cmData.setSrcAddr(result);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Source Address: %x\n", result);

	// Destination Address
	std::vector<unsigned char> vDstAddr(pData.begin() + CM_SDTS_DATA_DSTADD_INDEX, 
										pData.begin() + CM_SDTS_DATA_DSTADD_INDEX+3);
	result = CMHelper::decodeSSI(vDstAddr);
	m_cmData.setDstAddr(result);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Source Address:%x\n", result);

	// Protocol Identification
	std::vector<unsigned char> vPI(pData.begin() + CM_SDTS_DATA_PI_INDEX, 
								   pData.begin() + CM_SDTS_DATA_PI_INDEX+1);
	CMProtocolIdentifier pi=CMHelper::decodeCM_ProtocolIdentifier(vPI);
	m_cmData.setProtocolId(pi);

	// Flags, message reference, area selection
	m_cmData.setFlags(pData[CM_SDTS_DATA_FLAGS_INDEX]);
	m_cmData.setMsgRef(pData[CM_SDTS_DATA_MSGREF_INDEX]);
	m_cmData.setAreaSel(pData[CM_SDTS_DATA_AREASEL_INDEX]);

	// SDTS-TL PDU Type:
	std::vector<unsigned char> vPDUtype(1, pData[CM_SDTS_DATA_LENGTH]);
	TLPDUType pduType = CMHelper::decodeTL_PDUType(vPDUtype);
	m_cmData.setTLPduType(pduType);

	// Display user data:

	//std::cout << "CM Data received: " << std::endl;

	/*if (pduType == TLPDUTYPE_UNITDATA)
	{
		int dataLength = TLLength - CM_SDTS_TL_UNITDATA_INDEX;

		std::vector<unsigned char> vUserData(pData.begin() + CM_SDTS_DATA_LENGTH + CM_SDTS_TL_UNITDATA_INDEX, 
			pData.begin() + CM_SDTS_DATA_LENGTH + CM_SDTS_TL_UNITDATA_INDEX + dataLength);

		displayData(vUserData);
	}
	
	else if (pduType == TLPDUTYPE_DATA)
	{
		int dataLength = TLLength - CM_SDTS_TL_DATA_INDEX;

		std::vector<unsigned char> vUserData(pData.begin() + CM_SDTS_DATA_LENGTH + CM_SDTS_TL_DATA_INDEX, 
			pData.begin() + CM_SDTS_DATA_LENGTH + CM_SDTS_TL_DATA_INDEX + dataLength);

		displayData(vUserData);
	}*/

	FUNCTION_EXIT;
	return true;
}

//
//buildTLReport
//
void RadioCommsObserver::buildTLReport()
{
	FUNCTION_ENTRY("buildTLReport");

	std::vector<unsigned char> rawData;
	std::vector<unsigned char> tempData;

	// CM PDU Type
	tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_DATA_REQ);
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// Source Address
	tempData = CMHelper::encodeSSI( m_cmData.getDstAddr() );
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// Destination Address
	tempData = CMHelper::encodeSSI( m_cmData.getSrcAddr() );
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// Protocol Identifier
	tempData = CMHelper::encodeCM_ProtocolIdentifier( m_cmData.getProtocolId() );
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// Flags
	rawData.insert( rawData.end(), m_cmData.getFlags());

	// Message Reference
	rawData.insert( rawData.end(), m_cmData.getMsgRef());

	// Area Slection
	rawData.insert( rawData.end(), m_cmData.getAreaSel());

	// TL Length
	unsigned int nTLlength = 2*8;
	tempData = CMHelper::encode_u16(nTLlength);
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// TL PDU Type
	tempData = CMHelper::encodeTL_PDUType(TLPDUTYPE_REPORT);
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// Delivery Status
	tempData = CMHelper::encodeCM_DeliveryStatus(m_cmDeliveryStatus);
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// Adaptation length, 2 bytes at beginning
	unsigned int nAdaptation = rawData.size();
	tempData = CMHelper::encode_u16(nAdaptation);
	rawData.insert( rawData.begin(), tempData.begin(), tempData.end() );


	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending TL Response to Radio Agent:");
	printData(rawData,false);

	sendData(rawData);

	FUNCTION_EXIT;

}

//
//buildCMReport
//
void RadioCommsObserver::buildCMReport()
{
	FUNCTION_ENTRY("buildCMReport");

	std::vector<unsigned char> rawData;
	std::vector<unsigned char> tempData;

	// CM PDU Type
	tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_REPORT_RESP);
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// Source Address
	tempData = CMHelper::encodeSSI( m_cmData.getSrcAddr() );
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// Destination Address
	tempData = CMHelper::encodeSSI( m_cmData.getDstAddr() );
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// Protocol Identifier
	tempData = CMHelper::encodeCM_ProtocolIdentifier( m_cmData.getProtocolId() );
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// Message Reference
	rawData.insert( rawData.end(), m_cmData.getMsgRef());

	// Delivery Status
	tempData = CMHelper::encodeCM_DeliveryStatus(m_cmDeliveryStatus);
	rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

	// A`daptation length, 2 bytes at beginning
	unsigned int nAdaptation = rawData.size();
	tempData = CMHelper::encode_u16(nAdaptation);
	rawData.insert( rawData.begin(), tempData.begin(), tempData.end() );


	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending CM Response to Radio Agent:");
	printData(rawData,false);
	sendData(rawData);

	FUNCTION_EXIT;

}

void RadioCommsObserver::buildLivePaReset(const std::vector<unsigned char>& pData, std::vector<unsigned char>& resp)
{
    std::vector<unsigned char> tempData;
    int userDataIndex = 0;
    unsigned char announcementId;

    // Get announcementId
    if ( PA_LIVE_ANNOUNCEMENT == pData[23])
    {
        announcementId = pData[28];
    } 
    else if ( PA_CONTINUE == pData[23])
    {
        announcementId = pData[27];
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Invaild command type\n");
    }
    
    // CM PDU Type
    tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_DATA_REQ);
    resp.insert( resp.end(), tempData.begin(), tempData.end() );
    
    // Source Address
    tempData = CMHelper::encodeSSI( m_cmData.getDstAddr() );
    resp.insert( resp.end(), tempData.begin(), tempData.end() );
    
    // Destination Address
    tempData = CMHelper::encodeSSI( m_cmData.getSrcAddr() );
    resp.insert( resp.end(), tempData.begin(), tempData.end() );
    
    // Protocol Identifier
    tempData = CMHelper::encodeCM_ProtocolIdentifier( m_cmData.getProtocolId() );
    resp.insert( resp.end(), tempData.begin(), tempData.end() );
    
    // Flags
    resp.push_back(m_cmData.getFlags());
    //resp.insert( resp.end(), m_cmData.getFlags());
    
    // Message Reference
    resp.push_back(m_cmData.getMsgRef());
    //resp.insert( resp.end(), m_cmData.getMsgRef());
    
    // Area Slection
    resp.push_back(m_cmData.getAreaSel());
    //resp.insert( resp.end(), m_cmData.getAreaSel());
    
    // TL Length = 0, to be determined later
    //resp.push_back(0); resp.push_back(0);
    
    
    // TL Header
    //resp.push_back(0);
    //resp.push_back(0);
    for (int i = 0; i < m_tlHeader.size(); i+=2) {
        resp.push_back(CMHelper::Hex2Int(m_tlHeader.at(i),1)+CMHelper::Hex2Int(m_tlHeader.at(i+1),0));
    }
    
    // check CRC16
    std::vector<unsigned char> rawData;
    unsigned short calcCRC;
    userDataIndex = resp.size();
    
    //resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
    resp.push_back(USER_DATA_FLAG);
    resp.push_back(PA_LIVE_RESET);
    resp.push_back( pData[24] ); // physical train number
    resp.push_back(8); // Length
    resp.push_back(0); // Origin, TIMS = 0
    resp.push_back(pData[28]); // Announcement ID
    
    rawData.clear();
    rawData.insert(rawData.end(), resp[userDataIndex], resp[resp.size()-1]);
    calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(rawData,rawData.size());
    tempData = CMHelper::encode_u16(calcCRC);
    //resp.insert(resp.end(), tempData.begin(), tempData.end());
	resp.insert(resp.end(), tempData.at(1));
	resp.insert(resp.end(), tempData.at(0));
    
    // calculate TL Length
    int decLength = (resp.size()-userDataIndex +7 )*8;
    
    std::vector<unsigned char>::iterator iter;
    iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2;
    resp.insert(iter, (unsigned char)  (decLength & 0xFF00) >> 8);
    iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2+1;
    //resp[CM_SDTS_DATA_TLLENGTH_INDEX] = (decLength & 0xFF00) >> 8;
    resp.insert(iter, (unsigned char)  (decLength & 0x00FF));
    //resp[CM_SDTS_DATA_TLLENGTH_INDEX+1] = (decLength & 0x00FF);	
    // Adaptation length, 2 bytes at beginning
    unsigned int nAdaptation = resp.size();
    tempData = CMHelper::encode_u16(nAdaptation);
    resp.insert( resp.begin(), tempData.begin(), tempData.end() );
    
    
    //printf("\nSending Response (Message Type: %x):", resp[23]);
    //for(unsigned int j=0; j<resp.size(); j++)
    //{
    //printf("%3x ", resp[j]);
    //}
    //printf("Sending PA Ready Response to Radio Agent:\n");
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Prepare Live PA reset, id = %d\n", pData[28] - 0);
    
    
    printData(resp,false);
}

void RadioCommsObserver::buildLiveRequestResetResponse(const std::vector<unsigned char>& pData)
{
    std::vector<unsigned char> resp;
    std::vector<unsigned char> tempData;
    int userDataIndex = 0;
    unsigned char announcementId;

    // Get announcementId
    if ( PA_LIVE_ANNOUNCEMENT == pData[23])
    {
        announcementId = pData[28];
    } 
    else if ( PA_CONTINUE == pData[23])
    {
        announcementId = pData[27];
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Invaild command type\n");
    }
    
    // CM PDU Type
    tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_DATA_REQ);
    resp.insert( resp.end(), tempData.begin(), tempData.end() );
    
    // Source Address
    tempData = CMHelper::encodeSSI( m_cmData.getDstAddr() );
    resp.insert( resp.end(), tempData.begin(), tempData.end() );
    
    // Destination Address
    tempData = CMHelper::encodeSSI( m_cmData.getSrcAddr() );
    resp.insert( resp.end(), tempData.begin(), tempData.end() );
    
    // Protocol Identifier
    tempData = CMHelper::encodeCM_ProtocolIdentifier( m_cmData.getProtocolId() );
    resp.insert( resp.end(), tempData.begin(), tempData.end() );
    
    // Flags
    resp.push_back(m_cmData.getFlags());
    //resp.insert( resp.end(), m_cmData.getFlags());
    
    // Message Reference
    resp.push_back(m_cmData.getMsgRef());
    //resp.insert( resp.end(), m_cmData.getMsgRef());
    
    // Area Slection
    resp.push_back(m_cmData.getAreaSel());
    //resp.insert( resp.end(), m_cmData.getAreaSel());
    
    // TL Length = 0, to be determined later
    //resp.push_back(0); resp.push_back(0);
    
    
    // TL Header
    //resp.push_back(0);
    //resp.push_back(0);
    for (int i = 0; i < m_tlHeader.size(); i+=2) {
        resp.push_back(CMHelper::Hex2Int(m_tlHeader.at(i),1)+CMHelper::Hex2Int(m_tlHeader.at(i+1),0));
    }
    
    // check CRC16
    std::vector<unsigned char> rawData;
    unsigned short calcCRC;
    userDataIndex = resp.size();
    
    //resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
    resp.push_back(USER_DATA_FLAG);
    resp.push_back(REQUEST_FOR_PA_RESET);
    resp.push_back( pData[24] ); // physical train number
    resp.push_back(8); // Length
    resp.push_back(0); // Origin, TIMS = 0
    resp.push_back(announcementId); // Announcement ID
    
    rawData.clear();
    rawData.insert(rawData.end(), resp[userDataIndex], resp[resp.size()-1]);
    calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(rawData,rawData.size());
    tempData = CMHelper::encode_u16(calcCRC);
    resp.insert(resp.end(), tempData.begin(), tempData.end());
    
    // calculate TL Length
    int decLength = (resp.size()-userDataIndex +7 )*8;
    
    std::vector<unsigned char>::iterator iter;
    iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2;
    resp.insert(iter, (unsigned char)  (decLength & 0xFF00) >> 8);
    iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2+1;
    //resp[CM_SDTS_DATA_TLLENGTH_INDEX] = (decLength & 0xFF00) >> 8;
    resp.insert(iter, (unsigned char)  (decLength & 0x00FF));
    //resp[CM_SDTS_DATA_TLLENGTH_INDEX+1] = (decLength & 0x00FF);	
    // Adaptation length, 2 bytes at beginning
    unsigned int nAdaptation = resp.size();
    tempData = CMHelper::encode_u16(nAdaptation);
    resp.insert( resp.begin(), tempData.begin(), tempData.end() );
    
    
    //printf("\nSending Response (Message Type: %x):", resp[23]);
    //for(unsigned int j=0; j<resp.size(); j++)
    //{
    //printf("%3x ", resp[j]);
    //}
    //printf("Sending PA Ready Response to Radio Agent:\n");
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Prepare request for PA reset id = %d\n", announcementId - 0);
    
    
    printData(resp,false);

    LivePaReponseThread::LiveRespThreadId threadId = make_pair(announcementId, pData[24]);
    LivePaReponseThread::insertRequestResetResponse(threadId, resp);
}
void RadioCommsObserver::buildISCSResponse(const std::vector<unsigned char>& pData)
{
	std::vector<unsigned char> resp;
	std::vector<unsigned char> tempData;
	int userDataIndex = 0;

	// CM PDU Type
	tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_DATA_REQ);
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Source Address
	tempData = CMHelper::encodeSSI( m_cmData.getDstAddr() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Destination Address
	tempData = CMHelper::encodeSSI( m_cmData.getSrcAddr() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Protocol Identifier
	tempData = CMHelper::encodeCM_ProtocolIdentifier( m_cmData.getProtocolId() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Flags
	resp.push_back(m_cmData.getFlags());
	//resp.insert( resp.end(), m_cmData.getFlags());

	// Message Reference
	resp.push_back(m_cmData.getMsgRef());
	//resp.insert( resp.end(), m_cmData.getMsgRef());

	// Area Slection
	resp.push_back(m_cmData.getAreaSel());
	//resp.insert( resp.end(), m_cmData.getAreaSel());

	// TL Length = 0, to be determined later
	//resp.push_back(0); resp.push_back(0);


	// TL Header
	//resp.push_back(0);
	//resp.push_back(0);
	for (int i = 0; i < m_tlHeader.size(); i+=2) {
		resp.push_back(CMHelper::Hex2Int(m_tlHeader.at(i),1)+CMHelper::Hex2Int(m_tlHeader.at(i+1),0));
	}

	char v1,v2,v3,v4,v5,v6,v7,v8;
	v1 = m_ttisLibVer.at(0);
	v2 = m_ttisLibVer.at(1);
	v3 = m_ttisLibVer.at(2);
	v4 = m_ttisLibVer.at(3);
	v5 = m_ttisLibVer.at(4);
	v6 = m_ttisLibVer.at(5);
	v7 = m_ttisLibVer.at(6);
	v8 = m_ttisLibVer.at(7);
	int a1,a2,a3,a4 = 0;
	a1 = CMHelper::Hex2Int(v1,1) + CMHelper::Hex2Int(v2,0);
	a2 = CMHelper::Hex2Int(v3,1) + CMHelper::Hex2Int(v4,0);
	a3 = CMHelper::Hex2Int(v5,1) + CMHelper::Hex2Int(v6,0);
	a4 = CMHelper::Hex2Int(v7,1) + CMHelper::Hex2Int(v8,0);

	

	// check CRC16
	std::vector<unsigned char> rawData;
	rawData.insert(rawData.end(), pData[USER_DATA_COMMAND_INDEX], pData[pData.size()-1 - 2]); // size-2, not include the last 2 byte, CRC checksum
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "calculate checksum calcCRC");
	unsigned short calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(rawData,rawData.size());
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "calculate checksum dataCRC");
	unsigned short dataCRC = static_cast<unsigned short>((pData[pData.size()-2] << 8) + pData[pData.size()-1]);
	if (calcCRC == dataCRC) {
		resp.push_back(USER_DATA_FLAG);
		resp.push_back(BAD_CHECKSUM);
		resp.push_back(0); // physical train number
		resp.push_back(9); // Length
		resp.push_back(0); // Origin, TIMS = 0
		resp.push_back(pData[USER_DATA_COMMAND_INDEX]);
		resp.push_back(2);
	} else {
		userDataIndex = resp.size();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "userdataindex = %u",userDataIndex);
	
	switch (pData[USER_DATA_COMMAND_INDEX]) {	

		case PA_LIVE_ANNOUNCEMENT:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "receive live announcement id = %d\n", pData[28] - 0);
//			buildPAReadyResponse(pData);
/*
			//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(READY_FOR_LIVE_ANNOUNCEMENT);
			resp.push_back(0); // physical train number
			resp.push_back(8); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(pData[28]);
			// calculate check sum and send the message
*/
			//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(PA_COMMAND_RECEIVED);
			resp.push_back( pData[24] ); // physical train number
			resp.push_back(15); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back( 0 ); // status 2
			resp.push_back(OC1_PA_LIVE_ANNOUNCEMENT);
			resp.push_back(1); // status 3, need simulated thread
			resp.push_back(0);
			resp.push_back(0);
			resp.push_back(0);
			resp.push_back(0);
			resp.push_back(pData[28]);
			// calculate check sum and send the message


			break;
		case PA_PRERECORDED_ANNOUNCEMENT:
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(PA_COMMAND_RECEIVED);
			resp.push_back( pData[24] ); // physical train number
			resp.push_back(15); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(0); // status 2
			resp.push_back(OC1_PA_PRERECORDED_ANNOUNCEMENT);
			resp.push_back(1); // status 3, need simulated thread
			resp.push_back(pData[31]);
			resp.push_back(pData[32]);
			resp.push_back(pData[33]);
            resp.push_back(pData[34]);
            resp.push_back(pData[35]); // announcement id
			// calculate check sum and send the message


			break;
		case PA_LIVE_RESET:
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "receive pa live reset, id = %d\n", pData[27] - 0);
                resp.push_back(USER_DATA_FLAG);
                resp.push_back(PA_COMMAND_RECEIVED);
                resp.push_back( pData[24] ); // physical train number
                resp.push_back(15); // Length
                resp.push_back(0); // Origin, TIMS = 0
                resp.push_back(0); // status 2
                resp.push_back(OC1_LIVE_PA_RESET);
                resp.push_back(5); // status 3, need simulated thread TODO: for new protocal change to 5
                resp.push_back(0);
                resp.push_back(0);
                resp.push_back(0);
                resp.push_back(0);
                resp.push_back(pData[27]);
                // calculate check sum and send the message
                
                // Terminate Pa live thread
                std::vector<unsigned char> terminateResp;
                terminateResp.resize(24);
                terminateResp[23] = 0xFF;
                LivePaReponseThread::LiveRespThreadId threadId = make_pair(pData[27], pData[24]);
                LivePaReponseThread::insertRequestResetResponse(threadId, terminateResp);
                
			break;
            }
        case PA_PRERECORDED_RESET: // For new protocal
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "receive pa prerecord reset, id = %d\n", pData[27] - 0);
                resp.push_back(USER_DATA_FLAG);
                resp.push_back(PA_COMMAND_RECEIVED);
                resp.push_back( pData[24] ); // physical train number
                resp.push_back(15); // Length
                resp.push_back(0); // Origin, TIMS = 0
                resp.push_back(0); // status 2
                resp.push_back(OC1_DVA_PA_RESET);
                resp.push_back(5); // status 3, need simulated thread TODO: for new protocal change to 5
                resp.push_back(0);
                resp.push_back(0);
                resp.push_back(0);
                resp.push_back(0);
                resp.push_back(pData[27]);
                break;
            }
		case PA_CONTINUE:

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Receive continue resp id = %d\n", pData[27] - 0);
			//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(READY_FOR_LIVE_ANNOUNCEMENT);
			resp.push_back(0); // physical train number
			resp.push_back(8); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(pData[27]);
			// calculate check sum and send the message
            buildLiveRequestResetResponse(pData);
			break;
//case REQUEST_FOR_PA_RESET:
//case TRAIN_PA_MESSAGE_COMPLETE:

		case TTIS_FREE_TEXT_MESSAGE:
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(TTIS_COMMAND_RECEIVED);
			resp.push_back( pData[23] ); // physical train number
			resp.push_back(13); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(1); // status 5, to do
			resp.push_back(OC2_TTIS_FREE_TEXT_MESSAGE);
			resp.push_back(a1); // library version number
			resp.push_back(a2);
			resp.push_back(a3);
			resp.push_back(a4);
			// calculate check sum and send the message

			break;
		case TTIS_PRESTORED_MESSAGE:
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(TTIS_COMMAND_RECEIVED);
			resp.push_back( pData[23] ); // physical train number
			resp.push_back(13); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(1); // status 5, to do
			resp.push_back(OC2_TTIS_PRESTORED_MESSAGE);
			resp.push_back(a1); // library version number
			resp.push_back(a2);
			resp.push_back(a3);
			resp.push_back(a4);
			// calculate check sum and send the message

			break;
		case TTIS_LIBRARY_DOWNLOAD:
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(TTIS_COMMAND_RECEIVED);
			resp.push_back( pData[23] ); // physical train number
			resp.push_back(13); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(1); // status 5, to do
			resp.push_back(OC2_TTIS_LIBRARY_DOWNLOAD);
			resp.push_back(a1); // library version number
			resp.push_back(a2);
			resp.push_back(a3);
			resp.push_back(a4);
			// calculate check sum and send the message

			break;
		case TTIS_MESSAGE_RESET:
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(TTIS_COMMAND_RECEIVED);
			resp.push_back( pData[23] ); // physical train number
			resp.push_back(13); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(1); // status 5, to do
			resp.push_back(OC2_TTIS_MESSAGE_RESET);
			resp.push_back(a1); // library version number
			resp.push_back(a2);
			resp.push_back(a3);
			resp.push_back(a4);
			// calculate check sum and send the message

			break;
//case END_OF_TTIS_DOWNLOAD:
		case TTIS_LIBRARY_UPGRADE:
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(TTIS_COMMAND_RECEIVED);
			resp.push_back( pData[23] ); // physical train number
			resp.push_back(13); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(1); // status 5, to do
			resp.push_back(OC2_TTIS_LIBRARY_UPGRADE);
			resp.push_back(a1); // library version number
			resp.push_back(a2);
			resp.push_back(a3);
			resp.push_back(a4);
			// calculate check sum and send the message

			break;


		case PEC_ANSWER:
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(PEC_COMMAND_RECEIVED);
			resp.push_back( pData[23] ); // physical train number
			resp.push_back(10); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(pData[26]);
			resp.push_back(pData[27]);
			resp.push_back(1); // status 6, 1=proceeding
			break;

		case PEC_RESET:
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(PEC_RESET_RECEIVED);
			resp.push_back( pData[23] ); // physical train number
			resp.push_back(10); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(pData[26]);
			resp.push_back(pData[27]);
			resp.push_back(1); // status 6, 1=proceeding

			break;
//case PEC_ACTIVATED:
//case PEC_ANSWERED_BY_DRIVER:
//case PEC_COMMAND_RECEIVED:
//case READY_FOR_PEC_CONVERSATION:
         case PEC_HOLD:
           resp.push_back(USER_DATA_FLAG);
			resp.push_back(PEC_ACTIVATED);
			resp.push_back( pData[23] ); // physical train number
			resp.push_back(9); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(pData[26]);
			resp.push_back(pData[27]);
		//	resp.push_back(1); // status 6, 1=proceeding
			break ;
//case ALARM_TABLE:
//case CHANGEOVER_STATUS:
//case TEST_CALL:


/*
		case TETRA_ISCS_MODE:
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(CHANGEOVER_STATUS);
			resp.push_back(pData[24]); // physical train number
			resp.push_back(9); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(0);
			resp.push_back(0);
			break;

		case CHANGE_AREA:
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(CHANGEOVER_STATUS);
			resp.push_back(pData[24]); // physical train number
			resp.push_back(9); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(0);
			resp.push_back(0);
			// calculate check sum and send the message

			break;
			*/
		default: // bad command
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(UNKNOWN_COMMAND);
			resp.push_back(0); // physical train number
			resp.push_back(9); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(pData[23]);
			resp.push_back(2);
			// calculate check sum and send the message

	}
	}

	rawData.clear();
	rawData.insert(rawData.end(), resp.begin()+userDataIndex, resp.end());
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing CRC\n");
	printData(rawData, false);
	calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(rawData,rawData.size());
	tempData = CMHelper::encode_u16(calcCRC);
    //resp.insert(resp.end(), tempData.begin(), tempData.end());
	resp.insert(resp.end(), tempData.at(1));
	resp.insert(resp.end(), tempData.at(0));

	// calculate TL Length
	int decLength = (resp.size()-userDataIndex +6 )*8;
	
	std::vector<unsigned char>::iterator iter;
	iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2;
	resp.insert(iter, (unsigned char)  (decLength & 0xFF00) >> 8);
	iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2+1;
	//resp[CM_SDTS_DATA_TLLENGTH_INDEX] = (decLength & 0xFF00) >> 8;
	resp.insert(iter, (unsigned char)  (decLength & 0x00FF));
	//resp[CM_SDTS_DATA_TLLENGTH_INDEX+1] = (decLength & 0x00FF);	
	// Adaptation length, 2 bytes at beginning
	unsigned int nAdaptation = resp.size();
	tempData = CMHelper::encode_u16(nAdaptation);
	resp.insert( resp.begin(), tempData.begin(), tempData.end() );


	//printf("\nSending Response (Message Type: %x):", resp[23]);
	//for(unsigned int j=0; j<resp.size(); j++)
	//{
		//printf("%3x ", resp[j]);
	//}
	if (showMsg)
	printf("Sending ISCS Response to Radio Agent\n");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending ISCS Response to Radio Agent\n");

	
	printData(resp,false);
	// send the data
	sendData(resp);

	//some command need to send two command

	Sleep ( 100 ) ;

	switch (pData[USER_DATA_COMMAND_INDEX]) {	

		case PA_LIVE_ANNOUNCEMENT:
            {
                std::vector<unsigned char> livaPaReset;
			    buildPAReadyResponse(pData);
                buildLiveRequestResetResponse(pData);
                buildLivePaReset(pData, livaPaReset);
                LivePaReponseThread::LiveRespThreadId threadId = std::make_pair(pData[28], pData[24]);
                LivePaReponseThread* pThread = new LivePaReponseThread(threadId, this, livaPaReset);
                pThread->start();
			    break ;
            }
		case PA_PRERECORDED_ANNOUNCEMENT:
            {
                if ( 1 == pData[27] ) // For normal announcement, reply directly
                {
                    buildPAMessageCompleteResponse(pData);
                }
                else if ( 2 == pData[27]) // For emergency announcement, reply until timeout
                {
                    std::vector<unsigned char> PreRecordedPaTimeOutResp;
                    buildPreRecordedPaTimeOutResponse(pData, PreRecordedPaTimeOutResp);

                    // New a thread to wait abort operation until timeout
                    PreRecordedPaTimeoutThread::PaRecordedTimeoutThreadId threadId = std::make_pair(pData[35], pData[24]); // Nes protocal
                    PreRecordedPaTimeoutThread* pThread = new PreRecordedPaTimeoutThread(threadId, this, PreRecordedPaTimeOutResp);
                    pThread->start();
                }
                break ;
            }
		case PEC_ANSWER:
			buildPecReadyResponse(pData);
			break;
		//case PEC_RESET:
		//	buildPecResetResponse(pData);
		//	break;
        default:
			break ;
	}
    
    LivePaReponseThread::cleanUpUnusefulThread();
    PreRecordedPaTimeoutThread::cleanUpUnusefulThread();

}


void RadioCommsObserver::buildPAReadyResponse(const std::vector<unsigned char>& pData)
{
	std::vector<unsigned char> resp;
	std::vector<unsigned char> tempData;
	int userDataIndex = 0;

	// CM PDU Type
	tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_DATA_REQ);
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Source Address
	tempData = CMHelper::encodeSSI( m_cmData.getDstAddr() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Destination Address
	tempData = CMHelper::encodeSSI( m_cmData.getSrcAddr() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Protocol Identifier
	tempData = CMHelper::encodeCM_ProtocolIdentifier( m_cmData.getProtocolId() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Flags
	resp.push_back(m_cmData.getFlags());
	//resp.insert( resp.end(), m_cmData.getFlags());

	// Message Reference
	resp.push_back(m_cmData.getMsgRef());
	//resp.insert( resp.end(), m_cmData.getMsgRef());

	// Area Slection
	resp.push_back(m_cmData.getAreaSel());
	//resp.insert( resp.end(), m_cmData.getAreaSel());

	// TL Length = 0, to be determined later
	//resp.push_back(0); resp.push_back(0);


	// TL Header
	//resp.push_back(0);
	//resp.push_back(0);
	for (int i = 0; i < m_tlHeader.size(); i+=2) {
		resp.push_back(CMHelper::Hex2Int(m_tlHeader.at(i),1)+CMHelper::Hex2Int(m_tlHeader.at(i+1),0));
	}

	// check CRC16
	std::vector<unsigned char> rawData;
	unsigned short calcCRC;
		userDataIndex = resp.size();
	
			//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(READY_FOR_LIVE_ANNOUNCEMENT);
			resp.push_back( pData[24] ); // physical train number
			resp.push_back(8); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(pData[28]); // Announcement ID
	
			rawData.clear();
	rawData.insert(rawData.end(), resp.begin()+userDataIndex, resp.end());
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Testing CRC\n");
	printData(rawData, false);
	calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(rawData,rawData.size());
	tempData = CMHelper::encode_u16(calcCRC);
	//resp.insert(resp.end(), tempData.begin(), tempData.end());
	resp.insert(resp.end(), tempData.at(1));
	resp.insert(resp.end(), tempData.at(0));

	// calculate TL Length
	int decLength = (resp.size()-userDataIndex +7 )*8;
	
	std::vector<unsigned char>::iterator iter;
	iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2;
	resp.insert(iter, (unsigned char)  (decLength & 0xFF00) >> 8);
	iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2+1;
	//resp[CM_SDTS_DATA_TLLENGTH_INDEX] = (decLength & 0xFF00) >> 8;
	resp.insert(iter, (unsigned char)  (decLength & 0x00FF));
	//resp[CM_SDTS_DATA_TLLENGTH_INDEX+1] = (decLength & 0x00FF);	
	// Adaptation length, 2 bytes at beginning
	unsigned int nAdaptation = resp.size();
	tempData = CMHelper::encode_u16(nAdaptation);
    resp.insert(resp.begin(), tempData.begin(), tempData.end());


	//printf("\nSending Response (Message Type: %x):", resp[23]);
	//for(unsigned int j=0; j<resp.size(); j++)
	//{
		//printf("%3x ", resp[j]);
	//}
	if (showMsg)
	printf("Sending PA Ready Response to Radio Agent:\n");
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending PA Ready Response id = %d to Radio Agent:\n", pData[28] - 0);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending PA Ready Response to Radio Agent:\n");

	
	printData(resp,false);
	// send the data
	sendData(resp);
}

void RadioCommsObserver::buildPAMessageCompleteResponse(const std::vector<unsigned char>& pData)
{
	std::vector<unsigned char> resp;
	std::vector<unsigned char> tempData;
	int userDataIndex = 0;

	// CM PDU Type
	tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_DATA_REQ);
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Source Address
	tempData = CMHelper::encodeSSI( m_cmData.getDstAddr() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Destination Address
	tempData = CMHelper::encodeSSI( m_cmData.getSrcAddr() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Protocol Identifier
	tempData = CMHelper::encodeCM_ProtocolIdentifier( m_cmData.getProtocolId() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Flags
	resp.push_back(m_cmData.getFlags());
	//resp.insert( resp.end(), m_cmData.getFlags());

	// Message Reference
	resp.push_back(m_cmData.getMsgRef());
	//resp.insert( resp.end(), m_cmData.getMsgRef());

	// Area Slection
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Preparing Area Selection");
	resp.push_back(m_cmData.getAreaSel());
	//resp.insert( resp.end(), m_cmData.getAreaSel());

	// TL Length = 0, to be determined later
	//resp.push_back(0); resp.push_back(0);


	// TL Header
	//resp.push_back(0);
	//resp.push_back(0);
	for (int i = 0; i < m_tlHeader.size(); i+=2) {
		resp.push_back(CMHelper::Hex2Int(m_tlHeader.at(i),1)+CMHelper::Hex2Int(m_tlHeader.at(i+1),0));
	}

	// check CRC16
	std::vector<unsigned char> rawData;
	unsigned short calcCRC;
		userDataIndex = resp.size();
	
	//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
	resp.push_back(USER_DATA_FLAG);
	resp.push_back(TRAIN_PA_MESSAGE_COMPLETE);
	resp.push_back( pData[24] ); // physical train number
	resp.push_back(9); // Length
	resp.push_back(0); // Origin, TIMS = 0
    resp.push_back(1); //status 4
	resp.push_back(pData[35]); // Announcement ID //TODO: different protocol

	rawData.clear();
	rawData.insert(rawData.end(), resp.begin()+userDataIndex, resp.end());
	calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(rawData,rawData.size());
	tempData = CMHelper::encode_u16(calcCRC);
    //resp.insert(resp.end(), tempData.begin(), tempData.end());
	resp.insert(resp.end(), tempData.at(1));
	resp.insert(resp.end(), tempData.at(0));

	// calculate TL Length
	int decLength = (resp.size()-userDataIndex +7 )*8;
	
	std::vector<unsigned char>::iterator iter;
	iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2;
	resp.insert(iter, (unsigned char)  (decLength & 0xFF00) >> 8);
	iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2+1;
	//resp[CM_SDTS_DATA_TLLENGTH_INDEX] = (decLength & 0xFF00) >> 8;
	resp.insert(iter, (unsigned char)  (decLength & 0x00FF));
	//resp[CM_SDTS_DATA_TLLENGTH_INDEX+1] = (decLength & 0x00FF);	
	// Adaptation length, 2 bytes at beginning
	unsigned int nAdaptation = resp.size();
	tempData = CMHelper::encode_u16(nAdaptation);
    resp.insert(resp.end(), tempData.begin(), tempData.end());


	//printf("\nSending Response (Message Type: %x):", resp[23]);
	//for(unsigned int j=0; j<resp.size(); j++)
	//{
		//printf("%3x ", resp[j]);
	//}
	if (showMsg)
	printf("Sending PA Message Complete Response to Radio Agent\n");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending PA Message Complete Response to Radio Agent\n");

	
	printData(resp,false);
	// send the data
	sendData(resp);
}

void RadioCommsObserver::buildPecReadyResponse(const std::vector<unsigned char>& pData)
{
	FUNCTION_ENTRY("buildPecReadyResponse");
	std::vector<unsigned char> resp;
	std::vector<unsigned char> tempData;
	int userDataIndex = 0;

	// CM PDU Type
	tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_DATA_REQ);
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Source Address
	tempData = CMHelper::encodeSSI( m_cmData.getDstAddr() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Destination Address
	tempData = CMHelper::encodeSSI( m_cmData.getSrcAddr() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Protocol Identifier
	tempData = CMHelper::encodeCM_ProtocolIdentifier( m_cmData.getProtocolId() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Flags
	resp.push_back(m_cmData.getFlags());
	//resp.insert( resp.end(), m_cmData.getFlags());

	// Message Reference
	resp.push_back(m_cmData.getMsgRef());
	//resp.insert( resp.end(), m_cmData.getMsgRef());

	// Area Slection
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Preparing Area Selection");
	resp.push_back(m_cmData.getAreaSel());
	//resp.insert( resp.end(), m_cmData.getAreaSel());

	// TL Length = 0, to be determined later
	//resp.push_back(0); resp.push_back(0);


	// TL Header
	//resp.push_back(0);
	//resp.push_back(0);
	for (int i = 0; i < m_tlHeader.size(); i+=2) {
		resp.push_back(CMHelper::Hex2Int(m_tlHeader.at(i),1)+CMHelper::Hex2Int(m_tlHeader.at(i+1),0));
	}

	// check CRC16
	std::vector<unsigned char> rawData;
	unsigned short calcCRC;
		userDataIndex = resp.size();
	
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(READY_FOR_PEC_CONVERSATION);
			resp.push_back( pData[24] ); // physical train number
			resp.push_back(9); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(pData[27]);
			resp.push_back(pData[28]);
			//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
	
			rawData.clear();
	rawData.insert(rawData.end(), resp.begin()+userDataIndex, resp.end());
	calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(rawData,rawData.size());
	tempData = CMHelper::encode_u16(calcCRC);
	//resp.insert(resp.end(), tempData.begin(), tempData.end());
	resp.insert(resp.end(), tempData.at(1));
	resp.insert(resp.end(), tempData.at(0));

	// calculate TL Length
	int decLength = (resp.size()-userDataIndex +7 )*8;
	
	std::vector<unsigned char>::iterator iter;
	iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2;
	resp.insert(iter, (unsigned char)  (decLength & 0xFF00) >> 8);
	iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2+1;
	//resp[CM_SDTS_DATA_TLLENGTH_INDEX] = (decLength & 0xFF00) >> 8;
	resp.insert(iter, (unsigned char)  (decLength & 0x00FF));
	//resp[CM_SDTS_DATA_TLLENGTH_INDEX+1] = (decLength & 0x00FF);	
	// Adaptation length, 2 bytes at beginning
	unsigned int nAdaptation = resp.size();
	tempData = CMHelper::encode_u16(nAdaptation);
    resp.insert(resp.end(), tempData.begin(), tempData.end());


	//printf("\nSending Response (Message Type: %x):", resp[23]);
	//for(unsigned int j=0; j<resp.size(); j++)
	//{
		//printf("%3x ", resp[j]);
	//}
	if (showMsg)
	printf("Sending PA Reset Response to Radio Agent\n");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending PEC Ready Response to Radio Agent\n");

	
	printData(resp,false);
	// send the data
	sendData(resp);
	FUNCTION_EXIT;
}

void RadioCommsObserver::buildPreRecordedPaTimeOutResponse(const std::vector<unsigned char>& pData, std::vector<unsigned char>& resp)
{
    
    std::vector<unsigned char> tempData;
    int userDataIndex = 0;
    
    // CM PDU Type
    tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_DATA_REQ);
    resp.insert( resp.end(), tempData.begin(), tempData.end() );
    
    // Source Address
    tempData = CMHelper::encodeSSI( m_cmData.getDstAddr() );
    resp.insert( resp.end(), tempData.begin(), tempData.end() );
    
    // Destination Address
    tempData = CMHelper::encodeSSI( m_cmData.getSrcAddr() );
    resp.insert( resp.end(), tempData.begin(), tempData.end() );
    
    // Protocol Identifier
    tempData = CMHelper::encodeCM_ProtocolIdentifier( m_cmData.getProtocolId() );
    resp.insert( resp.end(), tempData.begin(), tempData.end() );
    
    // Flags
    resp.push_back(m_cmData.getFlags());
    //resp.insert( resp.end(), m_cmData.getFlags());
    
    // Message Reference
    resp.push_back(m_cmData.getMsgRef());
    //resp.insert( resp.end(), m_cmData.getMsgRef());
    
    // Area Slection
    resp.push_back(m_cmData.getAreaSel());
    //resp.insert( resp.end(), m_cmData.getAreaSel());
    
    // TL Length = 0, to be determined later
    //resp.push_back(0); resp.push_back(0);
    
    
    // TL Header
    //resp.push_back(0);
    //resp.push_back(0);
    for (int i = 0; i < m_tlHeader.size(); i+=2) {
        resp.push_back(CMHelper::Hex2Int(m_tlHeader.at(i),1)+CMHelper::Hex2Int(m_tlHeader.at(i+1),0));
    }
    
    // check CRC16
    std::vector<unsigned char> rawData;
    unsigned short calcCRC;
    userDataIndex = resp.size();
    
    //resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
    resp.push_back(USER_DATA_FLAG);
    resp.push_back(TRAIN_PA_MESSAGE_COMPLETE);
    resp.push_back( pData[24] ); // physical train number
    resp.push_back(9); // Length
    resp.push_back(0); // Origin, TIMS = 0
    resp.push_back(2); // status 4 
    resp.push_back(pData[35]); // Announcement ID //TODO: different protocol
    
    rawData.clear();
    rawData.insert(rawData.end(), resp.begin()+userDataIndex, resp.end());
    calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(rawData,rawData.size());
    tempData = CMHelper::encode_u16(calcCRC);
    //resp.insert(resp.end(), tempData.begin(), tempData.end());
	resp.insert(resp.end(), tempData.at(1));
	resp.insert(resp.end(), tempData.at(0));
    
    // calculate TL Length
    int decLength = (resp.size()-userDataIndex +7 )*8;
    
    std::vector<unsigned char>::iterator iter;
    iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2;
    resp.insert(iter, (unsigned char)  (decLength & 0xFF00) >> 8);
    iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2+1;
    //resp[CM_SDTS_DATA_TLLENGTH_INDEX] = (decLength & 0xFF00) >> 8;
    resp.insert(iter, (unsigned char)  (decLength & 0x00FF));
    //resp[CM_SDTS_DATA_TLLENGTH_INDEX+1] = (decLength & 0x00FF);	
    // Adaptation length, 2 bytes at beginning
    unsigned int nAdaptation = resp.size();
    tempData = CMHelper::encode_u16(nAdaptation);
    resp.insert(resp.end(), tempData.begin(), tempData.end());
    
    
    //printf("\nSending Response (Message Type: %x):", resp[23]);
    //for(unsigned int j=0; j<resp.size(); j++)
    //{
    //printf("%3x ", resp[j]);
    //}
    //printf("Sending PA Ready Response to Radio Agent:\n");
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Prepare Pre-recorded PA reset, id = %d(%d)\n", pData[35] - 0, pData[24] - 0);
    
    
    printData(resp,false);
}

void RadioCommsObserver::buildPecResetResponse(const std::vector<unsigned char>& pData)
{
	FUNCTION_ENTRY("buildPecResetResponse");
	std::vector<unsigned char> resp;
	std::vector<unsigned char> tempData;
	int userDataIndex = 0;

	// CM PDU Type
	tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_DATA_REQ);
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Source Address
	tempData = CMHelper::encodeSSI( m_cmData.getDstAddr() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Destination Address
	tempData = CMHelper::encodeSSI( m_cmData.getSrcAddr() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Protocol Identifier
	tempData = CMHelper::encodeCM_ProtocolIdentifier( m_cmData.getProtocolId() );
	resp.insert( resp.end(), tempData.begin(), tempData.end() );

	// Flags
	resp.push_back(m_cmData.getFlags());
	//resp.insert( resp.end(), m_cmData.getFlags());

	// Message Reference
	resp.push_back(m_cmData.getMsgRef());
	//resp.insert( resp.end(), m_cmData.getMsgRef());

	// Area Slection
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Preparing Area Selection");
	resp.push_back(m_cmData.getAreaSel());
	//resp.insert( resp.end(), m_cmData.getAreaSel());

	// TL Length = 0, to be determined later
	//resp.push_back(0); resp.push_back(0);


	// TL Header
	//resp.push_back(0);
	//resp.push_back(0);
	for (int i = 0; i < m_tlHeader.size(); i+=2) {
		resp.push_back(CMHelper::Hex2Int(m_tlHeader.at(i),1)+CMHelper::Hex2Int(m_tlHeader.at(i+1),0));
	}

	// check CRC16
	std::vector<unsigned char> rawData;
	unsigned short calcCRC;
		userDataIndex = resp.size();
	
			//resp.push_back(CMHelper::Hex2Int('0',1)+CMHelper::Hex2Int('7',0));
			resp.push_back(USER_DATA_FLAG);
			resp.push_back(PEC_RESET_RECEIVED);
			resp.push_back( pData[24] ); // physical train number
			resp.push_back(10); // Length
			resp.push_back(0); // Origin, TIMS = 0
			resp.push_back(pData[27]);
			resp.push_back(pData[28]);
			resp.push_back(1); // status 6, 1=proceeding
	
			rawData.clear();
	rawData.insert(rawData.end(), resp.begin()+userDataIndex, resp.end());
	calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(rawData,rawData.size());
	tempData = CMHelper::encode_u16(calcCRC);
	//resp.insert(resp.end(), tempData.begin(), tempData.end());
	resp.insert(resp.end(), tempData.at(1));
	resp.insert(resp.end(), tempData.at(0));

	// calculate TL Length
	int decLength = (resp.size()-userDataIndex +7 )*8;
	
	std::vector<unsigned char>::iterator iter;
	iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2;
	resp.insert(iter, (unsigned char)  (decLength & 0xFF00) >> 8);
	iter = resp.begin() + CM_SDTS_DATA_TLLENGTH_INDEX -2+1;
	//resp[CM_SDTS_DATA_TLLENGTH_INDEX] = (decLength & 0xFF00) >> 8;
	resp.insert(iter, (unsigned char)  (decLength & 0x00FF));
	//resp[CM_SDTS_DATA_TLLENGTH_INDEX+1] = (decLength & 0x00FF);	
	// Adaptation length, 2 bytes at beginning
	unsigned int nAdaptation = resp.size();
	tempData = CMHelper::encode_u16(nAdaptation);
    resp.insert(resp.end(), tempData.begin(), tempData.end());


	//printf("\nSending Response (Message Type: %x):", resp[23]);
	//for(unsigned int j=0; j<resp.size(); j++)
	//{
		//printf("%3x ", resp[j]);
	//}
	if (showMsg)
	printf("Sending PA Reset Response to Radio Agent\n");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending PEC Reset Response to Radio Agent\n");

	
	printData(resp,false);
	// send the data
	sendData(resp);
	FUNCTION_EXIT;	
}




//
//sendData
//
void RadioCommsObserver::sendData(const std::vector<unsigned char> & pData)
{
	FUNCTION_ENTRY("sendData");

	/*
	std::string Data = "Testing send data.";
    m_clientSocket->write(Data.c_str(), Data.length());
	printf("\nSent Data is %s\n", Data.c_str());
	//*/

	/*
	for(unsigned int i=0; i<pData.size(); i++)
		printf("%3c", pData[i]);
	printf("\nSent Data size is %i\n", pData.size());
	//*/
	
	std::ostringstream output;
	std::copy(pData.begin(), pData.end(), std::ostreambuf_iterator<char>(output));
	string sData = output.str();

	
	//printf("\nSent Data length is %i\n", sData.length());

    m_clientSocket->write(sData.c_str(), sData.length());

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
    //LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "LIST_CURRENT_ALARMS_REQUEST received." );
	
	std::string param1;
	std::string param2;
	std::string param3;
	
	unsigned int SSIAddress = 0;
	unsigned long passNumber = 0;
	
	int param = 0;
	for (unsigned int index=0;index<cmdValue.size();index++)
	{
		char   character = cmdValue[index];

		if ((param<2)&&(character==' '))
		{
			param++;
		}
		else
		{
			if (param==0)
			{
				param1+=character;
			}
			if (param==1)
			{
				param2+=character;
			}
			else if (param == 2)
			{
				param3+=character;
			}
		}
	}

	if(param1.size() != 6 || param3.size() != 8)
	{
		FUNCTION_EXIT;
		return false;

	}

	for (unsigned int index=0;index<param1.size();index++)
	{
		char   character = param1[index];
		unsigned int pos = param1.size() - 1 - index;
		unsigned int i = CMHelper::Hex2Int(character, pos);

		SSIAddress += i;
	}

	for (unsigned int index=0;index<param3.size();index++)
	{
		char   character = param3[index];
		unsigned int pos = param3.size() - 1 - index;
		unsigned int i = CMHelper::Hex2Int(character, pos);

		passNumber += i;
	}

	// Temp:
	std::cout << "SSI is " << SSIAddress << " passnumber is " << passNumber << std::endl;	

	RadioIdentity id; 
	id.setSSI(SSIAddress);
	id.setPassNumber(passNumber);

	radioIdMap.insert(pair<RadioIdentity, std::string>(id, ""));

	   
    FUNCTION_EXIT;
	return true;
}

bool RadioCommsObserver::execListISSIPassNum()
{
	FUNCTION_ENTRY("execListISSIPassNum");

	std::cout << "SSI\t" << "Pass Number" << std::endl;
	for(radioId=radioIdMap.begin(); radioId!=radioIdMap.end(); ++radioId)
	{
		RadioIdentity temp = radioId->first;

		std::cout << temp.getSSI() << "\t" << temp.getPassNumber() << std::endl;
	}
	std::cout << std::endl;


	FUNCTION_EXIT;
	return true;

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
	m_cmHeader = header;
}

void RadioCommsObserver::setTLHeader(std::string header)
{
	m_tlHeader = header;
}

std::string RadioCommsObserver::getCMHeader() 
{
	return m_cmHeader;
}

std::string RadioCommsObserver::getTLHeader()
{
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
	if (display) {
		if (showMsg)
		printf("Data received: \n%s\n", data.str().c_str());
	}
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