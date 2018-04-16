/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/PIDSAdHocDisplayRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include "app/signs/pids_agent/src/PIDSAdHocDisplayRequest.h"
#include "app/signs/pids_agent/src/PIDSManager.h"
#include "app/signs/pids_agent/src/RawTable.h"
#include "app/signs/pids_agent/src/globaldef.h"

#include <iomanip>

namespace TA_IRS_App
{
	PIDSAdHocDisplayRequest::PIDSAdHocDisplayRequest()
		: PIDSMessage()
	{
	}


	PIDSAdHocDisplayRequest::~PIDSAdHocDisplayRequest()
	{
	}


	TA_Base_Bus::DataBlock<WORD> PIDSAdHocDisplayRequest::createProtocolMessage()
	{
		RawTable rawTable(MODBUS_START_ADDRESS,MODBUS_END_ADDRESS);			//fixed length 1020
		rawTable.putBitToWord(0,MESSAGE_PRIORITY_BIT_IN_WORD,m_msgType);	//Message Priority
		rawTable.putBitToWord(0,DISPLAY_LOCATION_BIT_IN_WORD,m_showType);	//Display Location
		rawTable.putBitToWord(0,CLEAR_MESSAGE_BIT_IN_WORD,0);			//Clear Adhoc Message

		if(m_trainNo >= 0)
		{
			rawTable.putBitToWord(0,TRAIN_MESSAGE_BIT_IN_WORD,1);			// Train select
		}
		else
		{
			rawTable.putBitToWord(0,TRAIN_MESSAGE_BIT_IN_WORD,0);			// No train select
		}
		
		for (int i=0; i<m_destinationList.length();i++)		//Location
		{
			int byteIndex = (1 + (m_destinationList[i].station-1)/5)*2;
			int bitNum = ((m_destinationList[i].station-1)%5)*3;
			rawTable.putBitToWord(byteIndex,bitNum,m_destinationList[i].concourse);
			rawTable.putBitToWord(byteIndex,bitNum+1,m_destinationList[i].upTrack);
			rawTable.putBitToWord(byteIndex,bitNum+2,m_destinationList[i].downTrack);
		}
		// train no
		unsigned int trainNo = (m_trainNo == 0 ? ALL_TRAIN_SELECT_VALUE : (m_trainNo > 0 ? m_trainNo : 0));
		rawTable.getDataBlock().set(MODBUS_TRAINNO_MSW_ADDRESS, MSW(trainNo));
		rawTable.getDataBlock().set(MODBUS_TRAINNO_LSW_ADDRESS, LSW(trainNo));

		if(m_isCyclic)
		{
			rawTable.putByte(22, TOBCD(m_startTime.hour));
			rawTable.putByte(23, TOBCD(m_startTime.minute));
			rawTable.putByte(24, TOBCD(m_startTime.second));
			rawTable.putByte(25, TOBCD(m_endTime.hour));
			rawTable.putByte(26, TOBCD(m_endTime.minute));
			rawTable.putByte(27, TOBCD(m_endTime.second));
			rawTable.putByte(28, TOBCD(m_interval/60));
			rawTable.putByte(29, TOBCD(m_interval%60));

		}

		for (int i=0; i<m_messageContent.length();i++)		//Content
		{
			int index = 30 + i;								//Content start byte
			rawTable.putByte(index,m_messageContent[i]);
		}
		return rawTable.getDataBlock();
	}


	void PIDSAdHocDisplayRequest::log(bool sendSuccess)
	{
	}


	void PIDSAdHocDisplayRequest::setData(TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList destinationList, 
		TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage messageContent, 
		::CORBA::Boolean msgType, 
		::CORBA::Boolean showType, 
		::CORBA::Long trainNo, 
		const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& startTime, 
		const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& endTime, 
		::CORBA::Long interval, 
		::CORBA::Boolean isCyclic)
	{
		m_messageContent = messageContent;
		m_destinationList = destinationList;
		m_msgType = msgType;
		m_showType = showType;
		m_trainNo = trainNo;
		m_startTime = startTime;
		m_endTime = endTime;
		m_interval = interval;
		m_isCyclic = isCyclic;

		m_dataIsSet = true;
	}

}