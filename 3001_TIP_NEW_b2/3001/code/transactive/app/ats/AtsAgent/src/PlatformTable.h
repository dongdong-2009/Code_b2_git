/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/PlatformTable.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * This represents the ATS Platform Table
  */
#ifndef ATSPLATFORMTABLE_H
#define ATSPLATFORMTABLE_H

#include <map>

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"

#include "bus/ats/ats_agent/IDL/src/AtsCommonCorbaDef.h"

#include "app/ats/AtsAgent/src/IAtsTable.h"
#include "app/ats/AtsAgent/src/IIscsTable.h"
#include "app/ats/AtsAgent/src/DataManager.h"

namespace TA_IRS_App
{
	class PlatformTable : public IAtsTable, public IIscsTable
    {
    public:
		/**
		  * Constructor
		  *
		  */
		PlatformTable(unsigned long entityKey,
					  unsigned long locationKey, 
					  TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType,
					  int atsTablesBaseAddress, int iscsTablesBaseAddress,
					  DataManager& dataManager);

		static std::string getStaticType()
		{
			return "PlatformTable";
		};

		//
		// IAtsTable/IIscsTable Interface implementation
		//
		void			importAtsTable(RawTable& rawTable);
		unsigned int	getNumberOfAtsRecords();
		unsigned short	getAtsModbusStartAddress();
		unsigned short	getAtsModbusEndAddress();
		bool			tableReadAllowed();

		void			exportIscsTable(RawTable& rawTable);
		unsigned int	getNumberOfIscsRecords();
		unsigned short	getIscsModbusStartAddress();
		unsigned short	getIscsModbusEndAddress();
		bool			tableWriteAllowed();

	protected:
		void importRecord(RawTable& rawTable, unsigned int recordByteOffset, 
						  TA_IRS_Bus::PlatformIdType platformId);
		void importTrainInfo(RawTable& rawTable, unsigned int trainInfoOffset, unsigned int trainIndex,
							 TA_IRS_Bus::PlatformIdType platformId);
		unsigned short	getAtsModbusRecordSize();
		unsigned short	getAtsModbusSize();

		void exportRecord (RawTable& rawTable, unsigned int recordByteOffset,
						   AtsPsdData& psdData);
		unsigned short	getIscsModbusRecordSize();
		unsigned short	getIscsModbusSize();

    private:
        // Disable default constructor, copy constructor and assignment operator
        PlatformTable();
		PlatformTable( const PlatformTable& atsPlatformTable);
        PlatformTable& operator=(const PlatformTable &);

	protected:

		/** Base address for the tables */
		int									m_atsTablesBaseAddress;
		int									m_iscsTablesBaseAddress;

        /** which location this table is catering for */
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType	
											m_locationType;

		DataManager&						m_dataManager;
    };
}

#endif // !defined(ATSPLATFORMTABLE_H)
