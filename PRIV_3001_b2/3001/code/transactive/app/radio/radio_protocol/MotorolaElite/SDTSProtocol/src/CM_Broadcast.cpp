
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Broadcast.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Broadcast.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"

using namespace TA_Base_Core;

namespace TA_IRS_App {

	const int CM_Broadcast::CM_CHANNEL_INDEX = CM_MESSAGE_REF_INDEX + 1;
	const int CM_Broadcast::CM_REPEAT_REQ_INDEX = 14;
	const int CM_Broadcast::CM_REGION_INDEX = 15;
	const int CM_Broadcast::CM_BROADCAST_TL_LENGTH_INDEX = 17;

	CM_Broadcast::CM_Broadcast() 
	: channelSelection(CMCHANNELSELECTIONMASK_MCCH),repeatRequest(CMREPEATREQ_NREPEAT),regionSelection(1)
	{
		FUNCTION_ENTRY("constructor");
		
		setCM_PDUType(CMPDUTYPE_BROADCAST);
		setServiceSelection(CMSERVICESELECTION_GROUP);

		FUNCTION_EXIT;
	}

	CM_Broadcast::~CM_Broadcast()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	CMChannelSelectionMask CM_Broadcast::getChannelSelection()
	{
		return channelSelection;
	}

	CMRepeatRequest CM_Broadcast::getRepeatRequest()
	{
		return repeatRequest;
	}

	unsigned int CM_Broadcast::getRegionSelection()
	{
		return regionSelection;
	}

	void CM_Broadcast::setChannelSelection(CMChannelSelectionMask channel)
	{
		channelSelection = channel;
	}

	void CM_Broadcast::setRepeatRequest(CMRepeatRequest repeatReq)
	{
		repeatRequest = repeatReq;
	}

	void CM_Broadcast::setRegionSelection(unsigned int region)
	{
		regionSelection = region;
	}


	void CM_Broadcast::buildMsg()
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

		// Channel selection
		tempData = SDTSHelper::encodeCM_ChannelSelectionMask( getChannelSelection());
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		// Repeat Request
		tempData = SDTSHelper::encodeCM_RepeatRequest( repeatRequest);
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );
		
		// Region selection
		tempData = SDTSHelper::encode_u16(regionSelection);
		rawData.insert( rawData.end(), tempData.begin(), tempData.end() );

		// TL length will be assigned after creating TL PDU
		
		// Adaptation length
		unsigned int nAdaptation = rawData.size() - 2;
		tempData = SDTSHelper::encode_u16(nAdaptation);
		rawData.insert( rawData.begin(), tempData.begin(), tempData.end() );

		setRawMessage(rawData);

		FUNCTION_EXIT;
	}

	bool CM_Broadcast::parseData(const std::vector<unsigned char>& rawData)
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
        if (pduType != CMPDUTYPE_BROADCAST)
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

		// Channel Selection
		std::vector<unsigned char> channel(1, rawData[CM_CHANNEL_INDEX]);
		setChannelSelection( SDTSHelper::decodeCM_ChannelSelectionMask(channel) );

		// Repeat Request
		std::vector<unsigned char> repeatReq(1, rawData[CM_REPEAT_REQ_INDEX]);
		setRepeatRequest( SDTSHelper::decodeCM_RepeatRequest(repeatReq));
		
		// Region Selection
		std::vector<unsigned char> region(2, rawData[CM_REGION_INDEX]);
		setRegionSelection( SDTSHelper::decode_u16(region));

		// TL lengths
		std::vector<unsigned char> vLength(rawData.begin()+CM_BROADCAST_TL_LENGTH_INDEX, rawData.begin()+CM_BROADCAST_TL_LENGTH_INDEX+2);
		setTLlength(SDTSHelper::decode_u16(vLength));

		FUNCTION_EXIT;
		return true;
	}

}