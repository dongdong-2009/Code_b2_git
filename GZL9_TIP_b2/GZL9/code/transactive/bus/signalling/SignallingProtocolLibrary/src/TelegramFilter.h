#pragma once

#include "core/utilities/src/DebugUtil.h"
#include "bus/signalling/SignallingProtocolLibrary/src/ITelegramFilter.h"
#include "bus/signalling/SignallingProtocolLibrary/src/TelegramFilterHelper.h"
#include "core/threads/src/QueueProcessor.h"

namespace TA_IRS_Bus
{
	class WrapTelegram
	{
		public:
			WrapTelegram (std::vector <unsigned char> vctrData);
			~WrapTelegram ();
			std::vector <unsigned char> getRawData ();
		private:
			std::vector <unsigned char> m_rawData;
	};

	class TelegramFilter : public ITelegramFilter , 
						   public TA_Base_Core::QueueProcessor<WrapTelegram>

	{
		public:
			TelegramFilter(IDataPush * ptrPushDataCalback);
			~TelegramFilter(void);
		
			virtual void filterTelegram (std::vector <unsigned char> & vctrData);
		
		protected:
			void processEvent (boost::shared_ptr<WrapTelegram> eventItem);

		private:
			IDataPush * m_ptrPushDataCalback;
			TelegramFilterHelper m_telegramFilterHelper;
	};

}
