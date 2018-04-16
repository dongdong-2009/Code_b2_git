/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/PIDSClearRequest.cpp $
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
#include "app/signs/pids_agent/src/globaldef.h"

#include <iomanip>

//#include "core/data_access_interface/Pids_agent/src/IDisplayRequest.h"
//#include "core/data_access_interface/Pids_agent/src/DisplayRequestAccessFactory.h"

namespace TA_IRS_App
{


	PIDSClearRequest::PIDSClearRequest()
        : PIDSMessage()
	{
	}


	PIDSClearRequest::~PIDSClearRequest()
	{
	}


	TA_Base_Bus::DataBlock<WORD> PIDSClearRequest::createProtocolMessage()
	{
		RawTable rawTable(MODBUS_START_ADDRESS,MODBUS_END_ADDRESS);			//fixed length 1020
		rawTable.putBitToWord(0,CLEAR_MESSAGE_BIT_IN_WORD,1);			//Clear Adhoc Message
		
		if(m_trainNo >= 0)
		{
			rawTable.putBitToWord(0,TRAIN_MESSAGE_BIT_IN_WORD,1);			// Train select
		}
		else
		{
			rawTable.putBitToWord(0,TRAIN_MESSAGE_BIT_IN_WORD,0);			// No train select
		}

		// train no
		unsigned int trainNo = (m_trainNo == 0 ? ALL_TRAIN_SELECT_VALUE : (m_trainNo > 0 ? m_trainNo : 0));
		rawTable.getDataBlock().set(MODBUS_TRAINNO_MSW_ADDRESS, MSW(trainNo));
		rawTable.getDataBlock().set(MODBUS_TRAINNO_LSW_ADDRESS, LSW(trainNo));

		for (int i=0; i<m_destinationList.length();i++)		//Location
		{
			int byteIndex = (1 + (m_destinationList[i].station-1)/5)*2;
			int bitNum = ((m_destinationList[i].station-1)*3) % 15;
			rawTable.putBitToWord(byteIndex,bitNum,m_destinationList[i].concourse);
			rawTable.putBitToWord(byteIndex,bitNum+1,m_destinationList[i].upTrack);
			rawTable.putBitToWord(byteIndex,bitNum+2,m_destinationList[i].downTrack);
		}

		return rawTable.getDataBlock();	
	}


	void PIDSClearRequest::log(bool sendSuccess)
	{
	}


	void PIDSClearRequest::setData(TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList destinationList, ::CORBA::Long trainNo)
	{
		m_destinationList = destinationList;
		m_trainNo = trainNo;
	}
	
}
