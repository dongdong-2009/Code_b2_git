/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/DssTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last mofified by:  $Author: builder $
  *
  * This represents the ATS Train Table
  */
#ifndef DSSTABLE_H
#define DSSTABLE_H

#include <map>

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"

#include "app/ats/AtsAgent/src/IAtsTable.h"
#include "app/ats/AtsAgent/src/DataManager.h"

namespace TA_IRS_App
{
    class DssTable : public IAtsTable
    {
    public:
		/**
		  * Constructor
		  *
		  */
		DssTable(TA_Base_Core::AtsAgentEntityDataPtr entityData, DataManager& dataManager);

		static std::string getStaticType()
		{
			return "DssTable";
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
									 std::vector< AtsDssData >& incomingData);
		unsigned short	getAtsModbusRecordSize();


    private:
        // Disable default constructor, copy constructor and assignment operator
        DssTable();
		DssTable( const DssTable& atsDssTable);
        DssTable& operator=(const DssTable &);

	protected:
		
		/** which location this table is catering for */
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType	
											m_locationType;

		/** Base address for all the ATS tables */
		int									m_atsTablesBaseAddress;

		int									m_atsRecordNum;

		DataManager&						m_dataManager;

		std::vector<AtsDssData>				m_lastKnownData;
    };
}

#endif // !defined(ATSPLATFORMTABLE_H)
