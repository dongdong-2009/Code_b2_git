#include "TelegramFilter.h"

namespace TA_IRS_Bus
{
	
	WrapTelegram::WrapTelegram (std::vector <unsigned char> vctrData) : m_rawData (vctrData)
	{
		FUNCTION_ENTRY ("WrapTelegram");
		FUNCTION_EXIT;
	}
	
	WrapTelegram::~WrapTelegram ()
	{
		m_rawData.clear();
	}

	std::vector <unsigned char> WrapTelegram::getRawData ()
	{
		FUNCTION_ENTRY ("getRawData");
		FUNCTION_EXIT;
		return m_rawData;
	}
	

	TelegramFilter::TelegramFilter(IDataPush * ptrPushDataCalback) 
					: m_ptrPushDataCalback(ptrPushDataCalback)
	{
		FUNCTION_ENTRY ("TelegramFilter");
		start ();
		FUNCTION_EXIT;
	}

	TelegramFilter::~TelegramFilter(void)
	{
		FUNCTION_ENTRY ("TelegramFilter");
		terminateAndWait ();
		FUNCTION_EXIT;
	}

	void TelegramFilter::filterTelegram (std::vector <unsigned char> & vctrData)
	{
		FUNCTION_ENTRY ("filterTelegram");
		
		if (NULL == m_ptrPushDataCalback)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"No Listener");
			FUNCTION_EXIT;
			return;
		}
		
		WrapTelegram * wrapTelegram = new WrapTelegram (vctrData);
		boost::shared_ptr <WrapTelegram> sharedEvent (wrapTelegram);
		insert (sharedEvent);

		FUNCTION_EXIT;
	}

	void TelegramFilter::processEvent (boost::shared_ptr<WrapTelegram> eventItem)
	{
		
		std::vector <unsigned char> vctrDataCopy = eventItem.get()->getRawData();
		
		EFilterResult filterResult = m_telegramFilterHelper.filterIncomingData(vctrDataCopy);

		if (filterResult == ENotInserted || filterResult == EDone)
		{
			m_ptrPushDataCalback->pushData(vctrDataCopy);
		}
		else if (filterResult == EFailure)
		{
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugError,
						"Failure to process incoming telegram, encounter error when checking");
		}

	}
}