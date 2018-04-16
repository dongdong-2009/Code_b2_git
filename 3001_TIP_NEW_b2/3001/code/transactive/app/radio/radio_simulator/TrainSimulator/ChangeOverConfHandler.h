#pragma once

#include "AutoSendDataModel.h"
#include "AutoSendDataFactory.h"

class ChangeOverConfHandler
{
	public:
		ChangeOverConfHandler();
		~ChangeOverConfHandler(void);

		void updateConfigFile (IConfigDataItemPtrType autoSendPtr);
		std::string convertToString (bool bConvert);
		IConfigDataItemPtrType getAutoSendingData ();

	private:


		
		static std::string CHANGEOVER_SECTION;
		static std::string STR_FALSE;
		static std::string STR_TRUE;
		AutoSendDataFactory m_dataFactory;
		

		static std::string CHANGEOVER_KEY;
		static std::string CHANGEOVER_TTISKEY; 
		static std::string CHANGEOVER_PECKEY; 
		static std::string CHANGEOVER_PALIVESKEY; 
		static std::string CHANGEOVER_PAPRERECKEY; 
		static std::string CHANGEOVER_TTISUPGRADEKEY; 

};	
