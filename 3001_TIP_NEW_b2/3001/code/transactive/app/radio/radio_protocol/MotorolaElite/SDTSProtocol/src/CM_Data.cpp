
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Data.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Data.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;

namespace TA_IRS_App {

	const int CM_Data::CM_DATA_AREA_INDEX = CM_MESSAGE_REF_INDEX + 1;
	const int CM_Data::CM_DATA_TL_LENGTH_INDEX = CM_MESSAGE_REF_INDEX + 2;

	CM_Data::CM_Data() : areaSelection(0)
	{
		FUNCTION_ENTRY("constructor");

		setCM_PDUType(CMPDUTYPE_DATA_REQ);
		setServiceSelection(CMSERVICESELECTION_INDIVIDUAL);

		FUNCTION_EXIT;
	}

	CM_Data::~CM_Data()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	int CM_Data::getAreaSelection()
	{
		FUNCTION_ENTRY("getAreaSelection");
		FUNCTION_EXIT;
		
		return areaSelection;
	}

	void CM_Data::setAreaSelection(int area)
	{
		FUNCTION_ENTRY("setAreaSelection");
		
		areaSelection = area;

		FUNCTION_EXIT;
	}

	void CM_Data::buildMsg()
	{
		FUNCTION_ENTRY("buildMsg");

		std::vector<unsigned char> rawData;

		std::vector<unsigned char> tempData;

		// PDU type 
		tempData = SDTSHelper::encodeCM_PDUType( getCM_PDUType() );
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		// source ISSI
		tempData = SDTSHelper::encodeSSI( getSourceSSI() );
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		// destination ISSI
		tempData = SDTSHelper::encodeSSI( getDestSSI() );
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		//Protocol Identifier
		tempData = SDTSHelper::encodeCM_ProtocolIdentifier( getProtocolId() );
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		// CM flags
		unsigned char cmFlag = (getSDSType() << 5) + (getServiceSelection() << 4);
		rawData.push_back(cmFlag);

		// Message reference
		rawData.push_back(getMessageRef());

		// Area selection
		rawData.push_back(areaSelection);

		// CM TL length will be assigned after TL PDUs created
		
		unsigned int nAdaptation = rawData.size() - 2;
		tempData = SDTSHelper::encode_u16(nAdaptation);
		rawData.insert( rawData.begin(), tempData.begin(), tempData.end() );
		
		setRawMessage(rawData);

		FUNCTION_EXIT;
	}

	bool CM_Data::parseData( const std::vector<unsigned char>& rawData)
	{
		FUNCTION_ENTRY("parseData");

		// Adaptation layer
		std::vector<unsigned char> vAdaptation(rawData.begin(), rawData.begin()+ADAPTATION_LENGTH);
        unsigned int adaptationLength = SDTSHelper::decode_u16(vAdaptation);

        if (adaptationLength != rawData.size() - ADAPTATION_LENGTH)
        {
			TA_THROW(TA_Base_Core::TransactiveException("Invalid raw data size"));
            return false;
        }
     
		setAdaptationLength(adaptationLength);
		
		//CM PDU Type
		std::vector<unsigned char> vPDUtype(1, rawData[CM_PDUTYPE_INDEX]);
        CMPDUType pduType = SDTSHelper::decodeCM_PDUType(vPDUtype);
        if (pduType != CMPDUTYPE_DATA_REQ)
        {
			TA_THROW(TA_Base_Core::TransactiveException("Invalid CM PDU Type"));
            return false;
        }

		setCM_PDUType(pduType);
			
		//source ISSI
		std::vector<unsigned char> vSourceSSI(rawData.begin()+CM_SOURCE_SSI_INDEX, rawData.begin()+CM_SOURCE_SSI_INDEX+3);
		setSourceSSI(SDTSHelper::decodeSSI(vSourceSSI));

		//dest ISSI
		std::vector<unsigned char> vDestSSI(rawData.begin()+CM_DESTINATION_SSI_INDEX, rawData.begin()+CM_DESTINATION_SSI_INDEX+3);
		setDestSSI(SDTSHelper::decodeSSI(vDestSSI));

		// Protocol Identifier
		std::vector<unsigned char> vPI(1, rawData[CM_PI_INDEX]);
		setProtocolId( SDTSHelper::decodeCM_ProtocolIdentifier(vPI) );

		// SDS Type
		std::vector<unsigned char> vCMFlag(1, rawData[CM_FLAG_INDEX]);
		setSDSType(SDTSHelper::decodeCM_SDSType(vCMFlag));
		
		// Service selection
		setServiceSelection( SDTSHelper::decodeCM_ServiceSelection(vCMFlag));

		// Message Reference
		setMessageRef(rawData[CM_MESSAGE_REF_INDEX]);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Message Reference = %d",rawData[CM_MESSAGE_REF_INDEX]);

		// Area Selection
		setAreaSelection(rawData[CM_DATA_AREA_INDEX]);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Area Selection = %d",rawData[CM_DATA_AREA_INDEX]);

		// TL lengths
		std::vector<unsigned char> vLength(rawData.begin()+CM_DATA_TL_LENGTH_INDEX, rawData.begin()+CM_DATA_TL_LENGTH_INDEX+2);
		setTLlength(SDTSHelper::decode_u16(vLength));

		FUNCTION_EXIT;
        return true;
	}


}
