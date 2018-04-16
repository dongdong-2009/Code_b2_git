/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/WildTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This represents the ATS Train Table
  */
#ifndef WILDTABLE_H
#define WILDTABLE_H

#include <map>

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

//#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"

#include "app/ats/AtsAgent/src/DataManager.h"
#include "app/ats/AtsAgent/src/IAtsTable.h"

namespace TA_IRS_App
{
    class WildTable : public IAtsTable
    {
    public:
		/**
		  * Constructor
		  *
		  */
		WildTable(unsigned long entityKey,
					 unsigned long locationKey, TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
					 int atsTablesBaseAddress, DataManager& dataManager);

		static std::string getStaticType()
		{
			return "WildTable";
		};

		//
		// IAtsTable Interface implementation
		//
		unsigned int	getNumberOfAtsRecords();
		unsigned short	getAtsModbusStartAddress();
		unsigned short	getAtsModbusEndAddress();
		unsigned short	getAtsModbusSize();
		void			importAtsTable(RawTable& rawTable);
		bool			tableReadAllowed();

	protected:
		void			importRecord(RawTable& rawTable, unsigned int recordByteOffset,
									 unsigned int index);
		unsigned short	getAtsModbusRecordSize();

    private:
        // Disable default constructor, copy constructor and assignment operator
        WildTable();
		WildTable( const WildTable& atsWildTable);
        WildTable& operator=(const WildTable &);

	protected:

		/** which location this table is catering for */
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType	
											m_locationType;

		/** Base address for all the ATS tables */
		int									m_atsTablesBaseAddress;

		DataManager&						m_dataManager;
    };
}

#endif // !defined(ATSPLATFORMTABLE_H)
