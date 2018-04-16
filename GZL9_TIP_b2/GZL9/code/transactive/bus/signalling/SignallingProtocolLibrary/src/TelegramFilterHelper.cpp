#include "TelegramFilterHelper.h"

#define SYSTEM_ID_HEADER_POS	0x00
#define MSGFRAME_LEN_POS		0x01
#define MSGFRAME_MF_POS			0x03
#define MSGPACKETLEN_POS		0x04
#define MSGPACKETID_POS			0x06
#define MSGITEM_POS				0x08	// this is where the message start

#define SYSTEM_ID_VAL			0xFF
#define MAX_PACKET_LENGTH		6144

namespace TA_IRS_Bus
{
	TelegramFilterHelper::TelegramFilterHelper(void)
	{
		FUNCTION_ENTRY("TelegramFilterHelper");

		FUNCTION_EXIT;
	}

	TelegramFilterHelper::~TelegramFilterHelper(void)
	{
		FUNCTION_ENTRY("~TelegramFilterHelper");

		FUNCTION_EXIT;
	}
	
	EFilterResult TelegramFilterHelper::filterIncomingData (TelegramContainer & vctrData)
	{
		FUNCTION_ENTRY("filterIncomingData");
		
		EFilterResult filterResult = ENotInserted;

		TelegramContainer vctrDataCopy = vctrData;

		if (validateIncomingData (vctrDataCopy))
		{
			filterResult = insertTelegramToRec(vctrDataCopy);
			vctrData = vctrDataCopy;
		}
		else
		{
			filterResult = EFailure;
		}
		FUNCTION_EXIT;
		
		return filterResult;
	}
	
	bool TelegramFilterHelper::isMultiFlag (TelegramContainer & vctrData)
	{
		FUNCTION_ENTRY("isNotMultiFlag");
		
		bool bResult = false;
		unsigned short usMF = getByte(MSGFRAME_MF_POS,vctrData);
		
		bResult = (bool) usMF;

		FUNCTION_EXIT;

		return bResult;
	}

	bool TelegramFilterHelper::validateIncomingData (TelegramContainer & vctrData)
	{
		FUNCTION_ENTRY("validateIncomingData");

		bool bResult = false;

		// check for system Id
		unsigned char ucSystemId = getByte (SYSTEM_ID_HEADER_POS,vctrData);
		unsigned short usStatedFrameLen = getWord (MSGFRAME_LEN_POS,vctrData);
		unsigned short usStatedMsgLen = getWord (MSGPACKETLEN_POS,vctrData);

		unsigned short usActualLen = vctrData.size();
		
		if (SYSTEM_ID_VAL != ucSystemId ||
			usStatedFrameLen > usActualLen
			)
		{
			
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Error when reading the frame, System ID is wrong or specified length is incorrect");
			bResult = false;
		}
		else
		{
			bResult = true;
		}
		
		
		FUNCTION_EXIT;

		return bResult;
	}

	unsigned short TelegramFilterHelper::getWord(unsigned char position,TelegramContainer & telegramData)
	{
		FUNCTION_ENTRY("getWord");

		unsigned short msgLength = telegramData.size();
		unsigned short retData;

		unsigned char upperByte = *(telegramData.begin() + position);
		unsigned char lowerByte = *(telegramData.begin() + position + 1);

		retData = (upperByte << 8) + lowerByte;

		FUNCTION_EXIT;
		return retData;
	}
	

	unsigned short TelegramFilterHelper::getByte(unsigned char position,TelegramContainer & telegramData)
	{
		FUNCTION_ENTRY("getByte");

		unsigned short msgLength = telegramData.size();
		unsigned char retData;

		retData = telegramData[position];

		FUNCTION_EXIT;
		return retData;
	}

	EFilterResult TelegramFilterHelper::insertTelegramToRec (TelegramContainer & vctrData)
	{
		FUNCTION_ENTRY("insertTelegramToRec");
		
		TelegramContainer telegramData = vctrData;
		unsigned short usMsgId = getWord(MSGPACKETID_POS,vctrData);
		EFilterResult filterResult = ENotInserted;

		bool bMultiFlag = isMultiFlag(telegramData);

		// message is a multi flag append or insert the data on the
		// record
		// or if the multi flag is 0 but the telegram container is
		// not empty
		if (true == bMultiFlag)
		{
			
			// multi flag frame is set
			mergeMsgPacketData(vctrData);
			
			// set result to true consider this merge
			// us insert
			filterResult = EInserted;
			
		}
		else if (!m_telegramContainer.empty () && !bMultiFlag)
		{
			// multi flag is off but there 
			// are data on the container
			// this will be the last data that 
			// will complete the message
			mergeMsgPacketData(vctrData);

			// set result to true consider this merge
			// us insert
			filterResult = EDone;
			m_telegramContainer.clear();
		}
		else if (!bMultiFlag)
		{
			filterResult = ENotInserted;	// result is false meaning no insert has been done
			m_telegramContainer.clear();
		}

		FUNCTION_EXIT;
		
		return filterResult;
	}

	void TelegramFilterHelper::mergeMsgPacketData (TelegramContainer & vctrNewData)
	{
		FUNCTION_ENTRY("mergeMsgPacketData");
		
		unsigned short usStatedOldFrameLen = 0;
		unsigned short usStatedNewFrameLen = getWord (MSGFRAME_LEN_POS,vctrNewData);
		
		if (!m_telegramContainer.empty())
		{
			usStatedOldFrameLen = getWord (MSGFRAME_LEN_POS,m_telegramContainer);
		}
		
		unsigned short usMF = getByte(MSGFRAME_MF_POS,vctrNewData);

		TelegramContainer telegramTemp;
		
		// slice the new message 
		// and put the new message 
		// to the end of the old message
		// but ig the new message is the first one
		// just transfer the new message to the container
		
		if (!m_telegramContainer.empty())
		{

			telegramTemp.insert(telegramTemp.begin(), vctrNewData.begin() + MSGPACKETLEN_POS, vctrNewData.end());
		
			m_telegramContainer.insert(m_telegramContainer.end(),
									   telegramTemp.begin(),
									   telegramTemp.end());

			telegramTemp.clear();
		}
		else	// the container is still empty
		{
			m_telegramContainer.insert(m_telegramContainer.begin(),
									   vctrNewData.begin(),
									   vctrNewData.end());

		}

		// update the fields needed to be update respectively
		// Frame Length
		// Msg Length
		// Multi Flag

		usStatedNewFrameLen = usStatedNewFrameLen + usStatedOldFrameLen;
		setFrameLength(usStatedNewFrameLen);
		setMultiFlag(usMF);
		
		vctrNewData = m_telegramContainer;

		FUNCTION_EXIT;
	}

	void TelegramFilterHelper::setWord(unsigned short position, unsigned short value, TelegramContainer & vctrData)
	{
		FUNCTION_ENTRY("setWord");

		unsigned short size = vctrData.size();
		vctrData[position + 1]     = value&255; //Gets the lower byte of value
		vctrData[position] = value>>8; //Gets the upper byte of value

		FUNCTION_EXIT;
	}
	

	void TelegramFilterHelper::setByte(unsigned short position, unsigned short value, TelegramContainer & vctrData)
	{
		FUNCTION_ENTRY("setWord");

		unsigned short size = vctrData.size();
		vctrData[position] = value;

		FUNCTION_EXIT;
	}

	void TelegramFilterHelper::setFrameLength (unsigned short value)
	{
		FUNCTION_ENTRY("setFrameLength");

		setWord(MSGFRAME_LEN_POS,value,m_telegramContainer);

		FUNCTION_EXIT;
	}

	void TelegramFilterHelper::setMessageLength (unsigned short value, TelegramContainer & vctrData)
	{
		FUNCTION_ENTRY("setMessageLength");

		setWord(MSGPACKETLEN_POS,value,vctrData);

		FUNCTION_EXIT;
	}
	
	void TelegramFilterHelper::setMultiFlag (unsigned short value)
	{
		FUNCTION_ENTRY("setMessageLength");

		setByte(MSGFRAME_MF_POS,value,m_telegramContainer);

		FUNCTION_EXIT;
	}
}
