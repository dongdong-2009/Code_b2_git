#pragma once

#include "AutoSendDataModel.h"
#include "AutoSendDataFactory.h"

class RadioIdSetupConfigHandler
{
	public:
		RadioIdSetupConfigHandler();
		~RadioIdSetupConfigHandler(void);

		void updateConfigFile (IConfigDataItemPtrType autoSendPtr);
		std::string convertToString (bool bConvert);
		IConfigDataItemPtrType getAutoSendingData ();

	private:
		PAAutoSendInfo m_autoSendInfoPA;


		static std::string RADIOIDSETUP_KEY;
		static std::string RADIOIDSETUP_SECTION;
		static std::string STR_FALSE;
		static std::string STR_TRUE;
		
		AutoSendDataFactory m_dataFactory;

};	
