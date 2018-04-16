#pragma once

#include "AutoSendDataModel.h"
#include "AutoSendDataFactory.h"

class PECAutoSendConfigHandler
{
	public:
		PECAutoSendConfigHandler();
		~PECAutoSendConfigHandler(void);

		void updateConfigFile (IConfigDataItemPtrType autoSendPtr);
		std::string convertToString (bool bConvert);
		IConfigDataItemPtrType getAutoSendingData ();

	private:
		PecAutoSendInfo m_autoSendInfoPEC;


		static std::string PECANSWER_KEY;
		static std::string PECHOLD_KEY;
		static std::string PECRESET_KEY;

		static std::string AUTOSENDING_SECTION;
		static std::string STR_FALSE;
		static std::string STR_TRUE;
		
		AutoSendDataFactory m_dataFactory;

};	
