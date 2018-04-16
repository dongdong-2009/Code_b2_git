/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\MotorolaProtocolLib\src\MotorolaSDSProtocolHelper.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/10/08 04:05:25 $
  * Last modified by:  $Author: builder $
  *
  * Helper methods to encode/decode SDTS messages
  */

#include "app/radio/radio_simulator/MotorolaProtocolLib/src/MotorolaSDSProtocolHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMHelper.h"
#include "app/radio/radio_simulator/TrainSimulator/FrameCheckSequence.h"

#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "boost/shared_ptr.hpp"
#include <vector>
#define ISCS_COMMANDS 14

bool operator<(RadioIdentity a, RadioIdentity b)
{
	return a.getSSI()<b.getSSI();
}

namespace TA_IRS_App
{
	unsigned char MotorolaSDSProtocolHelper::_commands[] = { /*TTIS*/0x1F,0x20,0x21,0x23,0x2B,
	                                                 /*PEC*/ 0x33,0x34,0x3C,
	                                                 /*PA*/  0x0B,0x0C,0x0E,0x0F,0x12,
													 /*CHANGE MODE*/ 0x49 };
	MotorolaSDSProtocolHelper::MotorolaSDSProtocolHelper(IRegStatus* regStatus)
		:m_radioCommsObserver(NULL),
		 m_cmDeliveryStatus( CMDELIVERYSTATUS_SUCCESS),
		 m_tlHeader("008000000000"),
		 m_ttisLibVer("00000000"),
		 m_sdsMessageObserver(NULL),
		 m_regStatus(regStatus)

	{
		FUNCTION_ENTRY("MotorolaSDSProtocolHelper");
		FUNCTION_EXIT;
	}

	MotorolaSDSProtocolHelper::~MotorolaSDSProtocolHelper()
	{
		FUNCTION_ENTRY("~MotorolaSDSProtocolHelper");
		FUNCTION_EXIT;
	}

	void MotorolaSDSProtocolHelper::processIncomingSDS( std::vector<unsigned char>& pData )
	{
		FUNCTION_ENTRY("processIncomingSDS");

		switch(pData[CM_PDUTYPE_INDEX_POSITION])
		{
		case CMPDUTYPE_REGISTER_REQ:
			{
				CMRegistrationStatus status;

				m_logViewer->setLogView("Receive Registration request : ");
				m_radioCommsObserver->printData(pData);

				RadioIdentity* id = new RadioIdentity();
				
				if ( !parseRegistrationRequest(pData, id) )
				{
					std::cout << "Cannot parse the registration request!" << std::endl;
					status = CMREGISTERSTATUS_REGISTRATIONNOTAVAILABLE;
				}
				else
					status = processIdentity(id);
				
				buildRegisterResponse(pData,status);

				//update for registration status
				if (NULL != m_regStatus )
				{
					m_regStatus->updateRegStatus(true);
				}

				delete id;
				id = NULL;

			}
			break;

		case CMPDUTYPE_BROADCAST:
			{
			if(!parseCMBroadcast(pData))
				std::cout << "Cannot parse SDTS-CM-BROADCAST message!" << std::endl;
			}
			break;

		case CMPDUTYPE_DATA_REQ:
			{
				

				if ( checkDataControl(pData) && parseCMData(pData))
				{
					m_logViewer->setLogView("Receive Data Request: ");
					m_radioCommsObserver->printData(pData);

					if ( m_cmData.getTLPduType() == TLPDUTYPE_DATA )
					{
						if ( m_cmDeliveryStatus == CMDELIVERYSTATUS_SUCCESS )
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BuildTLReport!");
							m_logViewer->setLogView("BuildTLReport!");
							buildTLReport();
						}
						else {
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BuildCMReport!");
							m_logViewer->setLogView("BuildCMReport!");
							buildCMReport();
						}
						if (USER_DATA_INDEX < pData.size()) {
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "BuildISCSResponse!");
							m_logViewer->setLogView("BuildISCSResponse!");
							buildISCSResponse(pData);
							
						}
					}
				}

			}
			break;
		default:
			//std::cout << "Message with invalid PDU type received!" << std::endl;
			m_logViewer->setLogView("Message with invalid PDU type received!");
		}

		pData.clear();

		FUNCTION_EXIT;
	}

	bool MotorolaSDSProtocolHelper::checkDataControl(const std::vector<unsigned char>& data)
	{
		bool _ret =false;

		if ( data.size() >= 22)
		{
			for ( int count = 0; count < ISCS_COMMANDS/*Accepted Command Lenght*/; count++)
			{
				if ( data[22] == _commands[count] )
				{
					_ret = true;
					break;
				}
			}
		}
		
		return _ret;
	}

	bool MotorolaSDSProtocolHelper::parseRegistrationRequest(const std::vector<unsigned char>& pData, RadioIdentity* id)
	{
		FUNCTION_ENTRY("parseRegistrationRequest");

		// Message length
		if (pData.size() != CM_REGISTER_REQUEST_LENGTH_SIZE)
		{
			std::cout << "Invalid message length for registration request!" << std::endl;
			FUNCTION_EXIT;
			return false;
		}

		// Adaptation layer
		std::vector<unsigned char> vAdaptation(pData.begin(), pData.begin()+ADAPTATION_LENGTH_POSITION);
		int regAdaptationLength = CMHelper::decode_u16(vAdaptation);

		if (regAdaptationLength != CM_REGISTER_REQUEST_LENGTH_SIZE - ADAPTATION_LENGTH_POSITION)
		{
			std::cout << "Invalid adaptation length for registration request!" << std::endl;
			FUNCTION_EXIT;
			return false;
		}

		// CM PDU Type
		std::vector<unsigned char> vPDUtype(1, pData[CM_PDUTYPE_INDEX_POSITION]);
		CMPDUType pduType = CMHelper::decodeCM_PDUType(vPDUtype);
		if (pduType != CMPDUTYPE_REGISTER_REQ)
		{
			std::cout << "Invalid PDU type for registration request!" << std::endl;
			FUNCTION_EXIT;
			return false;
		}

		// Pass Number - 4 bytes
		std::vector<unsigned char> vPassNum(pData.begin() + CM_REG_PASSNUMBER_INDEX_POSITION, 
			pData.begin() + CM_REG_PASSNUMBER_INDEX_POSITION+4);
		unsigned long regPassNum = CMHelper::decode_u32(vPassNum);

		// ISSI - 3 bytes
		std::vector<unsigned char> vClientSSI(pData.begin() + CM_REG_ISSI_INDEX_POSITION, 
			pData.begin() + CM_REG_ISSI_INDEX_POSITION +3);									  
		unsigned int regSSI = CMHelper::decodeSSI(vClientSSI);

		id->setSSI(regSSI);
		id->setPassNumber(regPassNum);	

		FUNCTION_EXIT;

		return true;
	}

	CMRegistrationStatus MotorolaSDSProtocolHelper::processIdentity(RadioIdentity* id)
	{
		FUNCTION_ENTRY("processIdentity");
		CMRegistrationStatus status;

		unsigned int SSI = id->getSSI();
		unsigned long passNumber = id->getPassNumber();
		bool bFound = false;

		// Format socket id
		char buffStr[255] = {0};
		sprintf(buffStr, "%d", SSI);
		std::string socketIdStr=buffStr;

		for(radioId=radioIdMap.begin(); radioId!=radioIdMap.end(); ++radioId)
		{
			RadioIdentity temp = radioId->first;

			// Check ISSI 
			if(temp.getSSI()==SSI)
			{
				bFound = true;

				// Check pass number
				if(temp.getPassNumber()!=passNumber)
				{
					status = CMREGISTERSTATUS_INVALIDPASSNUMBER;
					break;
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
			//status = CMREGISTERSTATUS_INVALIDADDRESS;
			   status = CMREGISTERSTATUS_ADDRESSREGISTERED;

		FUNCTION_EXIT;
		return status;
	}

	bool MotorolaSDSProtocolHelper::execAddISSI(const std::string& cmdValue)
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

		radioIdMap.insert(std::pair<RadioIdentity, std::string>(id, ""));
		
		FUNCTION_EXIT;
		return true;
	}

	bool MotorolaSDSProtocolHelper::execListISSIPassNum()
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

	void MotorolaSDSProtocolHelper::buildRegisterResponse(const std::vector<unsigned char>& pData, CMRegistrationStatus status)
	{
		FUNCTION_ENTRY("buildRegisterResponse");

		std::vector<unsigned char> rawData;
		std::vector<unsigned char> tempData;

		// CM PDU Type
		tempData = CMHelper::encodeCM_PDUType(CMPDUTYPE_REGISTER_RESP);
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		// Pass Number
		rawData.insert( rawData.end(), pData.begin() + CM_REG_PASSNUMBER_INDEX_POSITION, 
			pData.begin() + CM_REG_PASSNUMBER_INDEX_POSITION+4);

		// ISSI Address
		rawData.insert( rawData.end(), pData.begin() + CM_REG_ISSI_INDEX_POSITION, 
			pData.begin() + CM_REG_ISSI_INDEX_POSITION +3);

		// Registration Type
		rawData.insert( rawData.end(), pData.begin() + CM_REG_REGISTRATIONTYPE_INDEX_POSITION, 
			pData.begin() + CM_REG_REGISTRATIONTYPE_INDEX_POSITION +1);

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

		m_logViewer->setLogView("Sending Registration Response: ");
		logger(rawData);
		m_radioCommsObserver->sendData(rawData);

		FUNCTION_EXIT;
	}

	void MotorolaSDSProtocolHelper::registerSocketSendData(IRadioCommsObserver* radioComms)
	{
		FUNCTION_ENTRY("registerSocketSendData");

		m_radioCommsObserver = radioComms;

		FUNCTION_EXIT;
	}

	bool MotorolaSDSProtocolHelper::parseCMBroadcast(const std::vector<unsigned char>& pData)
	{
		FUNCTION_ENTRY("parseBroadcastData");

		// Adaptation layer:
		std::vector<unsigned char> vAdaptation(pData.begin(), pData.begin()+ADAPTATION_LENGTH_POSITION);
		unsigned int AdaptationLength = CMHelper::decode_u16(vAdaptation);

		// Check for packet length
		if( AdaptationLength != pData.size() - ADAPTATION_LENGTH_POSITION)
		{
			FUNCTION_EXIT;
			return false;
		}

		// Get TL Length (in byte)
		std::vector<unsigned char> vTLLength(pData.begin() + CM_SDTS_BROADCAST_TLLENGTH_INDEX_POSITION,
			pData.begin() + CM_SDTS_BROADCAST_TLLENGTH_INDEX_POSITION + TL_LENGTH);
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
		
		std::cout << "Broadcast data received: " <<std::endl;
		if (pduType == TLPDUTYPE_UNITDATA)
		{
			int dataLength = TLLength - CM_SDTS_TL_UNITDATA_INDEX;

			std::vector<unsigned char> vUserData(pData.begin() + CM_SDTS_BRAODCAST_LENGTH + CM_SDTS_TL_UNITDATA_INDEX, 
				pData.begin() + CM_SDTS_BRAODCAST_LENGTH + CM_SDTS_TL_UNITDATA_INDEX + dataLength);

			//TODO
			//displayData(vUserData);
		}

		else if (pduType == TLPDUTYPE_DATA)
		{
			int dataLength = TLLength - CM_SDTS_TL_DATA_INDEX;

			std::vector<unsigned char> vUserData(pData.begin() + CM_SDTS_BRAODCAST_LENGTH + CM_SDTS_TL_DATA_INDEX, 
				pData.begin() + CM_SDTS_BRAODCAST_LENGTH + CM_SDTS_TL_DATA_INDEX + dataLength);

			//TODO
			//displayData(vUserData);
		}

		FUNCTION_EXIT;
		return true;
	}

	bool MotorolaSDSProtocolHelper::parseCMData(const std::vector<unsigned char>& pData)
	{
		FUNCTION_ENTRY("parseCMData");
	
	// Adaptation layer:
	std::vector<unsigned char> vAdaptation(pData.begin(), pData.begin()+ADAPTATION_LENGTH_POSITION);
	unsigned int AdaptationLength = CMHelper::decode_u16(vAdaptation);

	// Check packet length
	if( AdaptationLength != pData.size() - ADAPTATION_LENGTH_POSITION)
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

	void MotorolaSDSProtocolHelper::buildTLReport()
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
		
		m_logViewer->setLogView("Sending TL Response: ");
		logger(rawData);
		m_radioCommsObserver->sendData(rawData);

		FUNCTION_EXIT;
	}

	void MotorolaSDSProtocolHelper::buildCMReport()
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
		m_radioCommsObserver->printData(rawData,false);
		
		m_radioCommsObserver->sendData(rawData);

	}

	void MotorolaSDSProtocolHelper::buildISCSResponse(const std::vector<unsigned char>& pData)
	{
		FUNCTION_ENTRY("buildISCSResponse");

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
		
		//Data flow controller, check if the command exist
		if ( true == checkDataControl(pData) )
		{
			std::vector<unsigned char> _copyData;
			std::vector<unsigned char> _copyRep;
			_copyData.clear();
			_copyRep.clear();
			_copyRep.insert(_copyRep.begin(),resp.begin(),resp.end());
			_copyData.insert(_copyData.begin(),pData.begin(),pData.end());

			boost::shared_ptr<SDSMessageData> sdsMessage(new SDSMessageData(_copyData,_copyRep));

			m_logViewer->setLogView("processSDSMessage..");
			//resp will be updated based on the corresponding train command.
			m_sdsMessageObserver->processSDSMessage(sdsMessage);
		}		
						
		FUNCTION_EXIT;
	}

	void MotorolaSDSProtocolHelper::registerSDSMessage(ISDSMessageObserver* sdsMessageObserver)
	{
		FUNCTION_ENTRY("registerSDSMessage");

		m_sdsMessageObserver = sdsMessageObserver;

		FUNCTION_EXIT;
	}

	CMData& MotorolaSDSProtocolHelper::getCMData()
	{
		FUNCTION_ENTRY("getCMData");
		FUNCTION_EXIT;

		return m_cmData;

	}

	std::string& MotorolaSDSProtocolHelper::getTLHeader()
	{
		FUNCTION_ENTRY("getTLHeader");
		FUNCTION_EXIT;
		return m_tlHeader;
	}

	std::string& MotorolaSDSProtocolHelper::getTTISLibVersion()
	{
		FUNCTION_ENTRY("getTTISLibVersion");
		FUNCTION_EXIT;
		return m_ttisLibVer;
	}

	void MotorolaSDSProtocolHelper::buildMotorolaHeaders(const std::string& pecEventMsg)
	{
		FUNCTION_ENTRY("buildMotorolaHeaders");

		std::vector<unsigned char> _pecEvent = CMHelper::buildMotoralaProtocolHeaders(pecEventMsg);

		logger(_pecEvent);
		//Send the PEC Event Data
		m_radioCommsObserver->sendData(_pecEvent);
		FUNCTION_EXIT;
	}

	void MotorolaSDSProtocolHelper::buildMotorolaHeadersTIMSCommand(const std::vector<unsigned char>& data, bool bIsUsePrimary)
	{
		FUNCTION_ENTRY("buildMotorolaHeadersTIMSCommand");

		std::vector<unsigned char> tmsCommand = CMHelper::buildMotorolaHeadersTIMSCommand(data,bIsUsePrimary);

		logger(tmsCommand);

		//Send TIMS Command
		m_radioCommsObserver->sendData(tmsCommand);
	}

	std::vector<unsigned char>* MotorolaSDSProtocolHelper::buildMotorolaHeadersHeartBeat(const std::vector<unsigned char>& heartBeatProtocol)
	{
		FUNCTION_ENTRY("buildMotorolaHeadersHeartBeat");

		std::vector<unsigned char>* _heartBeatData = CMHelper::buildMotoralaProtocolHeadersHeartBeat(heartBeatProtocol);
		FUNCTION_EXIT;
		return _heartBeatData;
	}

	void MotorolaSDSProtocolHelper::logger(const std::vector<unsigned char>& data)
	{
		std::ostringstream _dataOut;
		_dataOut << std::hex;
		std::vector<unsigned char>::const_iterator iter = data.begin();
		for ( iter; iter != data.end(); iter++ )
		{
			_dataOut << std::setw(2) << std::setfill('0') << (int)(*iter) << " ";

		}
		std::string _dataSend = "Sending Data to Radio Agent : " + _dataOut.str();

		if ( NULL != m_logViewer )
		{
			m_logViewer->setLogView(_dataSend);
		}
		
	}

	void MotorolaSDSProtocolHelper::setLoggerView(IModelLogger* modelLogger)
	{
		FUNCTION_ENTRY("setLoggerView");

		m_logViewer = modelLogger;

		FUNCTION_EXIT;
	}
}
