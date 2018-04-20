/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/WashTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last mofified by:  $Author: builder $
  *
  * This represents the ATS Train Table
  */
#ifndef WASHTABLE_H
#define WASHTABLE_H

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

#include "app/ats/AtsAgent/src/IAtsTable.h"
#include "app/ats/AtsAgent/src/IIscsTable.h"
#include "app/ats/AtsAgent/src/DataManager.h"

namespace TA_IRS_App
{
    class WashTable : public IAtsTable, public IIscsTable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		WashTable(unsigned long entityKey, unsigned long locationKey, 
					 TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
					 int atsTablesBaseAddress, int iscsTablesBaseAddress,
					 DataManager& dataManager);

		virtual ~WashTable();

		static std::string getStaticType()
		{
			return "WashTable";
		};

		//
		// IAtsTable/IIscsTable Interface implementation
		//
		unsigned int	getNumberOfAtsRecords();
		unsigned short	getAtsModbusStartAddress();
		unsigned short	getAtsModbusEndAddress();
		void			importAtsTable(RawTable& rawTable);
		bool			tableReadAllowed();

		unsigned int	getNumberOfIscsRecords();
		unsigned short	getIscsModbusStartAddress();
		unsigned short	getIscsModbusEndAddress();
		void			exportIscsTable(RawTable& rawTable);
		bool			tableWriteAllowed();

	protected:
		void			importRecord(RawTable& rawTable, unsigned int recordByteOffset);
		unsigned short	getAtsModbusSize();
		unsigned short	getAtsModbusRecordSize();

		void			exportRecord(RawTable& rawTable, unsigned int recordByteOffset);
		unsigned short	getIscsModbusSize();
		unsigned short	getIscsModbusRecordSize();
		
    private:
        // Disable default constructor, copy constructor and assignment operator
		WashTable();
        WashTable( const WashTable& atsWashRecord);
        WashTable& operator=(const WashTable &);

	protected:

		/** which location this table is catering for */
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType	
											m_locationType;

		/** Base address for all the tables */
		int									m_atsTablesBaseAddress;
		int									m_iscsTablesBaseAddress;

		DataManager&						m_dataManager;
    };
}

#endif
