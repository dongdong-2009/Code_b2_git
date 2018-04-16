
#ifndef _BASE_TABLE_PARSER_H_
#define	_BASE_TABLE_PARSER_H_

#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaRawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/IProtocolParser.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataUpdate.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "boost/algorithm/string.hpp"

namespace TA_IRS_Bus
{
	class IAtsDataNotification;
}
namespace TA_IRS_App
{
	class GenaBaseTableParser  : public TA_IRS_Bus::IProtocolParser
	{
	public:
		/**
		 * Constructor
		 */
		GenaBaseTableParser(){};
		/**
		 * Destructor
		 */
		virtual ~GenaBaseTableParser(){};
		
		virtual void parseAtsTable(TA_IRS_Bus::RawTable& rawTable){};

		virtual void parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable)=0;


		/**
		 * setAtsDataNotification
		 *
		 * Method to use to set Data Notification object
		 *
		 * @param atsDataNofic : Data notification object
		 */
		virtual void setAtsDataNotification(TA_IRS_Bus::IAtsDataNotification* atsDataNofic)
		{
			m_atsNotification = atsDataNofic;
		}
	
	private:
		/**
		 * Disable default, copy and assignment constructor
		 */
		GenaBaseTableParser(const GenaBaseTableParser&);
		GenaBaseTableParser& operator= (const GenaBaseTableParser&);

	protected:
		TA_IRS_Bus::IAtsDataNotification* m_atsNotification;

	protected:

		signed short convertStrToEmun(const char ** InfoType,const std::string& data, int num=0)
		{

			for(int i=0; i < num; i++)
			{
				if(boost::to_upper_copy(data)==InfoType[i])
				{
					return i;
				}
			}
			return -1; 
		};

	};
}
#endif