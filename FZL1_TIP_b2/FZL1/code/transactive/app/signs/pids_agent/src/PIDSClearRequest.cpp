/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3002_TIP/3002/transactive/app/signs/pids_agent/src/PIDSClearRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include "app/signs/pids_agent/src/PIDSClearRequest.h"
#include "app/signs/pids_agent/src/PIDSManager.h"
#include "app/signs/pids_agent/src/RawTable.h"

#include <iomanip>

//#include "core/data_access_interface/Pids_agent/src/IDisplayRequest.h"
//#include "core/data_access_interface/Pids_agent/src/DisplayRequestAccessFactory.h"

namespace TA_IRS_App
{


	PIDSClearRequest::PIDSClearRequest()
        : PIDSMessage()
	{
//		m_messageIDString = "M20";
	}


	PIDSClearRequest::~PIDSClearRequest()
	{
	}


	TA_Base_Bus::DataBlock<WORD> PIDSClearRequest::createProtocolMessage()
	{
//		int nLength = MESSAGE_HEADER_LENGTH;
//		RawTable rawTable(MODBUS_START_ADDRESS,nLength+MODBUS_START_ADDRESS-1);			//variable length
		RawTable rawTable(MODBUS_START_ADDRESS,MODBUS_END_ADDRESS);			//fixed length 1020
		if (m_destinationList.length() > 0)
		{
			rawTable.putBitToWord(0,STATION_CLEAR_MESSAGE_BIT_IN_WORD,1);			//Clear Adhoc Message
		} 
		else
		{
			rawTable.putBitToWord(0,STATION_CLEAR_MESSAGE_BIT_IN_WORD,0);			//Clear Adhoc Message
		}
		if (m_trainNo >= 0)
		{
			rawTable.putBitToWord(0,TRAIN_CLEAR_MESSAGE_BIT_IN_WORD,1);			//Clear Adhoc Message
		} 
		else
		{
			rawTable.putBitToWord(0,TRAIN_CLEAR_MESSAGE_BIT_IN_WORD,0);			//Clear Adhoc Message
		}
		//put train info
		if (m_trainNo > 0)
		{
			rawTable.putByte(0,m_trainNo);			//train No
		}
		else if(m_trainNo == 0)
		{
			rawTable.putBitToWord(0,ALL_TRAIN_MESSAGE_BIT_IN_WORD,1);			//Train Message
		}

		for (int i=0; i<m_destinationList.length();i++)		//Location
		{
			int byteIndex = (1 + (m_destinationList[i].station-1)/4)*2;
			int bitNum = ((m_destinationList[i].station-1)*4) % 16;
//			int index = 16 + ((m_destinationList[i].station-1)/5)*16 + ((m_destinationList[i].station-1)%5)*3;
//			rawTable.bitSet(m_destinationList[i].downTrack,index);
//			rawTable.bitSet(m_destinationList[i].upTrack,index+1);
//			rawTable.bitSet(m_destinationList[i].concourse,index+2);
			rawTable.putBitToWord(byteIndex,bitNum,m_destinationList[i].concourse);
			rawTable.putBitToWord(byteIndex,bitNum+1,m_destinationList[i].upTrack);
			rawTable.putBitToWord(byteIndex,bitNum+2,m_destinationList[i].downTrack);
			rawTable.putBitToWord(byteIndex,bitNum+3,m_destinationList[i].gateway);

		}

		return rawTable.getDataBlock();	
	}


	void PIDSClearRequest::log(bool sendSuccess)
	{
/*		TA_ASSERT(m_dataIsSet, "Log() called for Predefined display request that has no data");

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
			" PIDSClearRequest::log() is called");

        TA_IRS_Core::IDisplayRequest* displayRequest = TA_IRS_Core::DisplayRequestAccessFactory::getInstance().createNewDisplayRequest();

        TA_IRS_Core::IDisplayRequest::EMessageType type;
        switch (m_type)
        {
        case TA_Base_Core::RAPIDS_IN_CLEAR:
            type = TA_IRS_Core::IDisplayRequest::RAPIDS_IN_CLEAR;
            break;
        case TA_Base_Core::RAPIDS_OUT_CLEAR:
            type = TA_IRS_Core::IDisplayRequest::RAPIDS_OUT_CLEAR;
     	    break;
        case TA_Base_Core::RAPIDS_IN_NEW:
            TA_ASSERT(false, "Attempting to log a  no RAPIDS approved");
            break;
        case TA_Base_Core::RAPIDS_OUT_NEW:
            TA_ASSERT(false, "Attempting to log a no RAPIDS approved");
            break;
        case TA_Base_Core::RAPIDS_IN_UPDATE:
            TA_ASSERT(false, "Attempting to log a no RAPIDS approved");
            break;
        case TA_Base_Core::RAPIDS_OUT_UPDATE:
            TA_ASSERT(false, "Attempting to log a no RAPIDS approved");
            break;
        case TA_IRS_Core::RAPIDS_APPROVED:
            TA_ASSERT(false, "Attempting to log a RAPIDS approved");
            break;

        }
        displayRequest->setMessageType( type );
		//defectID(17927) AVteam++
		//displayRequest->setAdhocMessageContent("RaPids Clear Message");
		displayRequest->setAdhocMessageContent( " " );
		//defectID(17927) ++AVteam
		displayRequest->setRaPidsTag( m_tag );
		displayRequest->setRaPidsDestination( m_destination );
		displayRequest->setRaPidsOverridable( m_overridable );
		displayRequest->setStartTime( "" );
		displayRequest->setEndTime( "" );
		displayRequest->setTimestamp( m_timestamp );
		//hongran++ TD17502
		displayRequest->setIfSendSuccess(sendSuccess ? TA_Base_Core::Pass : TA_Base_Core::Fail);
		//++hongran TD17502
		
        displayRequest->setDestinationSystem( "PIDS" );

        // operator name for Pids log viewer
        displayRequest->setSubmittedBy( m_operatorName );

		// write this display request to the database
		displayRequest->applyChanges();

		delete displayRequest;
		displayRequest = NULL;
	*/
	}


	void PIDSClearRequest::setData(TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList destinationList, ::CORBA::Long trainNo)
	{
		m_destinationList = destinationList;
		m_trainNo = trainNo;
	}
	
}
