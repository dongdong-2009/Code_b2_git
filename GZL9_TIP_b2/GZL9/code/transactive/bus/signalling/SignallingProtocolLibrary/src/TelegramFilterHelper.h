#pragma once

#include <vector>
#include <map>

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{
	
	typedef std::vector <unsigned char> TelegramContainer;
	typedef unsigned char				MessageId;
	
	enum EFilterResult
	{
		EInserted = 0,		// mf detected inserting
		ENotInserted,		// mf false and no current data on container
		EDone,				// mf is false but there is a data on container, this is the last message
		EFailure			// not inserted because of failure
	};

	class TelegramFilterHelper
	{
		public:
			TelegramFilterHelper(void);
			~TelegramFilterHelper(void);

			EFilterResult filterIncomingData (TelegramContainer & vctrData);
			std::vector <unsigned char> getReadyItem ();
			

		private:
			TelegramContainer m_telegramContainer;

			bool validateIncomingData (TelegramContainer & vctrData);
			EFilterResult insertTelegramToRec (TelegramContainer & vctrData);
			bool isMultiFlag (TelegramContainer & vctrData);
			void mergeMsgPacketData (TelegramContainer & vctrNewData);
			void setFrameLength (unsigned short value);
			void setMessageLength (unsigned short value, TelegramContainer & vctrData);
			void setMultiFlag (unsigned short value);


			// function helper for data extractor
			unsigned short getWord(unsigned char position,TelegramContainer & telegramData);
			unsigned short getByte(unsigned char position,TelegramContainer & telegramData);
			void setWord(unsigned short position, unsigned short value, TelegramContainer  & vctrData);
			void setByte(unsigned short position, unsigned short value, TelegramContainer & vctrData);
	};

}
