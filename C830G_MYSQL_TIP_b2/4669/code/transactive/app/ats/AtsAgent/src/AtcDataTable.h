/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/AtcDataTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last mofified by:  $Author: builder $
  *
  * This represents the ATS AtcAlarm Table
  */
#ifndef ATCDATATABLE_H
#define ATCDATATABLE_H

#include <map>

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"
#include "bus/ats/ats_agent/IDL/src/IAtsTrainCorbaDef.h"

#include "app/ats/AtsAgent/src/IIscsTable.h"
#include "app/ats/AtsAgent/src/DataManager.h"

namespace TA_Bus
{
    class DataPoint;
}

namespace TA_IRS_App
{
    class AtcDataTable : public IIscsTable
    {
    public:
		/**
		  * Constructor
		  *
		  */
		AtcDataTable(unsigned long locationKey, TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
					 int iscsTablesBaseAddress, DataManager& dataManager);
		virtual ~AtcDataTable();

		static std::string getStaticType()
		{
			return "AtcDataTable";
		};

		//
		// IIscsTable Implementation
		//
		unsigned int	getNumberOfIscsRecords();
		unsigned short	getIscsModbusStartAddress();
		unsigned short	getIscsModbusEndAddress();
		void			exportIscsTable(RawTable& rawTable);
		bool			tableWriteAllowed();

	protected:
		void exportRecord (RawTable& rawTable, unsigned int recordByteOffset, 
			  			   TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber);

		unsigned short	getIscsModbusSize();
		unsigned short	getIscsModbusRecordSize();

    private:
        // Disable default constructor, copy constructor and assignment operator
        AtcDataTable();
		AtcDataTable( const AtcDataTable& iscsAtcAlarmTable);
        AtcDataTable& operator=(const AtcDataTable &);

	protected:

        /** which location this table is catering for */
		unsigned long						m_locationKey;
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType	
											m_locationType;

		/** Base address for all the ATS tables */
		int									m_iscsTablesBaseAddress;

		DataManager&						m_dataManager;
    };
}

#endif
