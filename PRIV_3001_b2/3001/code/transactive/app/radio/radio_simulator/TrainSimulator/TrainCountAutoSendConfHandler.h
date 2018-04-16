#pragma once

#include "AutoSendDataModel.h"
#include "AutoSendDataFactory.h"

class TrainCountAutoSendConfHandler
{
	public:
		TrainCountAutoSendConfHandler();
		~TrainCountAutoSendConfHandler(void);

		void updateConfigFile (IConfigDataItemPtrType autoSendPtr);
		std::string convertToString (bool bConvert);
		IConfigDataItemPtrType getAutoSendingData ();

	private:


		
		static std::string TRAINCOUNT_SECTION;
		static std::string STR_FALSE;
		static std::string STR_TRUE;
		AutoSendDataFactory m_dataFactory;
		

		static std::string VALUE_KEY;
};	
