#pragma once

#include "AutoSendDataModel.h"
#include "AutoSendDataFactory.h"

class PAAutoSendConfigHandler
{
	public:
		PAAutoSendConfigHandler();
		~PAAutoSendConfigHandler(void);

		void updateConfigFile (IConfigDataItemPtrType autoSendPtr);
		std::string convertToString (bool bConvert);
		IConfigDataItemPtrType getAutoSendingData ();

	private:
		PAAutoSendInfo m_autoSendInfoPA;


		static std::string PALIVE_KEY;
		static std::string PAPRERECORDED_KEY;
		static std::string PALIVERESET_KEY;
		static std::string PAPRERECORDEDRESET_KEY;
		static std::string PACONTINUE_KEY;
		static std::string PAPRERECCOMPLETE_KEY;
		static std::string PAPRERECEMERGENCY_KEY;

		static std::string AUTOSENDING_SECTION;
		static std::string STR_FALSE;
		static std::string STR_TRUE;
		
		AutoSendDataFactory m_dataFactory;

};	
