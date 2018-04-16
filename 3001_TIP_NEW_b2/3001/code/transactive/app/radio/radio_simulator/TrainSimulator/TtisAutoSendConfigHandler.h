#pragma once

#include "AutoSendDataModel.h"
#include "AutoSendDataFactory.h"

class TtisAutoSendConfigHandler
{
	public:
		TtisAutoSendConfigHandler();
		~TtisAutoSendConfigHandler(void);

		void updateConfigFile (IConfigDataItemPtrType autoSendPtr);
		std::string convertToString (bool bConvert);
		IConfigDataItemPtrType getAutoSendingData ();

	private:
		TTISAutoSendInfo m_autoSendInfoTtis;


		static std::string TTISLIBDOWNLOAD;
		static std::string TTISCMDRECVFAILED;
		static std::string AUTOSENDING_SECTION;
		static std::string STR_FALSE;
		static std::string STR_TRUE;
		AutoSendDataFactory m_dataFactory;

};	
